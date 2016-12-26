#include "stdafx.h"
#include "Application.h"
#include <random>
#include <boost/thread.hpp>
#include <iostream>
#include "Initialer.h"

Application* Singleton<Application>::single = nullptr;
//-------------------------------------------------------------------------------------------
Application::Application()
{
	m_pNetService = new NetService(4);

	m_pNetService->RegistObserver(this);

	m_pDBService = new DBService(4);
	m_pDBService->Open("127.0.0.1", "root", "123456", "accdb");
	
	m_pTimerManager = new TimerManager();

	m_pModuleMgr = new ModuleManager;

	m_pLogServer = new LogService(LOGSERVERNAME);

	m_pTypeTable = new TypeTable();

	m_pPlayerMgr = new PlayerManager(m_pDBService);

	m_pMapMgr = new MapManager();


    m_pConnecter = new NetService(1);
	m_pAccount = new NetClient;
    m_pAccount->ConnectTo(m_pConnecter, ACCOUNTADDR, ACCOUNTPORT);

	m_id = 1;
	m_dbid = time(NULL) << 32;

    initial_mgr.SetCallBack(std::bind(&Application::AllInitialOK, this));
    initial_mgr.mgr_Initial();

//	boost::thread maprun(boost::bind(&MapManager::CalcPlayerMove, m_pMapMgr));
}

//-------------------------------------------------------------------------------------------
void Application::AllInitialOK()
{
    m_pNetService->start(SERVERPORT);
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

	delete m_pPlayerMgr;
	m_pPlayerMgr = nullptr;

	delete m_pMapMgr;
	m_pMapMgr = nullptr;

    delete m_pAccount;
    m_pAccount = nullptr;

	delete m_pTimerManager;
	m_pTimerManager = nullptr;

	delete m_pNetService;
	m_pNetService = nullptr;

    delete m_pConnecter;
    m_pConnecter = nullptr;

	delete m_pDBService;
	m_pDBService = nullptr;
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
		m_pTimerManager->Update();
	}

    if (m_pConnecter != nullptr)
    {
        m_pConnecter->update();
    }

    if (m_pAccount != nullptr)
    {
        m_pAccount->update();
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

//-------------------------------------------------------------------------------------------
NetClient& Application::GetDataBaseConnect()
{
	return *m_pDatabase;
}

#define GET_DBIDUNIQUE(unique) unique<<8

int64 Application::getDBID()
{
	if (m_increment > 0xFFFFFF)
	{
		m_dbid = time(NULL);
		m_dbid = m_dbid << 32;
		m_increment = 0;
	}

	return (m_id | m_dbid | GET_DBIDUNIQUE(m_increment++));
}
#undef GET_DBIDUNIQUE