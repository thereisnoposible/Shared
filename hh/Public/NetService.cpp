#include "Singleton.h"
#include "NetService.h"

//INSTANTIATE_SINGLETON_1(NetService);
NetService* Singleton<NetService>::single = nullptr;
//-------------------------------------------------------------------------------------------
NetService::NetService(int port, int ionum) : _io_service_pool(ionum)
{
	_acceptor = new boost::asio::ip::tcp::acceptor(_io_service_pool.get_io_service(), tcp::endpoint(tcp::v4(), port));
	accept();
}

//-------------------------------------------------------------------------------------------
NetService::~NetService()
{
	_io_service_pool.stop();

	if (_acceptor != NULL)
		delete _acceptor;
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
		return;
	ConnectPtr conn(new NetConnect(psocket, boost::bind(&NetService::GetNetPack, this, _1)));
	conn->RegistOnDisConnect(boost::bind(&NetService::OnDisConnect, this, _1));
	//conn->ReadHead();
	conn->Handshake(0);

	boost::mutex::scoped_lock connlock(_connectmutex);	
	newConnect.push_back(conn);
	connlock.unlock();
	//std::set<NetObserver*>::iterator setit = _NetObserverSet.begin();
	//for (; setit != _NetObserverSet.end(); setit++)
	//{
	//	(*setit)->OnConnect(conn);
	//}
	static int i = 0;
	++i;
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

	for (int i = 0; i < (int)NetPackVectorTemp.size();i++)
		FireMessage(NetPackVectorTemp[i]->getMessageId(), NetPackVectorTemp[i]);
	NetPackVectorTemp.clear();

	boost::mutex::scoped_lock connlock(_connectmutex);
	for (int i = 0; i < (int)newConnect.size();++i)
	{
		std::set<NetObserver*>::iterator setit = _NetObserverSet.begin();
		for (; setit != _NetObserverSet.end(); setit++)
		{
			(*setit)->OnConnect(newConnect[i]);
		}
		_connectMap.insert(std::make_pair(newConnect[i], newConnect[i]->GetAddress()));
	}
	newConnect.clear();
	connlock.unlock();

	boost::mutex::scoped_lock disconnlock(_disconnectmutex);
	for (int i=0; i < (int)newDisConnect.size(); i++)
	{		
		std::set<NetObserver*>::iterator setit = _NetObserverSet.begin();
		for (; setit != _NetObserverSet.end(); setit++)
		{
			(*setit)->OnDisConnect(newDisConnect[i]);
		}
		_connectMap.erase(newDisConnect[i]);
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
bool NetService::Connect(const std::string& ip, int port, boost::function<void(ConnectPtr&)> func)
{
	std::shared_ptr<tcp::socket> skt(new tcp::socket(_io_service_pool.get_io_service()));
	tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), port);
	boost::system::error_code error;

	//skt->connect(endpoint, error);
	skt->async_connect(endpoint, boost::bind(&NetService::OnConnect, this, skt, func, _1));

	//if (error)
	//{
	//	return false;
	//};

	//ConnectPtr conn(new NetConnect(skt, boost::bind(&NetService::GetNetPack, this, _1)));

	//conn->RegistOnDisConnect(boost::bind(&NetService::OnDisConnect, this, _1));
	//conn->ReadHead();

	//func(conn);

	return true;
}

//-------------------------------------------------------------------------------------------
void NetService::OnConnect(std::shared_ptr<boost::asio::ip::tcp::socket> psocket, 
	boost::function<void(ConnectPtr&)> func, boost::system::error_code ec)
{
	if (ec)
	{
		return;
	};

	ConnectPtr conn(new NetConnect(psocket, boost::bind(&NetService::GetNetPack, this, _1)));

	conn->RegistOnDisConnect(boost::bind(&NetService::OnDisConnect, this, _1));
	conn->ReadHead();

	func(conn);
}

//------------------------------------------------------------------------------------------ -
//void NetService::DisConnect(std::string addr)
//{
//	auto it = _connectMap.find(addr);
//	if (it != _connectMap.end())
//	{
//		std::set<NetObserver*>::iterator setit = _NetObserverSet.begin();
//		for (; setit != _NetObserverSet.end(); setit++)
//		{
//			(*setit)->OnDisConnect(addr);
//		}
//		_connectMap.erase(it);
//	}
//}

//-------------------------------------------------------------------------------------------
void NetService::SendPack(int messegeid, const ::google::protobuf::Message& mess, std::string addr, int playerid)
{
	//auto it = _connectMap.find(addr);
	//if (it != _connectMap.end())
	//{
	//	boost::shared_ptr<NetConnect> pConnect = it->second;
	//	pConnect->SendBuffer(messegeid, mess, mess.ByteSize(), playerid);
	//}
}

