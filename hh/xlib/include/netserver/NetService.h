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
#include "../include/log/LogService.h"

#pragma warning (push)

#pragma warning (disable: 4251)

namespace xlib
{
	struct NetConnect;
	class NetService;

	typedef boost::shared_ptr<NetPack> PackPtr;
	typedef boost::shared_ptr<NetConnect> ConnectPtr;

	using boost::asio::ip::tcp;
	struct XDLL NetConnect : public boost::enable_shared_from_this<NetConnect>
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

		boost::asio::ip::tcp::socket* GetSocket();

		const char* GetAddress();

		void Send(long messegeid, const char*pdata, int len, long roleid);
	private:
		NetConnect(std::shared_ptr<tcp::socket> pSocket, boost::function<void(PackPtr&)> func);

		void SetSocket(std::shared_ptr<boost::asio::ip::tcp::socket> pSocket);

		void RegistOnDisConnect(boost::function<void(ConnectPtr&, const std::string&)> OnBreak);

		void Handshake(int off);
		void OnHandShake(const boost::system::error_code& e, std::size_t bytes_transferred);

		void ReadWebPackHead();
		void ReadWebPackHeadReady(const boost::system::error_code& e, std::size_t bytes_transferred);
		void ReadWebPackHeadBody(int readdata);
		void ReadWebPackHeadBodyReady(const boost::system::error_code& e, std::size_t bytes_transferred);
		void ReadWebPackData();
		void ReadWebPackDataReady(const boost::system::error_code& e, std::size_t bytes_transferred);

		int32 GetNetPackBuffer(std::string& buff, NetPack& netPack);

		void ReadHead();
		void ReadReadyHead(const boost::system::error_code& e, std::size_t bytes_transferred);
		void ReadBody();
		void ReadReadyBody(const boost::system::error_code& e, std::size_t bytes_transferred);

		void WriteReady(const boost::system::error_code& e, std::size_t bytes_transferred);

		void CloseSocket();
		void handle_close();
	private:
		NetPack readBuf;
		std::string handshake;
		int rsize;
		WebSocketPack webpack;
		std::shared_ptr<boost::asio::ip::tcp::socket> _pSocket;
		//boost::array<char, 128 * 1024> buffer_;
		boost::function<void(PackPtr&)> _OnGetPack;
		boost::function<void(ConnectPtr&,const std::string&)> _OnBreak;
		std::string addr;
		SocketType type;

		bool bClose;
	};

	class XDLL NetObserver
	{
	public:
		virtual void OnConnect(ConnectPtr&) = 0;
		virtual void OnDisConnect(ConnectPtr&, const std::string& message) = 0;
	};

	class XDLL NetService : public MessageHandle<PackPtr>
	{
	public:
		NetService(int ionum);
		~NetService();
		void start(int port);
		void stop();

		void update();

		bool Connect(const char* ip, int port, boost::function<void(ConnectPtr&)> sfunc, boost::function<void(ConnectPtr&, const std::string&)> ffunc);
		//void DisConnect(std::string addr);

		void RegistObserver(NetObserver* observer);

		void UnRegistObserver(NetObserver* observer);
		void run();

		void SetLog(std::string& url);

		void OnDisConnect(const ConnectPtr& pConnetct, const std::string& message);
	protected:
		void GetNetPack(PackPtr& pPack);

		void accept();

		void OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> psocket,
			boost::function<void(ConnectPtr&)> sfunc, boost::function<void(ConnectPtr&, const std::string&)> ffunc, boost::system::error_code ec);
		void OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> psocket, boost::system::error_code ec);
	private:
		io_service_pool _io_service_pool;
		boost::asio::ip::tcp::acceptor* _acceptor;
		boost::mutex _packmutex;
		boost::mutex _connectmutex;
		boost::mutex _disconnectmutex;

		std::hash_map<ConnectPtr, std::string> _connectMap;

		std::vector<ConnectPtr> newConnect;
		std::vector<std::pair<ConnectPtr, std::string>> newDisConnect;

		std::vector<PackPtr> NetPackVector;
		std::vector<PackPtr> NetPackVectorTemp;

		std::set<NetObserver*> _NetObserverSet;

		bool bClose;

		LogService2* _loger;
	};
}

#pragma warning (pop)