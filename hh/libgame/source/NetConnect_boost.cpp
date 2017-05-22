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

		//֪ͨNetServer�����ѶϿ�
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
		///����յ���ȷ�İ�ͷ���ͼ���������
		if(m_recvBuff->VerifyHeader())
		{
			//�������建��
			m_recvBuff->InitDataBuff();

			///���̶�����
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
	{//�յ�������Э������Ѱ�����NetServer���ȴ�updateͳһ����
		NetService::GetSingleton().ReceivePack(m_recvBuff);

		if (BaseApp::GetSingletonPtr() != NULL )
		{
			LogService::GetSingleton().LogMessage("recv message, id=%d,role=%d,addr=%s", m_recvBuff->GetMessageID(), m_recvBuff->GetRoleID(),m_Address.c_str());
			Helper::dbgTrace("%s roleid = %d recv message,messageid=%d,len=%d,ip=%s\r\n", BaseApp::GetSingleton().GetAppName().c_str(), m_recvBuff->GetRoleID(), m_recvBuff->GetMessageID(), m_recvBuff->GetBuffSize(), m_Address.c_str());
		}

		//��ȡ��һ��Э���
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

	//��סѹ�뵽���Ͷ��е���ȥ
	boost::mutex::scoped_lock oLock(m_SendMutex);
	m_SendPackCollect.push_back(pPack);
	oLock.unlock();

	doSend();

	return true;
}


//-----------------------------------------------------------------------
void NetConnect::doSend()
{
	//ȡ��һ����׼������
	boost::mutex::scoped_lock oLock(m_SendMutex);
	if(m_SendPackCollect.empty())
	{
		m_bSending = false;
		return;
	}

	if(m_bSending)	//����Ƿ��ڷ���
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


	//��pPack���ͳ�ȥ
	boost::asio::async_write(m_Socket,boost::asio::buffer(m_sendPack->GetBuff(),m_sendPack->GetBuffSize()),boost::bind(&NetConnect::handWrite, shared_from_this(),boost::asio::placeholders::error));

}


//-----------------------------------------------------------------------
void NetConnect::handWrite(const boost::system::error_code& e)
{
	if(!e)
	{
		boost::mutex::scoped_lock oLock(m_SendMutex);

		m_bSending = false;		//����Ϊ��һ���ѷ������

		oLock.unlock();

		doSend();
	}
}

#endif
