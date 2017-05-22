#include "pch.h"
#include "helper.h"
#include "NetConnect.h"
#include "NetService.h"
#include "BaseApp.h"
#ifdef _BOOST_TEST_
//-----------------------------------------------------------------------
NetConnect::NetConnect(boost::asio::io_service& ioservice,bool client)
: m_Socket(ioservice), m_bDisconnect(false), m_bSending(false), _bClient(client)
{
}


//-----------------------------------------------------------------------
NetConnect::~NetConnect(void)
{
	if(m_Socket.is_open())
	{
		//m_Socket.close();

		//boost::system::error_code ignored_ec;
		//m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	}
}


//-----------------------------------------------------------------------
void NetConnect::SetAddress(xstring addr)
{
	m_Address = addr;
}


//-----------------------------------------------------------------------
void NetConnect::SetAddress()
{
	if(m_Address.empty())
	{
		if (_bClient)
		{
			xstring ipaddr = m_Socket.local_endpoint().address().to_string();
			uint16 port = m_Socket.local_endpoint().port();

			m_Address = ipaddr + ":" + Helper::Int32ToString(port);
		}
		else
		{
			xstring ipaddr = m_Socket.remote_endpoint().address().to_string();
			uint16 port = m_Socket.remote_endpoint().port();

			m_Address = ipaddr + ":" + Helper::Int32ToString(port);
		}
	}
}


//-----------------------------------------------------------------------
void NetConnect::close()
{
	if(m_Socket.is_open())
	{
		try{
			boost::system::error_code ignored_ec;
			m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

			m_Socket.close();
		}
		catch (...)
		{
		}
	}

	if(!m_bDisconnect)
	{
		m_bDisconnect = true;

		//通知NetServer连接已断开
		NetService::GetSingleton().OnDisconnect(m_Address);

	}
}


//-----------------------------------------------------------------------
bool NetConnect::Read()
{
	if(m_Socket.is_open()==false)
	{
		return  false;
	}

	m_recvBuff = NetPackPtr(new NetPack);
	m_recvBuff->SetAddress(m_Address);

	boost::asio::async_read(
		m_Socket,
		boost::asio::buffer(m_recvBuff->GetHeadBuff(),m_recvBuff->GetHeadSize()),
		boost::bind(&NetConnect::handleHead,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
	);

	return true;
}


//-----------------------------------------------------------------------
void NetConnect::handleHead(const boost::system::error_code& error, size_t len)
{
	if(!error)
	{
		///如果收到正确的包头。就继续读包身
		if(m_recvBuff->VerifyHeader())
		{
			//创建包体缓存
			m_recvBuff->InitDataBuff();

			///续继读包体
			boost::asio::async_read(
				m_Socket,
				boost::asio::buffer( m_recvBuff->GetData(),m_recvBuff->GetDataSize()),
				bind(&NetConnect::handleBody,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
			);

			return;
		}
	}

	close();
}


//-----------------------------------------------------------------------
void NetConnect::handleBody(const boost::system::error_code& error, size_t len)
{
	if(!error)
	{//收到完整的协议包，把包打入NetServer，等待update统一处理
		NetService::GetSingleton().ReceivePack(m_recvBuff);

		if (BaseApp::GetSingletonPtr() != NULL )
		{
			LogService::GetSingleton().LogMessage("recv message, id=%d,role=%d,addr=%s", m_recvBuff->GetMessageID(), m_recvBuff->GetRoleID(),m_Address.c_str());
			Helper::dbgTrace("%s roleid = %d recv message,messageid=%d,len=%d,ip=%s\r\n", BaseApp::GetSingleton().GetAppName().c_str(), m_recvBuff->GetRoleID(), m_recvBuff->GetMessageID(), m_recvBuff->GetBuffSize(), m_Address.c_str());
		}

		//收取下一个协议包
		Read();

		return;
	}

	close();
}


//-----------------------------------------------------------------------
bool NetConnect::Send(int32 messageid,const char* pdata,int32 length,uint32 roleid)
{
	if(pdata==NULL||length==0)
	{
		return false;
	}

	NetPackPtr pPack(new NetPack(messageid,pdata,length,roleid));

	//锁住压入到发送队列当中去
	boost::mutex::scoped_lock oLock(m_SendMutex);
	m_SendPackCollect.push_back(pPack);
	oLock.unlock();

	doSend();

	return true;
}


//-----------------------------------------------------------------------
void NetConnect::doSend()
{
	//取出一个包准备发送
	boost::mutex::scoped_lock oLock(m_SendMutex);
	if(m_SendPackCollect.empty())
	{
		m_bSending = false;
		return;
	}

	if(m_bSending)	//检查是否还在发送
		return;

	m_bSending = true;
	m_sendPack = m_SendPackCollect[0];
	m_SendPackCollect.erase(m_SendPackCollect.begin());

	oLock.unlock();

	if (BaseApp::GetSingletonPtr() != NULL)
	{
		LogService::GetSingleton().LogMessage("send message,id=%d,role=%d,addr=%s", m_sendPack->GetMessageID(), m_sendPack->GetRoleID(),m_Address.c_str());
		Helper::dbgTrace("%s roleid = %d send message,messageid=%d,len=%d,ip=%s\r\n", BaseApp::GetSingleton().GetAppName().c_str(), m_sendPack->GetRoleID(), m_sendPack->GetMessageID(), m_sendPack->GetBuffSize(), m_Address.c_str());
	}


	//将pPack发送出去
	boost::asio::async_write(m_Socket,boost::asio::buffer(m_sendPack->GetBuff(),m_sendPack->GetBuffSize()),boost::bind(&NetConnect::handWrite, shared_from_this(),boost::asio::placeholders::error));

}


//-----------------------------------------------------------------------
void NetConnect::handWrite(const boost::system::error_code& e)
{
	if(!e)
	{
		boost::mutex::scoped_lock oLock(m_SendMutex);

		m_bSending = false;		//设置为上一包已发送完成

		oLock.unlock();

		doSend();
	}
}

#endif
