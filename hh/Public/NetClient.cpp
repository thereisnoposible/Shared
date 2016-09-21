#include "NetClient.h"

NetClient::NetClient() :tTimer(nullptr)
{

}

NetClient::~NetClient(void)
{
	if (tTimer != nullptr)
		TimerManager::getInstance().RemoveTimer(tTimer);
}


bool NetClient::ConnectTo(NetService* pNetService, const std::string& ip, int port)
{
	if (m_port != port || m_ip != ip)
	{
		for (int i = 0; i < (int)m_pPackData.size(); i++)
		{
			Send(m_pPackData[i].messageid, m_pPackData[i].p, m_pPackData[i].length, m_pPackData[i].roleid, true);
			m_pPackData[i].Release();
		}
		m_pPackData.clear();
	}
	pNetService->RegistObserver(this);
	m_pNetService = pNetService;
	m_port = port;
	m_ip = ip;
	
	return m_pNetService->Connect(ip, port, boost::bind(&NetClient::OnConnect,this, _1));
}

bool NetClient::Send(int messageid, const char* pdata, int length, int roleid, bool bResend)
{
	if (m_pConnect.get() != nullptr)
	{
		m_pConnect->Send(messageid, pdata, length, roleid);
		return true;
	}
	else
	{
		if (bResend == true)
			m_pPackData.push_back(PackData(messageid, pdata, length,roleid));
	}
	return false;
}




bool NetClient::SendProtoBuf(int messageid, ::google::protobuf::Message& pdata, int roleid, bool bResend)
{
	if (m_pConnect.get() != nullptr)
	{
		m_pConnect->SendBuffer(messageid, pdata, roleid);
	}
	else
	{
		if (bResend == true)
			m_pPackData.push_back(PackData(messageid, pdata, roleid));
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

void NetClient::OnConnect(ConnectPtr& pConnect)
{
	if (pConnect->GetAddress() != getAddress())
		return;

	if (m_pConnect.get() != nullptr)
	{
		m_pConnect->CloseSocket();
	}
	m_pConnect = pConnect;

	if (tTimer != nullptr)
	{
		TimerManager::getInstance().RemoveTimer(tTimer);
		tTimer = nullptr;

		for (int i = 0; i < (int)m_pPackData.size(); i++)
		{
			Send(m_pPackData[i].messageid, m_pPackData[i].p, m_pPackData[i].length, m_pPackData[i].roleid, true);
			m_pPackData[i].Release();
		}
		m_pPackData.clear();	
	}
	
}

void NetClient::OnDisConnect(ConnectPtr& pConnect)
{
	if (pConnect->GetAddress() != getAddress())
		return;

	if (m_pConnect.get() != nullptr)
	{
		m_pConnect->CloseSocket();
		m_pConnect.reset();
	}

	if (tTimer != nullptr)
		TimerManager::getInstance().RemoveTimer(tTimer);

	tTimer = TimerManager::getInstance().AddIntervalTimer(10, boost::bind(&NetClient::ReConnect,this));

}

void NetClient::ReConnect()
{
	std::cout << "reconnect\n";
	m_pNetService->Connect(m_ip, m_port, boost::bind(&NetClient::OnConnect, this, _1));
}