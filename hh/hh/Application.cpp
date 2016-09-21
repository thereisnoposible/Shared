#include "stdafx.h"
#include "Application.h"
#include <random>
#include <boost/thread.hpp>
#include <iostream>

Application* Singleton<Application>::single = nullptr;
//-------------------------------------------------------------------------------------------
Application::Application()
{
	m_pNetService = new NetService(SERVERPORT,4);

	m_pNetService->RegistObserver(this);

	m_pDBService = new DBService(4);
	m_pDBService->Open("127.0.0.1", "root", "123456", "accdb");
	
	m_pTimerManager = new TimerManager();

	m_pModuleMgr = new ModuleManager;

	m_pLogServer = new LogService(LOGSERVERNAME);

	m_pTypeTable = new TypeTable();

	m_pFightMgr = new FightManager();

	m_pPlayerMgr = new PlayerManager(m_pDBService);

	m_pMapMgr = new MapManager();

	m_pAccount = new NetClient;
	m_pAccount->ConnectTo(m_pNetService, ACCOUNTADDR, ACCOUNTPORT);

//	boost::thread maprun(boost::bind(&MapManager::CalcPlayerMove, m_pMapMgr));
}
//-------------------------------------------------------------------------------------------
Application::~Application()
{
	delete m_pModuleMgr;
	m_pModuleMgr = nullptr;

	delete m_pLogServer;
	m_pLogServer = nullptr;

	delete m_pTypeTable;
	m_pTypeTable = nullptr;

	delete m_pFightMgr;
	m_pFightMgr = nullptr;

	delete m_pPlayerMgr;
	m_pPlayerMgr = nullptr;

	delete m_pMapMgr;
	m_pMapMgr = nullptr;

	delete m_pTimerManager;
	m_pTimerManager = nullptr;

	delete m_pAccount;
	m_pAccount = nullptr;

	delete m_pNetService;
	m_pNetService = nullptr;

	delete m_pDBService;
	m_pDBService = nullptr;
}
//-------------------------------------------------------------------------------------------
void Application::run()
{
	NetService::getInstance().run();
}
//-------------------------------------------------------------------------------------------
void Application::update(double diff)
{
	if (m_pNetService != nullptr)
	{
		m_pNetService->update();
	}

	if (m_pTimerManager != nullptr)
	{
		m_pTimerManager->Update(diff);
	}

}

//-------------------------------------------------------------------------------------------
void Application::OnConnect(ConnectPtr&pConnect)
{
//	m_pPlayerMgr->getInstance().OnPlayerLogin(pConnect);
}

//-------------------------------------------------------------------------------------------
void Application::OnDisConnect(ConnectPtr& pConnect)
{
	m_pPlayerMgr->getInstance().OnDisConnect(pConnect);
}

//-------------------------------------------------------------------------------------------
NetClient& Application::GetAccountConnect()
{
	return *m_pAccount;
}