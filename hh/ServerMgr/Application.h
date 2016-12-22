#pragma once
#include "NetService.h"
#include "Client.h"
#include <vector>
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
private:
	NetService* m_pNetService;
	LogService* m_pLogService;
	TimerManager* m_pTimerManager;
	Client* client;

	NetClient server;

	vector<NetClient> Clients;
};