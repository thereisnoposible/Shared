#pragma once
#include "../MessageHandle.h"
#include "NetPack.h"
#include <memory>
#include <boost/thread.hpp>   
#include <boost/aligned_storage.hpp>   
#include <boost/array.hpp>   
#include <boost/bind.hpp>   
#include <boost/enable_shared_from_this.hpp>  
#include <boost/noncopyable.hpp>   
#include <boost/asio.hpp>   
#include <boost/shared_ptr.hpp>
#include <hash_map>
#include <iostream>
#include <queue>
#include <set>
#include "io_service_pool.h"
#include "WebSocketProtocol.h"
#include "../helper/Helper.h"
#include <iostream>

#pragma warning (push)

#pragma warning (disable: 4251)

namespace xlib
{
	struct NetConnect;
	class NetService;

	typedef boost::shared_ptr<NetPack> PackPtr;
	typedef boost::shared_ptr<NetConnect> ConnectPtr;

	using boost::asio::ip::tcp;
	struct NetConnect : public boost::enable_shared_from_this<NetConnect>
	{
	public:
		friend class NetService;
		enum SocketType
		{
			TYPE_SOCKET = 1,
			TYPE_WEBSOCKET,
		};

		NetConnect()
		{

		}
		~NetConnect()
		{

		}

		//const std::string& GetReadBuffer()
		//{
		//	return readBuf;
		//}

		void CloseSocket()
		{
			if (_pSocket.get() != NULL && bClose == false)
			{
				bClose = true;
				_pSocket->get_io_service().post(std::bind(&NetConnect::handle_close, shared_from_this()));
			}

		}

		boost::asio::ip::tcp::socket& GetSocket()
		{
			return *_pSocket.get();
		}

		const char* GetAddress()
		{
			return addr.c_str();
		}

		void Send(long messegeid, const char*pdata, int len, long roleid)
		{
			if (bClose)
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

	private:
		NetConnect(std::shared_ptr<tcp::socket> pSocket, boost::function<void(PackPtr&)> func) :bClose(false)
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

		void SetSocket(std::shared_ptr<boost::asio::ip::tcp::socket> pSocket)
		{
			_pSocket = pSocket;
			bClose = false;
		}

		void RegistOnDisConnect(boost::function<void(ConnectPtr&)> OnBreak)
		{
			_OnBreak = OnBreak;
		}

		void Handshake(int off)
		{
			if (_pSocket)
			_pSocket->async_read_some(boost::asio::buffer(const_cast<char*>(handshake.c_str() + off), handshake.size() - off),
				boost::bind(&NetConnect::OnHandShake, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}

		void ReadHead()
		{
			if (_pSocket)
			boost::asio::async_read(*_pSocket, boost::asio::buffer(&readBuf.m_Head, sizeof(PackHead)),
				boost::bind(&NetConnect::ReadReadyHead, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}

		void OnHandShake(const boost::system::error_code& e, std::size_t bytes_transferred)
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

		void ReadWebPackHead()
		{
			if (_pSocket)
			boost::asio::async_read(*_pSocket, boost::asio::buffer(&webpack.head, sizeof(webpack.head)),
				boost::bind(&NetConnect::ReadWebPackHeadReady, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		void ReadWebPackHeadReady(const boost::system::error_code& e, std::size_t bytes_transferred)
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

		void ReadWebPackHeadBody(int readdata)
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

		void ReadWebPackHeadBodyReady(const boost::system::error_code& e, std::size_t bytes_transferred)
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

		void ReadWebPackData()
		{
			if (_pSocket)
			_pSocket->async_read_some(boost::asio::buffer(const_cast<char*>(webpack.data.c_str()), (int)webpack.datalen + (webpack.head.mask == 1 ? 4 : 0)),
				boost::bind(&NetConnect::ReadWebPackDataReady, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}

		void ReadWebPackDataReady(const boost::system::error_code& e, std::size_t bytes_transferred)
		{
			if (e)
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
			if (webpack.head.mask == 1)
			{
				webpack.mask_key = *(unsigned int*)webpack.data.c_str();
			}
			if (!WebSocketProtocol::decodingDatas(webpack.data.c_str() + (webpack.head.mask == 1 ? 4 : 0),
				webpack.data.size() - (webpack.head.mask == 1 ? 4 : 0), &readBuf, webpack.head.mask, webpack.mask_key))
			{
				//if (!_OnBreak.empty())
				//	_OnBreak(shared_from_this());

				return;
			}

			PackPtr p(new NetPack);
			*p = readBuf;
			p->setAddr(GetAddress());
			p->m_Connect = shared_from_this();
			_OnGetPack(p);

			if (p->getMessageId() == 2100071 && p->m_pBuff.size() == 0)
			{
				int32 couttt = 1;
			}

			readBuf.m_pBuff.clear();
			ReadWebPackHead();
		}

		void ReadReadyHead(const boost::system::error_code& e, std::size_t bytes_transferred)
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
		void ReadBody()
		{
			readBuf.m_pBuff.resize(readBuf.getBufferSize());/*, readBuf.getBufferSize()*/
			if (_pSocket)
			{
				boost::asio::async_read(*_pSocket, boost::asio::buffer(const_cast<char*>(readBuf.m_pBuff.c_str()), readBuf.getBufferSize()),
					boost::bind(&NetConnect::ReadReadyBody, shared_from_this(), boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			}

			//boost::asio::async_read(
			//	*_pSocket,
			//	boost::asio::buffer(const_cast<char*>(readBuf.m_pBuff.c_str()),readBuf.getBufferSize()),
			//	boost::bind(&NetConnect::ReadReadyBody, shared_from_this(), boost::asio::placeholders::error,
			//	boost::asio::placeholders::bytes_transferred));

		}
		void ReadReadyBody(const boost::system::error_code& e, std::size_t bytes_transferred)
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

		//NetPack* GetNetPack()
		//{
		//	return getNetPack();
		//}
		void WriteReady(const boost::system::error_code& e, std::size_t bytes_transferred)
		{
			if (e)
			{
				return;
			}
			//static unsigned long long total = 0;
			//total += bytes_transferred;
			//std::cout << Helper::Timet2String(time(NULL)) << " write buffer:" << bytes_transferred << " total:" << total << "\n";
		}

		void handle_close()
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

	private:
		NetPack readBuf;
		std::string handshake;
		int rsize;
		WebSocketPack webpack;
		std::shared_ptr<boost::asio::ip::tcp::socket> _pSocket;
		//boost::array<char, 128 * 1024> buffer_;
		boost::function<void(PackPtr&)> _OnGetPack;
		boost::function<void(ConnectPtr&)> _OnBreak;
		std::string addr;
		SocketType type;

		bool bClose;
	};

	class XDLL NetObserver
	{
	public:
		virtual void OnConnect(ConnectPtr&) = 0;
		virtual void OnDisConnect(ConnectPtr&) = 0;
	};

	class XDLL NetService : public MessageHandle<PackPtr>
	{
	public:
		NetService(int ionum);
		~NetService();
		void start(int port);
		void stop();

		void update();

		bool Connect(const char* ip, int port, boost::function<void(ConnectPtr&)> sfunc, boost::function<void(ConnectPtr&)> ffunc);
		//void DisConnect(std::string addr);

		void RegistObserver(NetObserver* observer);

		void UnRegistObserver(NetObserver* observer);
		void run();


		void OnDisConnect(ConnectPtr& pConnetct);
	protected:
		void GetNetPack(PackPtr& pPack);

		void accept();

		void OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> psocket,
			boost::function<void(ConnectPtr&)> sfunc, boost::function<void(ConnectPtr&)> ffunc, boost::system::error_code ec);
		void OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> psocket, boost::system::error_code ec);
	private:
		io_service_pool _io_service_pool;
		boost::asio::ip::tcp::acceptor* _acceptor;
		boost::mutex _packmutex;
		boost::mutex _disconnectmutex;

		std::hash_map<ConnectPtr, std::string> _connectMap;

		std::vector<ConnectPtr> newDisConnect;

		std::vector<PackPtr> NetPackVector;
		std::vector<PackPtr> NetPackVectorTemp;

		std::set<NetObserver*> _NetObserverSet;

		bool bClose;
	};
}

#pragma warning (pop)