#include "pch.h"
#include "helper.h"
#include "NetClient.h"

//-------------------------------------------------------------------------
NetClient::NetClient()
	: m_pNetService(NULL),m_bConnect(false)
{
}


//-------------------------------------------------------------------------
NetClient::~NetClient(void)
{
	
}

//-------------------------------------------------------------------------------------------
void NetClient::set_outerip(const xstring& ip)
{
	m_outerip = ip;
}

//-------------------------------------------------------------------------
bool NetClient::ConnectTo(NetService* pNetService,const xstring& ip,int32 port)
{
	m_ip = ip;
	m_port = port;

	m_address = "_" + m_ip + ":" + Helper::Int32ToString(m_port);

	m_pNetService = pNetService;

	CHECKERRORANDRETURNRESULT(pNetService!=NULL);

	m_pNetService->Connect(ip,port);

	return true;
}


//-------------------------------------------------------------------------
void NetClient::onConnect(const xstring& addr)
{
	if(addr == m_address)
	{
		m_bConnect = true;

		xApplication::GetSingleton().PrintMessage(msgInfo, "accept connection,%s", addr.c_str());
	}
}


//-------------------------------------------------------------------------
void NetClient::onDisconnect(const xstring& addr)
{
	if(addr == m_address)
	{
		if (m_bConnect)
		{
			xApplication::GetSingleton().PrintMessage(msgInfo, "lose connection,%s", addr.c_str());
		}
		m_bConnect = false;

		m_pNetService->Connect(m_ip,m_port);
	}
}


//----------------------------------------------------------------
bool NetClient::Send(int32 messageid, const char* pdata, int32 length, int32 roleid)
{
	if(pdata==NULL||length==NULL||!m_bConnect)
	{
		return false;
	}

	return m_pNetService->Send(messageid,pdata,length,m_address,roleid);
}


//----------------------------------------------------------------
bool NetClient::SendProtoBuf(int32 messageid, const ::google::protobuf::Message& proto, int32 roleid)
{
	std::string buf;
	proto.SerializePartialToString(&buf);

	return m_pNetService->SendProtoBuf(messageid,proto,m_address,roleid);
}


//----------------------------------------------------------------
xstring& NetClient::getAddress()
{
	return m_address;
}