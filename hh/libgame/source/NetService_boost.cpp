#include "pch.h"
#include "helper.h"
#include "NetService.h"
#include "BaseApp.h"
#ifdef _BOOST_TEST_
template<> NetService* Singleton<NetService>::s_pInstance=NULL;

//----------------------------------------------------------------
NetService::NetService(int32 iothreadnum)
: m_IOPool(iothreadnum), m_pAcceptor(NULL)
{
	m_bAccept = true;
}


//----------------------------------------------------------------
NetService::~NetService(void)
{
	m_bAccept = false;

	SafeDelete(m_pAcceptor);

	Sleep(200);

	m_IOPool.stop();

	m_ConnectMap.clear();	//�ͷ����е�����
}



//----------------------------------------------------------------
bool NetService::Start(int32 nPort)
{
	m_IOPool.run();

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string("0.0.0.0"),nPort);
	//boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), nPort);
	//������һ��io_service��Ϊ�����˿�
	m_pAcceptor = new boost::asio::ip::tcp::acceptor(m_IOPool.get_io_service(), ep);
	//m_pAcceptor->open(ep.protocol());
	m_pAcceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	//m_pAcceptor->bind(ep);
	//m_pAcceptor->listen();

	//��������
	accept();

	return true;
}



//----------------------------------------------------------------
void NetService::accept()
{
	NetConnectPtr  pConnect(new NetConnect(m_IOPool.get_io_service()));

	m_pAcceptor->async_accept(pConnect->getSocket(),
		boost::bind(&NetService::handleConnect,this,pConnect,boost::asio::placeholders::error)
	);

	return ;
}


//----------------------------------------------------------------
void NetService::handleConnect(NetConnectPtr pConnect,const boost::system::error_code& e)
{
	if(!e)
	{
		//����IPPort
		pConnect->SetAddress();

		//���뵽������������
		boost::mutex::scoped_lock oLock(m_ConnectMutex);

		m_ConnectMap.insert(std::make_pair(pConnect->GetAddress(),pConnect));
		m_NewConnectCollect.push_back(pConnect->GetAddress());			//���������Ӷ��У��ȴ��㲥

		oLock.unlock();

		pConnect->Read();
	}
	else
	{
		pConnect->close();
	}

	//�����ȴ�����
	if(m_bAccept)
		accept();
}


//----------------------------------------------------------------
void NetService::handleConnect2(NetConnectPtr pConnect, const boost::system::error_code& e)
{
	if (!e)
	{
		//����IPPort
		pConnect->SetAddress();

		//���뵽������������
		boost::mutex::scoped_lock oLock(m_ConnectMutex);

		m_ConnectMap.insert(std::make_pair(pConnect->GetAddress(), pConnect));
		m_NewConnectCollect.push_back(pConnect->GetAddress());			//���������Ӷ��У��ȴ��㲥

		oLock.unlock();

		pConnect->Read();
	}
	else
	{
		pConnect->close();
	}
}


//----------------------------------------------------------------
void NetService::RegisterObserver(NetObserver* pObserver)
{
	CHECKERRORANDRETURN(pObserver!=NULL);

	m_NetObservers.insert(pObserver);
}


//----------------------------------------------------------------
void NetService::UnregisterObserver(NetObserver* pObserver)
{
	CHECKERRORANDRETURN(pObserver!=NULL);

	NetObserverCollect::iterator it = m_NetObservers.find(pObserver);
	if(it!=m_NetObservers.end())
	{
		m_NetObservers.erase(it);
	}
}


//----------------------------------------------------------------
void NetService::RemoveObserver(NetObserver* pObserver)
{
	CHECKERRORANDRETURN(pObserver!=NULL);

	NetObserverCollect::iterator it = m_NetObservers.find(pObserver);
	if(it!= m_NetObservers.end())
	{
		m_NetObservers.erase(it);
	}
}


//----------------------------------------------------------------
void NetService::Update()
{
	/******������ķַ��������ӵĹ㲥���Ͽ����ӵĹ㲥*****/

	boost::mutex::scoped_lock oLock(m_PackMutex);
	NetPackCollect tmpPackCollect = m_PackCollect;
	m_PackCollect.clear();		//��������ʱ�����У������
	oLock.unlock();

	NetPackCollect::iterator itPack = tmpPackCollect.begin();
	NetPackCollect::iterator itPackEnd = tmpPackCollect.end();
	for(; itPack!=itPackEnd; itPack++)
	{
		NetPackPtr& pPack = *itPack;
		FireMessage(pPack->GetMessageID(),pPack);
	}
	tmpPackCollect.clear();			//�ͷŰ�����


	//��������������
	boost::mutex::scoped_lock netLock(m_ConnectMutex);
	NetRecordCollect tmpNew = m_NewConnectCollect;
	m_NewConnectCollect.clear();
	netLock.unlock();

	if(tmpNew.size()!=0)
	{
		NetRecordCollect::iterator newit = tmpNew.begin();
		NetRecordCollect::iterator newitEnd = tmpNew.end();
		for(; newit!=newitEnd; newit++)
		{
			std::for_each(m_NetObservers.begin(),m_NetObservers.end(),boost::bind(&NetObserver::onConnect,_1,*newit));
		}
	}


	//�������жϿ�����
	netLock.lock();
	NetRecordCollect tmpClose = m_CloseConnectCollect;
	m_CloseConnectCollect.clear();
	netLock.unlock();

	if(tmpClose.size()!=0)
	{
		NetRecordCollect::iterator closeit = tmpClose.begin();
		NetRecordCollect::iterator closeitEnd = tmpClose.end();
		for(; closeit!=closeitEnd; closeit++)
		{
			NetConnectMap::iterator it = m_ConnectMap.find(*closeit);
			if (it != m_ConnectMap.end())
			{
				m_ConnectMap.erase(it);
			}

			std::for_each(m_NetObservers.begin(),m_NetObservers.end(),boost::bind(&NetObserver::onDisconnect,_1,*closeit));
		}
	}

}


//----------------------------------------------------------------
void NetService::ReceivePack(NetPackPtr pPack)
{
	//��ӡ��־����
	xstring appName = "UnKnow";
	BaseApp* pApp = BaseApp::GetSingletonPtr();
	if (pApp != NULL)
	{
		appName = pApp->GetAppName();
	}

	Helper::dbgTrace("%s roleid = %d receive message,messageid=%d,len=%d,ip=%s\r\n", appName.c_str(), pPack->GetRoleID(), pPack->GetMessageID(), pPack->GetBuffSize(), pPack->GetAddress().c_str());


	boost::mutex::scoped_lock oLock(m_PackMutex);
	m_PackCollect.push_back(pPack);
}


//----------------------------------------------------------------
bool NetService::Connect(const xstring& ip,int32 port)
{
	NetConnectPtr  pConnect(new NetConnect(m_IOPool.get_io_service()));

	//�趨IP��ַ
	pConnect->SetAddress("_" + ip+ ":" + Helper::Int32ToString(port));

	boost::asio::ip::tcp::resolver resolver(pConnect->getSocket().get_io_service());
	boost::asio::ip::tcp::resolver::query query(ip.c_str(), Helper::Int32ToString(port));
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	boost::system::error_code error = boost::asio::error::host_not_found;

	boost::asio::async_connect(pConnect->getSocket(), endpoint_iterator, boost::bind(&NetService::handleConnect2, this, pConnect, boost::asio::placeholders::error));

	return true;
}


//-------------------------------------------------------------------------------------------
bool NetService::Connect2(const xstring& ip, int32 port)
{
	NetConnectPtr  pConnect(new NetConnect(m_IOPool.get_io_service(), true));

	boost::asio::ip::tcp::resolver resolver(pConnect->getSocket().get_io_service());
	boost::asio::ip::tcp::resolver::query query(ip.c_str(), Helper::Int32ToString(port));
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	boost::system::error_code error = boost::asio::error::host_not_found;

	boost::asio::async_connect(pConnect->getSocket(), endpoint_iterator, boost::bind(&NetService::handleConnect2, this, pConnect, boost::asio::placeholders::error));

	return true;
}

//----------------------------------------------------------------
bool NetService::Disconnect(const xstring& addr)
{
	//�ҵ����ӣ�������close
	boost::mutex::scoped_lock oLock(m_ConnectMutex);
	NetConnectMap::iterator it = m_ConnectMap.find(addr);
	if(it != m_ConnectMap.end())
	{
		NetConnectPtr pConnect = it->second;
		oLock.unlock();

		pConnect->close();
	}

	return true;
}


//----------------------------------------------------------------
void NetService::OnDisconnect(const xstring& addr)
{
	//�������ƶ����Ͽ������б���
	boost::mutex::scoped_lock oLock(m_ConnectMutex);
	m_CloseConnectCollect.push_back(addr);
}


//----------------------------------------------------------------
bool NetService::Send(int32 messageid,const char* pdata,int32 length,const xstring& addr,int32 roleid)
{
	if(pdata==NULL||length==NULL)
	{
		return false;
	}

	boost::mutex::scoped_lock lock(m_ConnectMutex);
	NetConnectMap::iterator it=m_ConnectMap.find(addr);
	if(it==m_ConnectMap.end())
	{
		return false;
	}

	it->second->Send(messageid,pdata,length,roleid);

	return true;
}


//----------------------------------------------------------------
bool NetService::SendProtoBuf(int32 messageid ,const  ::google::protobuf::Message & proto,const xstring& addr,int32 roleid)
{
	std::string buf;
	proto.SerializePartialToString(&buf);
	return Send(messageid,buf.c_str(),(int32)buf.length(),addr,roleid);
}
#endif

