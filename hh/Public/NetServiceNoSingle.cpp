#include "stdafx.h"
#include "NetServiceNoSingle.h"

int NetServiceNo::port = 10086;
//-------------------------------------------------------------------------------------------
NetServiceNo::NetServiceNo(int port, int ionum) : _io_service_pool(ionum)
{
	_acceptor = new boost::asio::ip::tcp::acceptor(_io_service_pool.get_io_service(), tcp::endpoint(tcp::v4(), port));
	accept();
}

//-------------------------------------------------------------------------------------------
NetServiceNo::~NetServiceNo()
{
	_io_service_pool.stop();

	if (_acceptor!=NULL)
		delete _acceptor;

//	std::hash_map<std::string, NetConnectNo*>::iterator it = _connectMap.begin();
//	for (; it != _connectMap.end(); it++)
//	{
////		it->second->CloseSocket();
//		delete it->second;
//	}
//	_connectMap.clear();
//
//	it = _connectToMap.begin();
//	for (; it != _connectToMap.end(); it++)
//	{
//		//		it->second->CloseSocket();
//		delete it->second;
//	}
//	_connectToMap.clear();


}

//-------------------------------------------------------------------------------------------
void NetServiceNo::accept()
{
	boost::shared_ptr<tcp::socket> skt(new tcp::socket(_io_service_pool.get_io_service()));

	_acceptor->async_accept(*skt, boost::bind(&NetServiceNo::OnConnect, this, skt, _1));
}

//-------------------------------------------------------------------------------------------
void NetServiceNo::OnConnect(boost::shared_ptr<tcp::socket> psocket, boost::system::error_code ec)
{
	if (ec)
		return;
	boost::shared_ptr<NetConnectNo> conn = boost::shared_ptr<NetConnectNo>(new NetConnectNo(psocket, boost::bind(&NetServiceNo::GetNetPack, this, _1)));
	conn->RegistOnDisConnect(boost::bind(&NetServiceNo::OnDisConnect, this, _1));

	std::string address;
	address = psocket->remote_endpoint().address().to_string();
	address += ":";
	char pot[10] = { 0 };
	sprintf_s<10>(pot, "%d", psocket->remote_endpoint().port());
	address += pot;

	_connectMap.insert(std::make_pair(address, conn));
	conn->ReadNetBuffer();

	std::set<NetObserver*>::iterator setit = _NetObserverSet.begin();
	for (; setit != _NetObserverSet.end(); setit++)
	{
		(*setit)->onConnect(address);
	}
	accept();
}

//-------------------------------------------------------------------------------------------
void NetServiceNo::OnDisConnect(std::string addr)
{
	boost::mutex::scoped_lock lock(_mutex);
	std::hash_map<std::string, boost::shared_ptr<NetConnectNo>>::iterator it = _connectMap.find(addr);
	if (it != _connectMap.end())
	{
		std::set<NetObserver*>::iterator setit = _NetObserverSet.begin();
		for (; setit != _NetObserverSet.end(); setit++)
		{
			(*setit)->onDisconnect(addr);
		}
//		it->second->CloseSocket();
//		delete it->second;
		_connectMap.erase(it);
	}
}

//-------------------------------------------------------------------------------------------
void NetServiceNo::update()
{
	boost::shared_ptr<NetPackNo> pPack;
	while(PutGetNetPack(false, pPack))
		FireMessage(pPack->getMessageId(), pPack);
}

//-------------------------------------------------------------------------------------------
void NetServiceNo::run()
{
//	boost::shared_ptr<boost::thread> ppthread(new boost::thread(boost::bind(&io_service_pool::run, &_io_service_pool)));
//	ppthread->join();
	_io_service_pool.run();
}

//-------------------------------------------------------------------------------------------
void NetServiceNo::GetNetPack(boost::shared_ptr<NetPackNo>& pPack)
{
	PutGetNetPack(true, pPack);
}

//-------------------------------------------------------------------------------------------
bool NetServiceNo::PutGetNetPack(bool bPut, boost::shared_ptr<NetPackNo>& pPack)
{
	boost::mutex::scoped_lock lock(_mutex);
	if (bPut == true)
	{
		NetPackQueue.push(pPack);
		return true;
	}
	else
	{
		if (NetPackQueue.size() == 0)
			return false;
		else
		{
			pPack = NetPackQueue.front();
			NetPackQueue.pop();
			return true;
		}
	}
}

int NetServiceNo::GetPort()
{
	return port++;
}

void NetServiceNo::RegistObserver(NetObserver* observer)
{
	_NetObserverSet.insert(observer);
}

void NetServiceNo::UnRegistObserver(NetObserver* observer)
{
	_NetObserverSet.erase(observer);
}
//-------------------------------------------------------------------------------------------
bool NetServiceNo::Connect(const std::string& ip, int port)
{
	boost::shared_ptr<tcp::socket> skt(new tcp::socket(_io_service_pool.get_io_service()));
	tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), port);
	boost::system::error_code error;

	skt->connect(endpoint, error);

	if (error)
		return false;

	boost::shared_ptr<NetConnectNo> conn = boost::shared_ptr<NetConnectNo>(new NetConnectNo(skt, boost::bind(&NetServiceNo::GetNetPack, this, _1)));

	conn->ReadNetBuffer();
	_connectToMap.insert(std::make_pair(conn->GetAddress(), conn));

	std::set<NetObserver*>::iterator setit = _NetObserverSet.begin();
	for (; setit != _NetObserverSet.end(); setit++)
	{
		(*setit)->onConnect(conn->GetAddress());
	}
	return true;
}

//-------------------------------------------------------------------------------------------
void NetServiceNo::DisConnect(std::string addr)
{
	boost::mutex::scoped_lock lock(_mutex);
	std::hash_map<std::string, boost::shared_ptr<NetConnectNo>>::iterator it = _connectToMap.find(addr);
	if (it != _connectToMap.end())
	{
		_connectToMap.erase(it);
	}
}

//-------------------------------------------------------------------------------------------
void NetServiceNo::SendPack(int messegeid,const ::google::protobuf::Message& mess, std::string addr, int playerid = 0)
{
	auto it = _connectMap.find(addr);
	if (it != _connectMap.end())
	{
		boost::shared_ptr<NetConnectNo> pConnect = it->second;
		pConnect->SendBuffer(messegeid, mess, mess.ByteSize(), playerid);
	}

	it = _connectToMap.find(addr);
	if (it != _connectToMap.end())
	{
		boost::shared_ptr<NetConnectNo> pConnect = it->second;
		pConnect->SendBuffer(messegeid, mess, mess.ByteSize(), playerid);
	}
}