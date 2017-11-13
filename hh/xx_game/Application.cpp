#include "pch.h"
#include "Application.h"
#include <boost/format.hpp>

extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

SINGLETON_DECLARE_OPERATE(Application);

Application::Application()
{

}

void Application::Init()
{
	m_pConfig = new Config;
	m_pConfig->LoadConfig("server.ini");
	m_pTimerManager = new TimerManager();
	m_pLogService = new LogService2("log/LogicApp");
	m_pNetService = new NetService(4);
	m_pNetService->RegistObserver(this);
	m_pLuaEngine = new LuaEngine;
	m_pLuaEngine->Init();

	m_pDBData = new DBService(4);
	std::string dbip = m_pConfig->GetStringValue("LocalBaseDataApp", "dbip");
	std::string usr = m_pConfig->GetStringValue("LocalBaseDataApp", "usr");
	std::string psw = m_pConfig->GetStringValue("LocalBaseDataApp", "psw");
	std::string dbase = m_pConfig->GetStringValue("LocalBaseDataApp", "dbase");
	int32 dbport = Helper::StringToInt32(m_pConfig->GetStringValue("LocalBaseDataApp", "dbport"));

	//打开ten_type_yf数据库
	if (!m_pDBData->Open(dbip.c_str(), usr.c_str(), psw.c_str(), dbase.c_str(), dbport))
	{
		std::cout << ("Can not open LocalBaseDataApp!");
		//system("pause");
	}


	std::string port = m_pConfig->GetStringValue("LogicApp", "port");
	std::string log_url = "log/AcceptLog";
	m_pNetService->SetLog(log_url);
	m_pNetService->start(Helper::StringToInt32(port));

	std::cout << "server start port " << port << "\n";
}

time_t Application::GetServerTime()
{
	return std::chrono::steady_clock::to_time_t(m_server_time);
}

bool Application::Update()
{
	m_server_time = std::chrono::steady_clock::now();

	if (m_pNetService)
	{
		m_pNetService->update();
	}

	if (m_pTimerManager)
	{
		m_pTimerManager->Update(m_server_time);
	}

	if (m_pDBData)
	{
		m_pDBData->Update();
	}
	return true;
}

void Application::Destroy()
{
	m_pLuaEngine->Destroy();
	m_pNetService->UnRegistObserver(this);
	//后new 先delete
	SAFE_DELETE(m_pLuaEngine);
	SAFE_DELETE(m_pDBData);
	m_pNetService->stop();
	SAFE_DELETE(m_pNetService);
	SAFE_DELETE(m_pLogService);
	SAFE_DELETE(m_pTimerManager);
	SAFE_DELETE(m_pConfig);
}

void Application::OnConnect(ConnectPtr& conn)
{

}

void Application::OnDisConnect(ConnectPtr& conn, const std::string& message)
{

}