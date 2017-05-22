#pragma once

#include "NetService.h"

class XClass NetClient
{
public:
	NetClient();
	~NetClient(void);

public:
	bool ConnectTo(NetService* pNetService,const xstring& ip,int32 port);

	void set_outerip(const xstring& ip);

	bool Send(int32 messageid,const char* pdata,int32 length,int32 roleid = 0);


	template<typename T>
	bool Send(uint32 mesageid ,const T& t,int32 roleid = 0)
	{
		return m_pNetService->Send(mesageid,(const char *)&t,sizeof(T),m_address,roleid);
	}


	bool SendProtoBuf(int32 messageid ,const ::google::protobuf::Message& pdata,int32 roleid = 0);


	xstring& getAddress();

	xstring& getIP(){ return m_ip; }

	xstring& getOuterIP(){ return m_outerip; }

	int32 getPort(){ return m_port; }

public:
	void onConnect(const xstring& addr);
	void onDisconnect(const xstring& addr);

private:
	xstring m_outerip;

	xstring m_ip;
	int32 m_port;

	xstring m_address;

	NetService* m_pNetService;

	bool m_bConnect;
};

