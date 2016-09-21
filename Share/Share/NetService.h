#pragma once
#include "Singleton.h"
#include "MessageHandle.h"
#include "NetPack.h"
#include <memory>
#include "google/protobuf/message.h"
#include <boost/thread.hpp>   
#include <boost/aligned_storage.hpp>   
#include <boost/array.hpp>   
#include <boost/bind.hpp>   
#include <boost/enable_shared_from_this.hpp>  
#include <boost/noncopyable.hpp>   
#include <boost/shared_ptr.hpp>   
#include <boost/asio.hpp>   
#include <hash_map>
#include <iostream>
#include <queue>
#include <set>
#include "io_service_pool.h"
#include "WebSocketProtocol.h"
#include "Helper.h"

using boost::asio::ip::tcp;
struct NetConnect : public boost::enable_shared_from_this<NetConnect>
{
public:
	enum SocketType
	{
		TYPE_SOCKET = 1,
		TYPE_WEBSOCKET,
	};
	NetConnect(boost::shared_ptr<tcp::socket> pSocket, boost::function<void(boost::shared_ptr<NetPack>&)> func)
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
	NetConnect()
	{

	}
	~NetConnect()
	{
		if (_pSocket.get() != nullptr)
		{
			_pSocket->close();
			_pSocket.reset();
		}
	}

	//const std::string& GetReadBuffer()
	//{
	//	return readBuf;
	//}

	void CloseSocket()
	{
		_pSocket->close();
		_pSocket.reset();
	}

	void SetSocket(boost::shared_ptr<boost::asio::ip::tcp::socket> pSocket)
	{
		_pSocket = pSocket;
	}

	void RegistOnDisConnect(boost::function<void(boost::shared_ptr<NetConnect>&)> OnBreak)
	{
		_OnBreak = OnBreak;
	}

	boost::asio::ip::tcp::socket& GetSocket()
	{
		return *_pSocket.get();
	}

	std::string GetAddress()
	{
		return addr;
	}

	void Handshake(int off)
	{
		_pSocket->async_read_some(boost::asio::buffer(const_cast<char*>(handshake.c_str() + off), handshake.size() - off),
			boost::bind(&NetConnect::OnHandShake, shared_from_this(), boost::asio::placeholders::error,
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
			handshake.resize(480);
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

		if (rsize != handshake.size())
		{
			Handshake(rsize);
			return;
		}
		else
		{
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}		
	}

	void ReadWebPackHead()
	{	
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

		webpack.data.resize(webpack.data.size() + (int)webpack.datalen + (webpack.head.mask == 1 ? 4 : 0));
		ReadWebPackData();
	}

	void ReadWebPackData()
	{
		_pSocket->async_read_some(boost::asio::buffer(const_cast<char*>(webpack.data.c_str()), (int)webpack.datalen + (webpack.head.mask==1?4:0)),
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
			if (!_OnBreak.empty())
				_OnBreak(shared_from_this());
			return;
		}

		boost::shared_ptr<NetPack> p(new NetPack);
		*p = readBuf;
		p->setAddr(GetAddress());
		p->m_Connect = shared_from_this();
		_OnGetPack(p);

		ReadWebPackHead();
	}

	void ReadHead()
	{
		boost::asio::async_read(*_pSocket,boost::asio::buffer(&readBuf.m_Head, sizeof(PackHead)),
			boost::bind(&NetConnect::ReadReadyHead, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

		//boost::asio::async_read(
		//	*_pSocket,
		//	boost::asio::buffer((char*)(&readBuf.m_Head), sizeof(PackHead)),
		//	boost::bind(&NetConnect::ReadReadyHead, shared_from_this(), boost::asio::placeholders::error,
		//	boost::asio::placeholders::bytes_transferred));

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
		boost::asio::async_read(*_pSocket, boost::asio::buffer(const_cast<char*>(readBuf.m_pBuff.c_str()), readBuf.getBufferSize()),
			boost::bind(&NetConnect::ReadReadyBody, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

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
		boost::shared_ptr<NetPack> p(new NetPack);
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
	void Send(long messegeid, const char*pdata ,int len, long roleid)
	{
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
			webpack.head.mask = 0;
			WebSocketProtocol::makeFrame(&Pack, &WritePack, WebSocketProtocol::BINARY_FRAME);

			_pSocket->async_write_some(boost::asio::buffer(WritePack.m_pBuff.c_str(), WritePack.m_pBuff.size()),
				boost::bind(&NetConnect::WriteReady, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
	}

	void SendBuffer(long messegeid, const ::google::protobuf::Message& mess, long roleid)
	{
		std::string message;
		mess.SerializeToString(&message);
		if (type == TYPE_SOCKET)
		{
			NetPack Pack(messegeid, message.c_str(), message.size(), roleid, type);
			_pSocket->async_write_some(boost::asio::buffer(Pack.m_pBuff.c_str(), Pack.m_pBuff.size()),
				boost::bind(&NetConnect::WriteReady, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else if (type == TYPE_WEBSOCKET)
		{
			NetPack Pack(messegeid, message.c_str(), message.size(), roleid, type);
			NetPack WritePack;
			WebSocketProtocol::makeFrame(&Pack, &WritePack, WebSocketProtocol::BINARY_FRAME);

			_pSocket->async_write_some(boost::asio::buffer(WritePack.m_pBuff.c_str(), WritePack.m_pBuff.size()),
				boost::bind(&NetConnect::WriteReady, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		//boost::asio::async_write(
		//	*_pSocket,
		//	boost::asio::buffer(Pack.m_pBuff.c_str(), Pack.m_pBuff.size()),
		//	boost::bind(&NetConnect::WriteReady, shared_from_this(), boost::asio::placeholders::error,
		//	boost::asio::placeholders::bytes_transferred));

	}

	void WriteReady(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			return;
		}
	}

	//NetPack* getNetPack()
	//{
	//	NetPack* nPack = new NetPack;
	//	tagPackHead head;

	//	for (; !readBuf.empty();)
	//	{
	//		const char* it = readBuf.c_str();

	//		int count = 0;
	//		for (int i = 0; i < (int)readBuf.size(); i++)
	//		{
	//			if (*(it + i) == tagPackHead::PACK_HFLAG)
	//				break;
	//			else
	//				count++;
	//		}
	//		readBuf.erase(0, count);
	//		it = readBuf.c_str();
	//		if (readBuf.empty())
	//			break;

	//		if (readBuf.size() < sizeof(tagPackHead))
	//			break;
	//		const tagPackHead* pPHead = reinterpret_cast<const tagPackHead*>(it);
	//		if (pPHead->endflag == tagPackHead::PACK_EFLAG)
	//		{
	//			tagPackHead& head = nPack->m_Head;
	//			memcpy(&head, &(*it), sizeof(tagPackHead));
	//			unsigned int bagSize = sizeof(tagPackHead)+head.datasize;
	//			if (readBuf.size() >= bagSize)
	//			{
	//				nPack->m_pBuff.assign(readBuf, sizeof(tagPackHead), head.datasize);
	//				nPack->m_address = GetAddress();
	//				readBuf.erase(0, bagSize);
	//			}
	//			break;
	//		}
	//		else
	//		{
	//			readBuf.erase(0, 1);
	//		}
	//	}
	//	return nPack;
	//}

private:
	NetPack readBuf;
	std::string handshake;
	int rsize;
	WebSocketProtocol::WebSocketPack webpack;
	boost::shared_ptr<boost::asio::ip::tcp::socket> _pSocket;
	//boost::array<char, 128 * 1024> buffer_;
	boost::function<void(boost::shared_ptr<NetPack>&)> _OnGetPack;
	boost::function<void(boost::shared_ptr<NetConnect>&)> _OnBreak;
	std::string addr;
	SocketType type;
};

class NetObserver
{
public:
	virtual void OnConnect(boost::shared_ptr<NetConnect>&) = 0;
	virtual void OnDisConnect(boost::shared_ptr<NetConnect>&) = 0;
};

class NetService : public Singleton<NetService>, public MessageHandle<boost::shared_ptr<NetPack>>
{
public:
	NetService(int port, int ionum);
	~NetService();
	void start();
	void ReceivePack();
	void SendPack(int messegeid,const ::google::protobuf::Message& mess, std::string addr, int playerid);

	void update();

	bool Connect(const std::string& ip, int port, boost::function<void(boost::shared_ptr<NetConnect>&)> func);
	//void DisConnect(std::string addr);

	void GetNetPack(boost::shared_ptr<NetPack>& pPack);

	void RegistObserver(NetObserver* observer);

	void UnRegistObserver(NetObserver* observer);

	void OnDisConnect(boost::shared_ptr<NetConnect> pConnetct);
	void run();
protected:
	void accept();

	void OnConnect(boost::shared_ptr<boost::asio::ip::tcp::socket> psocket, boost::system::error_code ec);
private:
	io_service_pool _io_service_pool;
	boost::asio::ip::tcp::acceptor* _acceptor;
	boost::mutex _packmutex;
	boost::mutex _connectmutex;
	boost::mutex _disconnectmutex;

	std::hash_map<boost::shared_ptr<NetConnect>,std::string> _connectMap;

	std::vector<boost::shared_ptr<NetConnect>> newConnect;
	std::vector<boost::shared_ptr<NetConnect>> newDisConnect;

	std::vector<boost::shared_ptr<NetPack>> NetPackVector;
	std::vector<boost::shared_ptr<NetPack>> NetPackVectorTemp;

	std::set<NetObserver*> _NetObserverSet;
};

#define  sNetService Singleton<NetService>::Instance();