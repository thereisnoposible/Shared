#include "pch.h"
#include "NetConnect.h"
#include "NetService.h"
#ifdef _WIN32
//-------------------------------------------------------------------------------------------
NetConnect::NetConnect(SOCKET sock)
: _socket(sock), _channelStatus(recvHead), _needLen(0), _recvLen(0), m_bSending(false)
{

}

//-------------------------------------------------------------------------------------------
NetConnect::~NetConnect()
{

}

//-------------------------------------------------------------------------------------------
void NetConnect::StartRead()
{
	_channelStatus = recvHead;

	_needLen = sizeof(PackHead);

	_readBuff = NetPackPtr(new NetPack());

	PIOContext pContext = NetService::GetSingleton().AllocateContext();
	pContext->IoOperation = IO_READ;
	//投递操作到完成端口,进入协议头的读取
	pContext->wsabuf.buf = _readBuff->GetHeadBuff();
	pContext->wsabuf.len = _readBuff->GetHeadSize();

	DWORD flag = 0;
	WSARecv(_socket, &(pContext->wsabuf), 1, NULL, &flag, &(pContext->ol), NULL);
}


//-------------------------------------------------------------------------------------------
void NetConnect::onRead(char* pBuff, int32 len)
{
	_recvLen += len;
	CHECKERRORANDRETURN(_recvLen <= _needLen);

	char* pRecvBuff = NULL;

	if (_recvLen == _needLen)
	{//收满了头或者协议尾
		if (_channelStatus == recvHead)
		{
			//如果协议头出错，投递关闭命令到完成端口
			_readBuff->VerifyHeader();
			_readBuff->InitDataBuff();

			_needLen = _readBuff->GetDataSize();
			_channelStatus = recvBody;

			pRecvBuff = _readBuff->GetData();
		}
		else
		{
			_readBuff->SetAddress(_addr);

			NetService::GetSingleton().ReceivePack(_readBuff);

			_needLen = sizeof(PackHead);
			//准备一个新的协议包
			_readBuff = NetPackPtr(new NetPack());

			_channelStatus = recvHead;

			pRecvBuff = _readBuff->GetHeadBuff();
		}

		_recvLen = 0;
	}
	else
	{//还要继续接收
		if (_channelStatus == recvHead)
		{
			pRecvBuff = _readBuff->GetHeadBuff();
		}
		else
		{
			pRecvBuff = _readBuff->GetData();
		}

		pRecvBuff += _recvLen;
	}


	//处理完成，等待后续数据
	PIOContext pContext = NetService::GetSingleton().AllocateContext();
	pContext->IoOperation = IO_READ;
	pContext->wsabuf.buf = pRecvBuff;
	pContext->wsabuf.len = _needLen-_recvLen;
	DWORD flag = 0;

	WSARecv(_socket, &(pContext->wsabuf), 1, NULL, &flag, &(pContext->ol), NULL);
}

//-------------------------------------------------------------------------------------------
void NetConnect::onWrite()
{
	boost::mutex::scoped_lock oLock(m_SendMutex);
	m_bSending = false;
	oLock.unlock();

	doSend();
}

//-------------------------------------------------------------------------------------------
void NetConnect::SetAddress(const xstring& addr)
{
	_addr = addr;
}

//-------------------------------------------------------------------------------------------
xstring& NetConnect::GetAddress()
{
	return _addr;
}

//-------------------------------------------------------------------------------------------
SOCKET NetConnect::GetSocket()
{
	return _socket;
}


//-----------------------------------------------------------------------
void NetConnect::doSend()
{
	//取出一个包准备发送
	boost::mutex::scoped_lock oLock(m_SendMutex);
	if (m_SendPackCollect.empty())
	{
		m_bSending = false;
		return;
	}

	if (m_bSending)	//检查是否还在发送
		return;

	m_bSending = true;

	NetPackPtr m_sendPack;			/** 发送缓存 */

	m_sendPack = m_SendPackCollect[0];
	m_SendPackCollect.erase(m_SendPackCollect.begin());

	oLock.unlock();


	//将pPack发送出去
	PIOContext pContext = NetService::GetSingleton().AllocateContext();
	pContext->IoOperation = IO_WRITE;

	pContext->wsabuf.buf = m_sendPack->GetBuff();
	pContext->wsabuf.len = m_sendPack->GetBuffSize();

	DWORD flag = 0;
	WSASend(_socket, &(pContext->wsabuf), 1, NULL, flag, &(pContext->ol), NULL);
}

//-----------------------------------------------------------------------
bool NetConnect::Send(int32 messageid, const char* pdata, int32 length, uint32 roleid)
{
	if (pdata == NULL || length == 0)
	{
		return false;
	}

	NetPackPtr pPack(new NetPack(messageid, pdata, length, roleid));

	//锁住压入到发送队列当中去
	boost::mutex::scoped_lock oLock(m_SendMutex);
	m_SendPackCollect.push_back(pPack);
	oLock.unlock();

	doSend();

	return true;
}

void NetConnect::close()
{
	closesocket(_socket);
	_socket = INVALID_SOCKET;
}

#endif
