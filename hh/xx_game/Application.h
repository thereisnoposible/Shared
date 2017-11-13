#pragma once
#include "LuaEngine.h"

class Application :public NetObserver
{

	SINGLETON_DECLARE(Application);
public:
	bool Update();

	void Destroy();

	Config& GetConfig()
	{
		return *m_pConfig;
	}

	NetService& GetNetService()
	{
		return *m_pNetService;
	}

	time_t GetServerTime();

	void OnConnect(ConnectPtr&);
	void OnDisConnect(ConnectPtr&, const std::string& message);

	LogService2& GetLogService()
	{
		return *m_pLogService;
	}

	DBService* GetDBData()
	{
		return m_pDBData;
	}

	LuaEngine& GetLuaEngine()
	{
		return *m_pLuaEngine;
	}

	TimerManager& GetTimerManager()
	{
		return *m_pTimerManager;
	}

	void Init();
protected:


private:
	Config* m_pConfig;
	NetService* m_pNetService;
	LogService2* m_pLogService;
	TimerManager* m_pTimerManager;
	LuaEngine* m_pLuaEngine;

	DBService* m_pDBData;

	std::chrono::steady_clock::time_point m_server_time;
};

#define sApp Application::GetInstance()
#define sNetService sApp.GetNetService()
#define sDBData sApp.GetDBData()
#define sTimerManager sApp.GetTimerManager()
#define sConfig sApp.GetConfig()