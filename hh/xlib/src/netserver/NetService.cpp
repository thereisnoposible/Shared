#include "../include/netserver/NetService.h"

namespace xlib
{
	//-------------------------------------------------------------------------------------------
	NetService::NetService(int ionum) : _io_service_pool(ionum)
	{
		run();
	}

	//-------------------------------------------------------------------------------------------
	NetService::~NetService()
	{

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

		_io_service_pool.stop();
	}

	//-------------------------------------------------------------------------------------------
	void NetService::accept()
	{
		std::shared_ptr<tcp::socket> skt(new tcp::socket(_io_service_pool.get_io_service()));

		_acceptor->async_accept(*skt, boost::bind(&NetService::OnConnect, this, skt, _1));
	}

	//-------------------------------------------------------------------------------------------
	void NetService::OnConnect(std::shared_ptr<tcp::socket> psocket, boost::system::error_code ec)
	{
		if (ec)
		{
			if (_acceptor)
				accept();
			return;
		}

		ConnectPtr conn(new NetConnect(psocket, boost::bind(&NetService::GetNetPack, this, _1)));
		conn->RegistOnDisConnect(boost::bind(&NetService::OnDisConnect, this, _1));
		//conn->ReadHead();
		conn->Handshake(0);

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
		boost::mutex::scoped_lock lock(_packmutex);
		NetPackVectorTemp = NetPackVector;
		NetPackVector.clear();
		lock.unlock();

		for (int i = 0; i < (int)NetPackVectorTemp.size(); i++)
			FireMessage(NetPackVectorTemp[i]->getMessageId(), NetPackVectorTemp[i]);
		NetPackVectorTemp.clear();

		boost::mutex::scoped_lock disconnlock(_disconnectmutex);
		for (int i = 0; i < (int)newDisConnect.size(); i++)
		{
			newDisConnect[i]->CloseSocket();
			std::for_each(_NetObserverSet.begin(), _NetObserverSet.end(), boost::bind(&NetObserver::OnDisConnect, _1, newDisConnect[i]));
		}
		newDisConnect.clear();
		disconnlock.unlock();
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
	bool NetService::Connect(const std::string& ip, int port,
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


}
