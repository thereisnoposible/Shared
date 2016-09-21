#include "NetClient.h"

NetClient::NetClient()
{

}

NetClient::~NetClient(void)
{

}


bool NetClient::ConnectTo(NetService* pNetService, const std::string& ip, int port)
{
	m_pNetService = pNetService;
	m_port = port;
	m_ip = ip;
	
	return m_pNetService->Connect(ip, port, boost::bind(&NetClient::OnConnect,this, _1));
}

bool NetClient::Send(int messageid, const char* pdata, int length, int roleid)
{
	return true;
}




bool NetClient::SendProtoBuf(int messageid, ::google::protobuf::Message& pdata, int roleid)
{
	if (m_pConnect != nullptr)
	{
		m_pConnect->SendBuffer(messageid, pdata, roleid);
	}
	return true;
}


std::string& NetClient::getAddress()
{
	m_address = m_ip;
	m_address += ":";
	char pot[10] = { 0 };
	sprintf_s<10>(pot, "%d", m_port);
	m_address += pot;
	return m_address;
}

void NetClient::OnConnect(boost::shared_ptr<NetConnect>& pConnect)
{
	if (pConnect->GetAddress() != getAddress())
		return;

	if (m_pConnect.get() != nullptr)
	{
		m_pConnect->CloseSocket();
	}
	m_pConnect = pConnect;
}

void NetClient::OnDisConnect(boost::shared_ptr<NetConnect>& pConnect)
{
	if (pConnect->GetAddress() != getAddress())
		return;

	if (m_pConnect != nullptr)
	{
		m_pConnect->CloseSocket();
	}

	m_pNetService->Connect(m_ip, m_port, boost::bind(&NetClient::OnConnect, this, _1));
}