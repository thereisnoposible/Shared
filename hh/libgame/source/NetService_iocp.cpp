#include "pch.h"
#include "NetService.h"

#ifdef _WIN32

NetService* Singleton<NetService>::s_pInstance = NULL;

//-------------------------------------------------------------------------------------------
NetService::NetService(int32 iothreadnum)
: _io_thread_num(iothreadnum), _hIOCompletionPort(INVALID_HANDLE_VALUE), _bShutDown(false)
{
	
}

//-------------------------------------------------------------------------------------------
NetService::~NetService()
{
	//消除内存泄露
	IOContextCollect::iterator it = _IOContextCollect.begin();
	for (; it != _IOContextCollect.end(); it++)
	{
		delete *it;
	}

	_IOContextCollect.clear();
}

//-------------------------------------------------------------------------------------------
bool NetService::Start(int32 nPort)
{
	//创建IO线程
	CHECKERRORANDRETURNRESULT(SetupIOWorkers());

	//创建监听工作线程
	setupListenWorker(nPort);

	return true;
}

//-------------------------------------------------------------------------------------------
void NetService::setupListenWorker(int32 port)
{
	//建立监听连接
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	_sockListen = socket(AF_INET, SOCK_STREAM, 0);

	_hEvent = WSACreateEvent();

	WSAEventSelect(_sockListen, _hEvent, FD_ACCEPT);

	SOCKADDR_IN		saddr;
	saddr.sin_port = htons(port);
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(_sockListen, (LPSOCKADDR)&saddr, sizeof(saddr));

	listen(_sockListen, 20);

	boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&NetService::ListenThreadProc, this)));
	_threads.push_back(thread);
}

//-------------------------------------------------------------------------------------------
bool NetService::SetupIOWorkers()
{
	//创建完成端口
	_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CHECKERRORANDRETURNRESULT(_hIOCompletionPort != 0);

	for (int32 i = 0; i < _io_thread_num; i++)
	{
		boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&NetService::IOThreadProc, this)));
		_threads.push_back(thread);
	}
	
	return true;
}

//-------------------------------------------------------------------------------------------
void NetService::ListenThreadProc()
{
	//等待接收网路连接
	while (!_bShutDown)
	{
		DWORD dwRet;
		dwRet = WSAWaitForMultipleEvents(1,
			&_hEvent,
			FALSE,
			1000,
			FALSE);

		if (_bShutDown)
			break;
		if (dwRet == WSA_WAIT_TIMEOUT)
			continue;

		WSANETWORKEVENTS events;
		int nRet = WSAEnumNetworkEvents(_sockListen,
			_hEvent,
			&events);

		if (nRet == SOCKET_ERROR)
		{
			break;
		}

		if (events.lNetworkEvents & FD_ACCEPT)
		{//收到连接
			if (events.iErrorCode[FD_ACCEPT_BIT] == 0)
			{
				SOCKET		clientSocket = INVALID_SOCKET;
				int			nRet = -1;
				int			nLen = -1;
				nLen = sizeof(SOCKADDR_IN);

				clientSocket = WSAAccept(_sockListen,
					NULL,
					&nLen,
					0,
					0);

				if (clientSocket != SOCKET_ERROR)
				{
					//成功收到一个连接,创建出一个connect对象
					CHECKERRORANDCONTINUE(CreateIoCompletionPort((HANDLE)clientSocket, _hIOCompletionPort, clientSocket, 0) != 0);

					struct sockaddr_in peeraddr;
					int addr_len = sizeof(peeraddr);

					ZeroMemory(&peeraddr, sizeof(peeraddr));
					getpeername(clientSocket, (struct sockaddr *)(&peeraddr), &addr_len);

					xstring tmp = inet_ntoa(peeraddr.sin_addr);
					int32 nport = ntohs(peeraddr.sin_port);
					
					std::stringstream ss;
					ss << tmp << ":" <<nport;
					NetConnectPtr pNetConnect(new NetConnect(clientSocket));
					pNetConnect->SetAddress(ss.str());

					onNewSocket(pNetConnect);
				}
			}
		}
	}

	//退出监听线程
}

//-------------------------------------------------------------------------------------------
void NetService::IOThreadProc()
{
	while (true)
	{
		DWORD dwByteTrans;
		SOCKET s;

		PIOContext pContext = NULL;
#ifdef _WIN64
		BOOL bRet = GetQueuedCompletionStatus(_hIOCompletionPort, &dwByteTrans,
			&s, (LPOVERLAPPED*)&pContext, INFINITE);
#else
		BOOL bRet = GetQueuedCompletionStatus(_hIOCompletionPort, &dwByteTrans,
			(PULONG_PTR)&s, (LPOVERLAPPED*)&pContext, INFINITE);
#endif // DEBUG

		

		if (!bRet)
		{
			if (pContext != NULL)
			{
				ReleaseContext(pContext);
			}
			continue;
		}
		else
		{
			if (pContext == NULL)
			{//断开连接
				return;
			}

			NetConnectPtr pConnect = getConnectBy(s);
			if (pConnect.get() == NULL)
			{
				ReleaseContext(pContext);
				continue;
			}

			int8 opType = pContext->IoOperation;
			if (opType == IO_WRITE)
			{
				ReleaseContext(pContext);
				pConnect->onWrite();
				
				continue;
			}

			if (opType == IO_READ)
			{
				if (dwByteTrans != 0)
				{
					ReleaseContext(pContext);
					pConnect->onRead(pContext->wsabuf.buf, dwByteTrans);
					
					continue;
				}
				else
				{
					//连接断开
					onDisconnectSocket(pConnect);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------
bool NetService::Connect(const xstring& ip, int32 port)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN	SockAddr;
	memset(&SockAddr, 0, sizeof(SockAddr));

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	SockAddr.sin_port = htons(port);

	int32 nRet = connect(sock, (sockaddr*)&SockAddr, sizeof(SockAddr));
	if (nRet != SOCKET_ERROR)
	{
		if (!CreateIoCompletionPort((HANDLE)sock, _hIOCompletionPort, sock, 0))
		{
			return false;
		}

		xstringstream ss;
		ss <<"_" << ip << ":" << port;

		NetConnectPtr pConnect(new NetConnect(sock));

		pConnect->SetAddress(ss.str());

		onNewSocket(pConnect);
	}

	return true;
}

//-------------------------------------------------------------------------------------------
bool NetService::Connect2(const xstring& ip, int32 port)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN	SockAddr;
	memset(&SockAddr, 0, sizeof(SockAddr));

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	SockAddr.sin_port = htons(port);

	int32 nRet = connect(sock, (sockaddr*)&SockAddr, sizeof(SockAddr));
	if (nRet != SOCKET_ERROR)
	{
		if (!CreateIoCompletionPort((HANDLE)sock, _hIOCompletionPort, sock, 0))
		{
			return false;
		}

		//获取本地ip地址和端口号
		struct sockaddr_in peeraddr;
		int addr_len = sizeof(peeraddr);

		ZeroMemory(&peeraddr, sizeof(peeraddr));
		getsockname(sock, (struct sockaddr *)(&peeraddr), &addr_len);

		xstring tmp = inet_ntoa(peeraddr.sin_addr);
		int32 nport = ntohs(peeraddr.sin_port);

		xstringstream ss;
		ss << tmp << ":" << nport;

		NetConnectPtr pConnect(new NetConnect(sock));

		pConnect->SetAddress(ss.str());

		onNewSocket(pConnect);
	}

	return true;
}

//-------------------------------------------------------------------------------------------
void NetService::recordClientConnect(NetConnectPtr& pConnect)
{
	SOCKET sock = pConnect->GetSocket();

	_allConnectIDMap.insert(std::make_pair(sock, pConnect));
}


//-------------------------------------------------------------------------------------------
void NetService::onNewSocket(NetConnectPtr& pConnect)
{
	boost::mutex::scoped_lock oLock(_connectMutex);

	//函数调用前，已绑定完成端口，管理起来，并调用start
	SOCKET sock = pConnect->GetSocket();
	xstring addr = pConnect->GetAddress();

	if (_allConnectIDMap.find(sock) == _allConnectIDMap.end())
	{//有可能通过recordClientConnect已经记录了
		_allConnectIDMap.insert(std::make_pair(sock, pConnect));
	}
	
	_allConnectAddrMap.insert(std::make_pair(pConnect->GetAddress(), pConnect));

	m_NewConnectCollect.push_back(pConnect->GetAddress());			//加入新连接队列，等待广播

	pConnect->StartRead();
}

//-------------------------------------------------------------------------------------------
void NetService::onDisconnectSocket(NetConnectPtr& pConnect)
{
	boost::mutex::scoped_lock oLock(_connectMutex);
	SOCKET sock = pConnect->GetSocket();
	xstring addr = pConnect->GetAddress();

	ConnectIDMap::iterator itSock = _allConnectIDMap.find(sock);
	if (itSock != _allConnectIDMap.end())
	{
		_allConnectIDMap.erase(itSock);
	}

	ConnectAddrMap::iterator itAddr = _allConnectAddrMap.find(addr);
	if (itAddr != _allConnectAddrMap.end())
	{
		_allConnectAddrMap.erase(itAddr);
	}
	oLock.unlock();

	OnDisconnect(addr);
}

//-------------------------------------------------------------------------------------------
PIOContext NetService::AllocateContext()
{
	PIOContext pContext = NULL;

	boost::mutex::scoped_lock oLock(_contextMutex);
	IOContextCollect::iterator it = _IOContextCollect.begin();
	if (it != _IOContextCollect.end())
	{
		pContext = *it;

		_IOContextCollect.erase(it);
	}
	else
	{
		pContext = new IOContext();
	}

	pContext->wsabuf.buf = NULL;
	pContext->wsabuf.len = 0;

	ZeroMemory(&pContext->ol, sizeof(OVERLAPPED));

	return pContext;
}

//-------------------------------------------------------------------------------------------
void NetService::ReleaseContext(PIOContext pContext)
{
	boost::mutex::scoped_lock oLock(_contextMutex);

	_IOContextCollect.push_back(pContext);
}

//-------------------------------------------------------------------------------------------
void NetService::ReceivePack(NetPackPtr& pPack)
{
	boost::mutex::scoped_lock oLock(m_PackMutex);
	m_PackCollect.push_back(pPack);
}

//-------------------------------------------------------------------------------------------
NetConnectPtr NetService::getConnectBy(SOCKET sock)
{
	NetConnectPtr pConnect(NULL);
	boost::mutex::scoped_lock oLock(_connectMutex);
	ConnectIDMap::iterator it = _allConnectIDMap.find(sock);
	if (it != _allConnectIDMap.end())
	{
		pConnect = it->second;
	}
	oLock.unlock();

	return pConnect;
}

//----------------------------------------------------------------
void NetService::Update()
{
	/******网络包的分发，新连接的广播，断开连接的广播*****/

	boost::mutex::scoped_lock oLock(m_PackMutex);
	NetPackCollect tmpPackCollect = m_PackCollect;
	m_PackCollect.clear();		//拷贝到临时队列中，并清空
	oLock.unlock();

	NetPackCollect::iterator itPack = tmpPackCollect.begin();
	NetPackCollect::iterator itPackEnd = tmpPackCollect.end();
	for (; itPack != itPackEnd; itPack++)
	{
		NetPackPtr& pPack = *itPack;
		FireMessage(pPack->GetMessageID(), pPack);
	}
	tmpPackCollect.clear();			//释放包对象


	//处理所有新连接
	boost::mutex::scoped_lock netLock(_connectMutex);
	NetRecordCollect tmpNew = m_NewConnectCollect;
	m_NewConnectCollect.clear();
	netLock.unlock();

	if (tmpNew.size() != 0)
	{
		NetRecordCollect::iterator newit = tmpNew.begin();
		NetRecordCollect::iterator newitEnd = tmpNew.end();
		for (; newit != newitEnd; newit++)
		{
			std::for_each(m_NetObservers.begin(), m_NetObservers.end(), boost::bind(&NetObserver::onConnect, _1, *newit));
		}
	}


	//处理所有断开连接
	netLock.lock();
	NetRecordCollect tmpClose = m_CloseConnectCollect;
	m_CloseConnectCollect.clear();
	netLock.unlock();

	if (tmpClose.size() != 0)
	{
		NetRecordCollect::iterator closeit = tmpClose.begin();
		NetRecordCollect::iterator closeitEnd = tmpClose.end();
		for (; closeit != closeitEnd; closeit++)
		{
			ConnectAddrMap::iterator it = _allConnectAddrMap.find(*closeit);
			if (it != _allConnectAddrMap.end())
			{
				_allConnectAddrMap.erase(it);
			}

			std::for_each(m_NetObservers.begin(), m_NetObservers.end(), boost::bind(&NetObserver::onDisconnect, _1, *closeit));
		}
	}

}


//----------------------------------------------------------------
void NetService::RegisterObserver(NetObserver* pObserver)
{
	m_NetObservers.insert(pObserver);
}


//----------------------------------------------------------------
void NetService::UnregisterObserver(NetObserver* pObserver)
{
	NetObserverCollect::iterator it = m_NetObservers.find(pObserver);
	if (it != m_NetObservers.end())
	{
		m_NetObservers.erase(it);
	}
}


//----------------------------------------------------------------
void NetService::RemoveObserver(NetObserver* pObserver)
{
	NetObserverCollect::iterator it = m_NetObservers.find(pObserver);
	if (it != m_NetObservers.end())
	{
		m_NetObservers.erase(it);
	}
}


//----------------------------------------------------------------
void NetService::OnDisconnect(const xstring& addr)
{
	//把连接移动到断开连接列表中
	boost::mutex::scoped_lock oLock(_connectMutex);
	m_CloseConnectCollect.push_back(addr);
}

//-------------------------------------------------------------------------------------------
bool NetService::Disconnect(const xstring& addr)
{
	//找到连接，并调用close
	boost::mutex::scoped_lock oLock(_connectMutex);
	ConnectAddrMap::iterator it = _allConnectAddrMap.find(addr);
	if (it != _allConnectAddrMap.end())
	{
		NetConnectPtr pConnect = it->second;
		oLock.unlock();

		pConnect->close();
	}

	return true;
}

//----------------------------------------------------------------
bool NetService::Send(int32 messageid, const char* pdata, int32 length, const xstring& addr, int32 roleid)
{
	if (pdata == NULL || length == NULL)
	{
		return false;
	}

	boost::mutex::scoped_lock lock(_connectMutex);
	ConnectAddrMap::iterator it = _allConnectAddrMap.find(addr);
	if (it == _allConnectAddrMap.end())
	{
		return false;
	}

	it->second->Send(messageid, pdata, length, roleid);

	return true;
}


bool NetService::SendProtoBuf(int32 messageid, const  ::google::protobuf::Message & proto, const xstring& addr, int32 roleid)
{
	std::string buf;
	proto.SerializePartialToString(&buf);
	return Send(messageid, buf.c_str(), (int32)buf.length(), addr, roleid);
}
#endif

