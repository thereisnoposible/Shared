#pragma once

#include "NetService.h"

class NetClient
{
public:
	NetClient();
	~NetClient(void);

public:
	bool ConnectTo(NetService* pNetService, const std::string& ip, int port);

	bool Send(int messageid, const char* pdata, int length, int roleid = 0);


	template<typename T>
	bool Send(unsigned int mesageid, const T& t, int roleid = 0)
	{
		return m_pNetService->Send(mesageid, (const char *)&t, sizeof(T), m_address, roleid);
	}


	bool SendProtoBuf(int messageid, ::google::protobuf::Message& pdata, int roleid = 0);


	std::string& getAddress();

	std::string& getIP(){ return m_ip; }

	int getPort(){ return m_port; }

public:
	void OnConnect(boost::shared_ptr<NetConnect>&);
	void OnDisConnect(boost::shared_ptr<NetConnect>&);

private:
	std::string m_ip;
	int m_port;

	std::string m_address;

	NetService* m_pNetService;

	boost::shared_ptr<NetConnect> m_pConnect;

	bool m_bConnect;
};