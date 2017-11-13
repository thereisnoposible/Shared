#include "../include/netserver/NetService.h"

namespace xlib
{
	//-------------------------------------------------------------------------------------------
	NetService::NetService(int ionum) : _io_service_pool(ionum), _acceptor(nullptr), bClose(false)
	{
		_loger = nullptr;
		run();
	}

	//-------------------------------------------------------------------------------------------
	NetService::~NetService()
	{
		if (_loger)
		{
			delete _loger;
			_loger = nullptr;
		}
	}

	//-------------------------------------------------------------------------------------------
	void NetService::start(int port)
	{
		_acceptor = new boost::asio::ip::tcp::acceptor(_io_service_pool.get_io_service(),
			tcp::endpoint(boost::asio::ip::tcp::v4(), port));

		accept();
	}

	//-------------------------------------------------------------------------------------------
	void NetService::stop()
	{
		if (_acceptor != NULL)
		{
			_acceptor->close();
			delete _acceptor;
			_acceptor = nullptr;
		}

		bClose = true;

		for (int32 i = 0; i < (int32)newConnect.size(); i++)
		{
			newConnect[i]->CloseSocket();
		}
		newConnect.clear();

		for (int32 i = 0; i < (int32)newDisConnect.size(); i++)
		{
			newDisConnect[i]->CloseSocket();
		}
		newDisConnect.clear();
		
		_io_service_pool.stop();
	}

	//-------------------------------------------------------------------------------------------
	void NetService::accept()
	{
		std::shared_ptr<tcp::socket> skt(new tcp::socket(_io_service_pool.get_io_service()));

		if (_acceptor)
		{
			_acceptor->async_accept(*skt, boost::bind(&NetService::OnConnect, this, skt, _1));
			if (_loger)
			{
				_loger->LogMessage("start accept");
			}
		}
	}

	//-------------------------------------------------------------------------------------------
	void NetService::OnConnect(std::shared_ptr<tcp::socket> psocket, boost::system::error_code ec)
	{
		if (ec)
		{
			if (_loger)
			{
				_loger->LogMessage("accept failed");
			}
			if (_acceptor && bClose == false)
				accept();			
			return;
		}

		ConnectPtr conn(new NetConnect(psocket, boost::bind(&NetService::GetNetPack, this, _1)));
		conn->RegistOnDisConnect(boost::bind(&NetService::OnDisConnect, this, _1));
		//conn->ReadHead();
		conn->Handshake(0);

		boost::mutex::scoped_lock disconnlock(_connectmutex);
		newConnect.push_back(conn);
		disconnlock.unlock();
		

		if (_loger)
		{
			_loger->LogMessage("accept success addr = %s",conn->GetAddress());
		}
		if (_acceptor)
			accept();
	}

	//-------------------------------------------------------------------------------------------
	void NetService::OnDisConnect(ConnectPtr& pConnetct)
	{
		boost::mutex::scoped_lock lock(_disconnectmutex);
		newDisConnect.push_back(pConnetct);
	}

	//-------------------------------------------------------------------------------------------
	void NetService::update()
	{
		{
			boost::mutex::scoped_lock connlock(_connectmutex);
			auto tmp_newConnect = newConnect;
			newConnect.clear();
			connlock.unlock();

			for (int i = 0; i < (int)tmp_newConnect.size(); i++)
			{
				std::for_each(_NetObserverSet.begin(), _NetObserverSet.end(), boost::bind(&NetObserver::OnConnect, _1, tmp_newConnect[i]));
			}
		}

		boost::mutex::scoped_lock lock(_packmutex);
		NetPackVectorTemp = NetPackVector;
		NetPackVector.clear();
		lock.unlock();

		for (int i = 0; i < (int)NetPackVectorTemp.size(); i++)
		{
			FireMessage(NetPackVectorTemp[i]->getMessageId(), NetPackVectorTemp[i]);
		}
		NetPackVectorTemp.clear();

		boost::mutex::scoped_lock disconnlock(_disconnectmutex);
		auto tmp_newDisConnect = newDisConnect;
		newDisConnect.clear();
		disconnlock.unlock();
		for (int i = 0; i < (int)tmp_newDisConnect.size(); i++)
		{
			tmp_newDisConnect[i]->CloseSocket();
			std::for_each(_NetObserverSet.begin(), _NetObserverSet.end(), boost::bind(&NetObserver::OnDisConnect, _1, tmp_newDisConnect[i]));
		}
		

	}

	//-------------------------------------------------------------------------------------------
	void NetService::SetLog(std::string& url)
	{
		if (_loger == nullptr)
		{
			_loger = new LogService2(url);
		}
	}

	//-------------------------------------------------------------------------------------------
	void NetService::run()
	{
		_io_service_pool.run();
	}

	//-------------------------------------------------------------------------------------------
	void NetService::GetNetPack(PackPtr& pPack)
	{
		boost::mutex::scoped_lock lock(_packmutex);
		NetPackVector.push_back(pPack);
	}

	void NetService::RegistObserver(NetObserver* observer)
	{
		_NetObserverSet.insert(observer);
	}

	void NetService::UnRegistObserver(NetObserver* observer)
	{
		_NetObserverSet.erase(observer);
	}
	//-------------------------------------------------------------------------------------------
	bool NetService::Connect(const char* ip, int port,
		boost::function<void(ConnectPtr&)> sfunc, boost::function<void(ConnectPtr&)> ffunc)
	{
		std::shared_ptr<tcp::socket> skt(new tcp::socket(_io_service_pool.get_io_service()));
		tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), port);
		boost::system::error_code error;

		skt->async_connect(endpoint, boost::bind(&NetService::OnConnect, this, skt, sfunc, ffunc, _1));

		return true;
	}

	//-------------------------------------------------------------------------------------------
	void NetService::OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> psocket,
		boost::function<void(ConnectPtr&)> sfunc, boost::function<void(ConnectPtr&)> ffunc, boost::system::error_code ec)
	{
		if (ec)
		{
			ffunc(ConnectPtr());
			return;
		};

		ConnectPtr conn(new NetConnect(psocket, boost::bind(&NetService::GetNetPack, this, _1)));

		conn->RegistOnDisConnect(ffunc);
		conn->ReadHead();

		sfunc(conn);
	}


	//-------------------------------------------------------------------------------------------

	NetConnect::NetConnect(std::shared_ptr<tcp::socket> pSocket, boost::function<void(PackPtr&)> func) :bClose(false)
	{
		_pSocket = pSocket;
		_OnGetPack = func;
		//		_OnBreak = boost::function<void(std::string)>();

		addr = _pSocket->remote_endpoint().address().to_string();
		addr += ":";
		char a[10] = { 0 };
		sprintf_s(a, "%d", _pSocket->remote_endpoint().port());
		addr += a;

		handshake.resize(sizeof(PackHead));
		rsize = 0;

		type = TYPE_SOCKET;
	}

	void NetConnect::SetSocket(std::shared_ptr<boost::asio::ip::tcp::socket> pSocket)
	{
		_pSocket = pSocket;
		bClose = false;
	}

	void NetConnect::RegistOnDisConnect(boost::function<void(ConnectPtr&)> OnBreak)
	{
		_OnBreak = OnBreak;
	}

	void NetConnect::CloseSocket()
	{
		if (_pSocket.get() != NULL && bClose == false)
		{
			bClose = true;
			_pSocket->get_io_service().post(std::bind(&NetConnect::handle_close, shared_from_this()));
		}

	}

	boost::asio::ip::tcp::socket& NetConnect::GetSocket()
	{
		return *_pSocket.get();
	}

	const char* NetConnect::GetAddress()
	{
		return addr.c_str();
	}

	void NetConnect::Send(long messegeid, const char*pdata, int len, long roleid)
	{
		if (bClose)
			return;

		if (_pSocket == nullptr)
			return;

		if (type == TYPE_SOCKET)
		{
			NetPack Pack(messegeid, pdata, len, roleid, type);
			_pSocket->async_write_some(boost::asio::buffer(Pack.m_pBuff.c_str(), Pack.m_pBuff.size()),
				boost::bind(&NetConnect::WriteReady, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else if (type == TYPE_WEBSOCKET)
		{
			NetPack Pack(messegeid, pdata, len, roleid, type);
			NetPack WritePack;
			//webpack.head.mask = 0;
			WebSocketProtocol::makeFrame(&Pack, &WritePack, WebSocketProtocol::BINARY_FRAME);

			_pSocket->async_write_some(boost::asio::buffer(WritePack.m_pBuff.c_str(), WritePack.m_pBuff.size()),
				boost::bind(&NetConnect::WriteReady, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
	}

	void NetConnect::Handshake(int off)
	{
		if (_pSocket)
			_pSocket->async_read_some(boost::asio::buffer(const_cast<char*>(handshake.c_str() + off), handshake.size() - off),
			boost::bind(&NetConnect::OnHandShake, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void NetConnect::OnHandShake(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}
		//		readBuf += buffer_.data();
		rsize += bytes_transferred;

		if (rsize < sizeof(PackHead))
		{
			Handshake(rsize);
			return;
		}

		const PackHead* pHead = (const PackHead*)handshake.c_str();
		if (pHead->begflag == tagPackHead::PACK_HFLAG && pHead->endflag == tagPackHead::PACK_EFLAG)
		{
			readBuf.m_Head = *pHead;
			ReadBody();
			type = TYPE_SOCKET;
			return;
		}

		if (rsize == sizeof(PackHead))
		{
			handshake.resize(1000);
		}

		if (WebSocketProtocol::isWebSocketProtocol(handshake.c_str(), rsize))
		{
			type = TYPE_WEBSOCKET;
			if (!WebSocketProtocol::handshake(_pSocket, handshake.c_str(), rsize))
			{
				if (!_OnBreak.empty())
					_OnBreak(shared_from_this());
				return;
			}
			ReadWebPackHead();
			return;
		}
		else
		{
			Handshake(rsize);
		}
		/*		else
		{
		if (!_OnBreak.empty())
		_OnBreak(shared_from_this());
		return;
		}	*/
	}

	void NetConnect::ReadHead()
	{
		if (_pSocket)
			boost::asio::async_read(*_pSocket, boost::asio::buffer(&readBuf.m_Head, sizeof(PackHead)),
			boost::bind(&NetConnect::ReadReadyHead, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void NetConnect::ReadReadyHead(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}
		//		readBuf += buffer_.data();

		if (readBuf.m_Head.begflag != tagPackHead::PACK_HFLAG || readBuf.m_Head.endflag != tagPackHead::PACK_EFLAG)
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}
		ReadBody();
	}

	void NetConnect::ReadBody()
	{
		readBuf.m_pBuff.resize(readBuf.getBufferSize());/*, readBuf.getBufferSize()*/
		if (_pSocket)
		{
			boost::asio::async_read(*_pSocket, boost::asio::buffer(const_cast<char*>(readBuf.m_pBuff.c_str()), readBuf.getBufferSize()),
				boost::bind(&NetConnect::ReadReadyBody, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
	}

	void NetConnect::ReadReadyBody(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}
		//		readBuf += buffer_.data();
		PackPtr p(new NetPack);
		*p = readBuf;
		p->setAddr(GetAddress());
		p->m_Connect = shared_from_this();
		_OnGetPack(p);
		ReadHead();
	}

	void NetConnect::ReadWebPackHead()
	{
		if (_pSocket)
			boost::asio::async_read(*_pSocket, boost::asio::buffer(&webpack.head, sizeof(webpack.head)),
			boost::bind(&NetConnect::ReadWebPackHeadReady, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void NetConnect::ReadWebPackHeadReady(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}

		if (webpack.head.opcode == WebSocketProtocol::CLOSE_FRAME)
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}

		int readdata = 0;
		int readmask = 0;

		if (webpack.head.payloadlen <= 125)
		{
			webpack.high_low.low = webpack.head.payloadlen;
		}
		else if (webpack.head.payloadlen == 126)
			readdata = 2;
		else if (webpack.head.payloadlen == 127)
			readdata = 8;

		ReadWebPackHeadBody(readdata);
	}

	void NetConnect::ReadWebPackHeadBody(int readdata)
	{
		char* p = NULL;

		if (readdata == 2)
			p = reinterpret_cast<char*>(&webpack.high_low.low);
		else if (readdata == 8)
			p = reinterpret_cast<char*>(&webpack.datalen);

		if (_pSocket)
			boost::asio::async_read(*_pSocket, boost::asio::buffer(p, readdata),
			boost::bind(&NetConnect::ReadWebPackHeadBodyReady, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void NetConnect::ReadWebPackHeadBodyReady(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}

		if (webpack.head.payloadlen == 126)
			Helper::apply(&webpack.high_low.low);
		else if (webpack.head.payloadlen == 127)
			Helper::apply(&webpack.datalen);

		//webpack.data.resize(webpack.data.size() + (int)webpack.datalen + (webpack.head.mask == 1 ? 4 : 0));
		webpack.data.resize((int)webpack.datalen + (webpack.head.mask == 1 ? 4 : 0));
		ReadWebPackData();
	}

	void NetConnect::ReadWebPackData()
	{
		if (_pSocket)
			_pSocket->async_read_some(boost::asio::buffer(const_cast<char*>(webpack.data.c_str()), (int)webpack.datalen + (webpack.head.mask == 1 ? 4 : 0)),
			boost::bind(&NetConnect::ReadWebPackDataReady, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	int32 NetConnect::GetNetPackBuffer(std::string& buff, NetPack& netPack)
	{
		if (buff.size() < sizeof(PackHead))
			return 1;

		memcpy(&netPack.m_Head, buff.c_str(), sizeof(PackHead));
		if (netPack.m_Head.begflag != tagPackHead::PACK_HFLAG || netPack.m_Head.endflag != tagPackHead::PACK_EFLAG)
			return 2;

		if (buff.size() < sizeof(PackHead) + netPack.m_Head.datasize)
			return 1;

		netPack.m_pBuff.clear();
		netPack.m_pBuff.assign(buff.c_str() + sizeof(PackHead), netPack.m_Head.datasize);

		buff.erase(buff.begin(), buff.begin() + sizeof(PackHead) + netPack.m_Head.datasize);
		return 0;
	}

	void NetConnect::ReadWebPackDataReady(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}

		if (webpack.head.mask == 1)
		{
			webpack.mask_key = *(unsigned int*)webpack.data.c_str();
		}

		int32 off = webpack.total_data.size();
		int32 buffer_length = webpack.data.size() - (webpack.head.mask == 1 ? 4 : 0);
		webpack.total_data.resize(off + buffer_length);
		if (!WebSocketProtocol::decodingDatas(webpack.data.c_str() + (webpack.head.mask == 1 ? 4 : 0), buffer_length,
			const_cast<char*>(webpack.total_data.c_str()) + off, webpack.head.mask, webpack.mask_key))
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());

			return;
		}

		NetPack temp_pack;
		int32 result = GetNetPackBuffer(webpack.total_data, temp_pack);
		while (result == 0)
		{
			PackPtr p(new NetPack);
			*p = temp_pack;
			p->setAddr(GetAddress());
			p->m_Connect = shared_from_this();
			_OnGetPack(p);

			temp_pack = NetPack();

			result = GetNetPackBuffer(webpack.total_data, temp_pack);
		}

		if (result == 2)
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}

		if (webpack.head.fin == 0)
		{
			ReadWebPackHead();
			return;
		}

		//if (!WebSocketProtocol::decodingDatas(webpack.data.c_str() + (webpack.head.mask == 1 ? 4 : 0),
		//	webpack.data.size() - (webpack.head.mask == 1 ? 4 : 0), &readBuf, webpack.head.mask, webpack.mask_key))
		//{
		//	if (!_OnBreak.empty())
		//		_OnBreak(shared_from_this());

		//	return;
		//}

		readBuf.m_pBuff.clear();
		ReadWebPackHead();
	}

	void NetConnect::WriteReady(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			return;
		}
	}

	void NetConnect::handle_close()
	{
		try{
			boost::system::error_code ignored_ec;
			_pSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			_pSocket->close();
			_pSocket.reset();
		}
		catch (...)
		{
			//Helper::dbgTrace("socket error when close socket");
		}

	}

}
