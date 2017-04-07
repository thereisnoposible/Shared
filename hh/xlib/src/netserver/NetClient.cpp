#include "../include/netserver/NetClient.h"

namespace xlib
{
	NetClient::NetClient(TimerManager* ptimer) :tTimer(nullptr)
	{
		m_pTimer = ptimer;
	}

	NetClient::~NetClient(void)
	{
		if (tTimer != nullptr && m_pTimer != nullptr)
			m_pTimer->RemoveTimer(tTimer);

		if (m_pConnect != nullptr)
		{
			m_pConnect->CloseSocket();
			m_pConnect.reset();
		}

		if (m_pNetService != nullptr)
		{
			m_pNetService->stop();
			delete m_pNetService;
			m_pNetService = nullptr;
		}
	}


	bool NetClient::ConnectTo(const std::string& ip, int port)
	{
		if (m_pNetService == nullptr)
		{
			m_pNetService = new NetService(1);
			registMessage();
		}

		if (m_port != port || m_ip != ip)
		{
			for (int i = 0; i < (int)m_pPackData.size(); i++)
			{
				Send(m_pPackData[i].messageid, m_pPackData[i].p, m_pPackData[i].length, m_pPackData[i].roleid, true);
				m_pPackData[i].Release();
			}
			m_pPackData.clear();
		}

		m_port = port;
		m_ip = ip;

		if (m_pConnect != nullptr)
		{
			m_pConnect->CloseSocket();
			m_pConnect.reset();
		}

		return m_pNetService->Connect(ip, port, boost::bind(&NetClient::OnConnect, this, _1), boost::bind(&NetClient::OnDisConnect, this, _1));
	}

	void NetClient::registMessage()
	{

	}

	void NetClient::RegisterMessage(int id, boost::function<void(PackPtr&)> sfunc)
	{
		m_pNetService->RegisterMessage(id, sfunc);
	}

	bool NetClient::Send(int messageid, const char* pdata, int length, int roleid, bool bResend)
	{
		if (m_pConnect.get() != nullptr)
		{
			m_pConnect->Send(messageid, pdata, length, roleid);
			return true;
		}

		if (bResend == true)
			m_pPackData.push_back(PackData(messageid, pdata, length, roleid));
		return false;
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

	void NetClient::update()
	{
		if (m_pNetService != nullptr)
			m_pNetService->update();

		if (conn_status == status_connect)
		{
			if (m_pConnect != nullptr)
			{
				return;
			}
			m_pConnect = m_pConnectTemp;

			if (tTimer != nullptr && m_pTimer != nullptr)
			{
				m_pTimer->RemoveTimer(tTimer);
				tTimer = nullptr;

				for (int i = 0; i < (int)m_pPackData.size(); i++)
				{
					Send(m_pPackData[i].messageid, m_pPackData[i].p, m_pPackData[i].length, m_pPackData[i].roleid, true);
					m_pPackData[i].Release();
				}
				m_pPackData.clear();
			}
		}

		if (conn_status == status_disconnect)
		{
			if (m_pConnect != nullptr)
			{
				m_pConnect->CloseSocket();
				m_pConnect.reset();
			}

			if (tTimer != nullptr && m_pTimer != nullptr)
			{
				m_pTimer->RemoveTimer(tTimer);
				tTimer = nullptr;
			}

			conn_status = status_free;

			ReConnect();
			if (m_pTimer)
				tTimer = m_pTimer->AddIntervalTimer(10, boost::bind(&NetClient::ReConnect, this));
		}
	}

	void NetClient::OnConnect(ConnectPtr& pConnect)
	{
		conn_status = status_connect;
		m_pConnectTemp = pConnect;
	}

	void NetClient::OnDisConnect(ConnectPtr& pConnect)
	{
		conn_status = status_disconnect;
		m_pConnectTemp = pConnect;
	}

	void NetClient::ReConnect()
	{
		std::cout << "reconnect\n";
		m_pNetService->Connect(m_ip, m_port, boost::bind(&NetClient::OnConnect, this, _1), boost::bind(&NetClient::OnDisConnect, this, _1));
	}
}
