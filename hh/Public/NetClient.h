#pragma once

#include "NetService.h"
#include "TimerManager.h"

enum c_status
{
    status_free,
    status_connect,
    status_disconnect,
};

class NetClient
{
public:
	NetClient();
	virtual ~NetClient(void);

	struct PackData
	{
		void Release()
		{
			if (p != nullptr)
			{
				delete p;
				p = nullptr;
			}
		}
		PackData(int id,const char* pdata, int len, int _roleid)
		{
			messageid = id;
			p = new char[len];
			memcpy(p, pdata, len);
			length = len;
			roleid = _roleid;
		}
		PackData(int id, ::google::protobuf::Message& pdata, int _roleid = 0)
		{
			messageid = id;
			p = new char[pdata.ByteSize()];
			pdata.SerializeToArray(p, pdata.ByteSize());
			roleid = _roleid;
		}
		int messageid;
		char*p;
		int length;
		int roleid;
	};

public:
    virtual void registMessage();
    void RegisterMessage(int id, boost::function<void(PackPtr&)> sfunc);
    template<class... _Types >
    void Register(int id, _Types... arg)
    {
        m_pNetService->RegisterMessage(id, arg...);
    }

	bool ConnectTo(const std::string& ip, int port);

	bool Send(int messageid, const char* pdata, int length, int roleid = 0, bool bResend = false);
	template<typename T>
	bool Send(const T& t,unsigned int mesageid,  int roleid = 0, bool bResend = false)
	{
		return Send(mesageid, (const char *)&t, sizeof(T), m_address, roleid);
	}
	bool SendProtoBuf(int messageid, ::google::protobuf::Message& pdata, int roleid = 0,bool bResend = false);
	std::string& getAddress();

	std::string& getIP(){ return m_ip; }

	int getPort(){ return m_port; }

    void update();

protected:
    virtual void OnConnect(ConnectPtr&);
    virtual void OnDisConnect(ConnectPtr&);

    void ReConnect();
private:
	std::string m_ip;
    int m_port = 0;
	std::string m_address;
	NetService* m_pNetService;
	ConnectPtr m_pConnect;
    ConnectPtr m_pConnectTemp;
	std::vector<PackData> m_pPackData;
	Timer* tTimer;
    int conn_status = status_free;
};