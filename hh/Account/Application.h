#pragma once
#include "NetService.h"
#include "Log.h"
#include "TimerManager.h"
#include "Login.h"

class Application : public NetObserver, public Singleton<Application>
{
public:
	Application();
	~Application();
	void run();
	void update(double diff);

	void OnConnect(ConnectPtr&);
	void OnDisConnect(ConnectPtr&);
private:
	NetService* m_pNetService;
	LogService* m_pLogServer;
	TimerManager* m_pTimerManager;
	LoginManager* m_pLoginManager;
};