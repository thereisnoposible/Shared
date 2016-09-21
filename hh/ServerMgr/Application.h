#pragma once
#include "NetService.h"
#include "Client.h"
#include <vector>
#include "../new/proto/protobuf/hello.pb.h"
#include "../new/proto/protobuf/tonghe.pb.h"
#include "../new/proto/protobuf/object.pb.h"
#include "TimerManager.h"

class Application : public Singleton<Application>
{
public:
	Application();
	~Application();
	void run();
	std::string GetServerAddress()
	{
		return server.getAddress();
	}
	NetClient& GetServer()
	{
		return server;
	}
	void update(double diff);

	void Brocast(int messageid, ::google::protobuf::Message& pdata, int roleid)
	{
		for (int i = 0; i < (int)Clients.size(); ++i)
		{
			if (messageid == 10100)
			{
				int x = Helper::GetRandom(1, 800);
				int y = Helper::GetRandom(1, 600);
				pm_request_move* request = dynamic_cast<pm_request_move*>(&pdata);
				request->set_movetox(x);
				request->set_movetoy(y);
			}
			Clients[i].SendProtoBuf(messageid, pdata, roleid);
		}
	}
private:
	NetService* m_pNetService;
	LogService* m_pLogService;
	TimerManager* m_pTimerManager;
	Client* client;

	NetClient server;

	vector<NetClient> Clients;
};