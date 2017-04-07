#include "stdafx.h"
#include "Application.h"
#include <random>
#include <boost/thread.hpp>
#include <iostream>
#include "Initialer.h"

#include "table/type_sell_props.h"
#include "table/type_prop.h"
#include "table/type_npc_talk.h"
#include "table/type_npc_action.h"

Application* Singleton<Application>::single = nullptr;

//-------------------------------------------------------------------------------------------
Application::Application()
{
	m_pNetService = new NetService(4);

	m_pNetService->RegistObserver(this);

	m_pMysqlStmt = new MysqlStmt;
	m_pMysqlStmt->Open("127.0.0.1", "root", "123456", "accdb");

	m_pDBService = new DBService(4);
	m_pDBService->Open("127.0.0.1", "root", "123456", "accdb");

	loadTable();
	
	m_pTimerManager = new TimerManager();
	m_pTimeWheel = new timewheel::TimeWheel();
	m_pTimeWheel->Init(30, 200);

	m_pModuleMgr = new ModuleManager;

	m_pLogServer = new LogService(LOGSERVERNAME);

	m_pTypeTable = new TypeTable();

	m_pPlayerMgr = new PlayerManager(m_pDBService);

	m_pMapMgr = new MapManager();


    m_pAccount = new AccountNetClient;
    m_pDatabase = new DBNetClient;
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
    std::cout << "start server\n";
}

//-------------------------------------------------------------------------------------------
Application::~Application()
{
    m_pNetService->stop();

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

	delete m_pNetService;
	m_pNetService = nullptr;

	delete m_pDBService;
	m_pDBService = nullptr;

    delete m_pTimerManager;
    m_pTimerManager = nullptr;

	delete m_pTimeWheel;
	m_pTimeWheel = nullptr;
}

//-------------------------------------------------------------------------------------------
void Application::update(double diff)
{
	std::chrono::steady_clock::time_point fCurr = std::chrono::steady_clock::now();
	time_t tNow = std::chrono::steady_clock::to_time_t(fCurr);

	if (m_pNetService != nullptr)
	{
		m_pNetService->update();
	}

	if (m_pTimerManager != nullptr)
	{
		m_pTimerManager->Update(fCurr);
	}

	if (m_pTimeWheel)
	{
		m_pTimeWheel->Update();
	}

    if (m_pAccount != nullptr)
    {
        m_pAccount->update();
    }

    if (m_pDatabase)
    {
        m_pDatabase->update();
    }
}

//-------------------------------------------------------------------------------------------
void Application::OnConnect(ConnectPtr&pConnect)
{

}

//-------------------------------------------------------------------------------------------
void Application::OnDisConnect(ConnectPtr& pConnect)
{
	m_pPlayerMgr->getInstance().OnDisConnect(pConnect);
}

//-------------------------------------------------------------------------------------------
void Application::loadTable()
{
	if (s_type_npc_actionTable.create())
		s_type_npc_actionTable.Load();

	if (s_type_npc_talkTable.create())
		s_type_npc_talkTable.Load();

	if (s_type_propTable.create())
		s_type_propTable.Load();

	if (s_type_sell_propsTable.create())
		s_type_sell_propsTable.Load();
}

//-------------------------------------------------------------------------------------------
AccountNetClient& Application::GetAccountConnect()
{
	return *m_pAccount;
}

//-------------------------------------------------------------------------------------------
DBNetClient& Application::GetDataBaseConnect()
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