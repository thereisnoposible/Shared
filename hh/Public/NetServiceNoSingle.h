#pragma once
#include "NetPackNo.h"
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
using boost::asio::ip::tcp;
struct NetConnectNo : public boost::enable_shared_from_this<NetConnectNo>
{
public:
	NetConnectNo(boost::shared_ptr<tcp::socket> pSocket, boost::function<void(boost::shared_ptr<NetPackNo>&)> func)
	{
		bRead = false;
		bWrite = false;
		_pSocket = pSocket;
		_OnGetPack = func;
		_OnBreak = boost::function<void(std::string)>();

		addr = _pSocket->remote_endpoint().address().to_string();
		addr += ":";
		addr += Helper::Int32ToString((int32)_pSocket->remote_endpoint().port());
	}
	NetConnectNo() :_pSocket(nullptr)
	{
		bRead = false;
		bWrite = false;
	}
	~NetConnectNo()
	{
		if (_pSocket != nullptr)
			_pSocket->close();

		//while (bRead == true || bWrite == true)
		//{

		//}
	}

	const std::string& GetReadBuffer()
	{
		return readBuf;
	}

	void CloseSocket()
	{
		_pSocket->close();
		_pSocket = nullptr;
	}

	void SetSocket(boost::shared_ptr<boost::asio::ip::tcp::socket> pSocket)
	{
		_pSocket = pSocket;
	}

	void RegistOnDisConnect(boost::function<void(std::string)> OnBreak)
	{
		_OnBreak = OnBreak;
	}

	boost::asio::ip::tcp::socket& GetSocket()
	{
		return *_pSocket;
	}

	std::string GetAddress()
	{
		return addr;
	}

	void ReadNetBuffer()
	{
		
		memset(buffer_.c_array(), 0, sizeof(buffer_));
		_pSocket->async_read_some(boost::asio::buffer(buffer_),
			boost::bind(&NetConnectNo::ReadReady, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

	}
	void ReadReady(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{	
			bRead = false;
			if (!_OnBreak.empty())
				_OnBreak(addr);		
			return;
		}
//		readBuf += buffer_.data();
		readBuf.append(buffer_.data(), bytes_transferred);
		bool bRet = true;
		while (bRet)
		{
			bRet = false;
			NetPackNo* p = getNetPack();
			p->setAddr(GetAddress());
			boost::shared_ptr<NetPackNo> pack(p);
			if (pack->m_Head.begflag == tagPackHeadNo::PACK_HFLAG && pack->m_Head.endflag == tagPackHeadNo::PACK_EFLAG)
			{
				if (pack->m_Head.datasize == pack->m_pBuff.size())
				{
					_OnGetPack(pack);
					bRet = true;
				}
			}
		}
		ReadNetBuffer();
		
	}

	NetPackNo* GetNetPack()
	{
		return getNetPack();
	}
	void SendBuffer(long messegeid, const ::google::protobuf::Message& mess,long size, long roleid)
	{
		bWrite = true;
		memset(buffer_.c_array(), 0, sizeof(buffer_));
		
		std::string message;
		mess.SerializeToString(&message);
		NetPackNo Pack(messegeid, message.c_str(), size, roleid);
		memcpy(&buffer_, &Pack.m_Head, sizeof(Pack.m_Head));
		int count = sizeof(Pack.m_Head);
		memcpy(&buffer_[count], Pack.m_pBuff.c_str(), Pack.m_pBuff.size());

		_pSocket->async_write_some(boost::asio::buffer(buffer_,count+Pack.getBufferSize()),
			boost::bind(&NetConnectNo::WriteReady, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
		
	}

	void WriteReady(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			return;
		}
		bWrite = false;
	}

	NetPackNo* getNetPack()
	{
		NetPackNo* nPack = new NetPackNo;
		tagPackHeadNo head;
		
		for (; !readBuf.empty();)
		{
			const char* it = readBuf.c_str();

			int count = 0;
			for (int i =0; i<(int)readBuf.size();i++)
			{
				if (*(it + i) == tagPackHeadNo::PACK_HFLAG)
					break;
				else
					count++;
			}
			readBuf.erase(0, count);
			it = readBuf.c_str();
			if (readBuf.empty())
				break;

			if (readBuf.size() < sizeof(tagPackHeadNo))
				break;
			const tagPackHeadNo* pPHead = reinterpret_cast<const tagPackHeadNo*>(it);
			if (pPHead->endflag == tagPackHeadNo::PACK_EFLAG)
			{
				tagPackHeadNo& head = nPack->m_Head;
				memcpy(&head, &(*it), sizeof(tagPackHeadNo));
				unsigned int bagSize = sizeof(tagPackHeadNo)+head.datasize;
				if (readBuf.size() >= bagSize)
				{
					nPack->m_pBuff.assign(readBuf, sizeof(tagPackHeadNo), head.datasize);
					nPack->m_address = GetAddress();
					readBuf.erase(0, bagSize);
				}
				break;
			}
			else
			{
				readBuf.erase(0, 1);
			}
		}
		return nPack;
	}

private:
	std::string readBuf;
	std::string writeBuf;
	boost::shared_ptr<boost::asio::ip::tcp::socket> _pSocket;
	boost::array<char, 128 * 1024> buffer_;
	boost::function<void(boost::shared_ptr<NetPackNo>&)> _OnGetPack;
	boost::function<void(std::string)> _OnBreak;
	std::string addr;
	bool bRead;
	bool bWrite;
};

class NetServiceNo : public MessageHandle<boost::shared_ptr<NetPackNo>>
{
public:
	NetServiceNo(int port, int ionum);
	~NetServiceNo();
	void start();
	void ReceivePack();
	void SendPack(int messegeid, const ::google::protobuf::Message& mess, std::string addr, int playerid);

	void update();

	bool Connect(const std::string& ip, int port);
	void DisConnect(std::string addr);

	void GetNetPack(boost::shared_ptr<NetPackNo>& pPack);
	bool PutGetNetPack(bool bPut, boost::shared_ptr<NetPackNo>& pPack);

	static int GetPort();

	void RegistObserver(NetObserver* observer);

	void UnRegistObserver(NetObserver* observer);

	void OnDisConnect(std::string addr);
	void run();
protected:
	void accept();
	
	void OnConnect(boost::shared_ptr<boost::asio::ip::tcp::socket> psocket, boost::system::error_code ec);
private:
	io_service_poolno _io_service_pool;
	boost::asio::ip::tcp::acceptor* _acceptor;
	boost::mutex _mutex;
	
	boost::shared_ptr<boost::thread> _pthread;
	std::hash_map<std::string, boost::shared_ptr<NetConnectNo>> _connectMap; 
	std::hash_map<std::string, boost::shared_ptr<NetConnectNo>> _connectToMap;
	std::queue<boost::shared_ptr<NetPackNo>> NetPackQueue;

	std::set<NetObserver*> _NetObserverSet;

	static int port;
};