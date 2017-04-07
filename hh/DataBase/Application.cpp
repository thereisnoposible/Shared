#include "stdafx.h"
#include "Application.h"
#include <random>
#include <boost/thread.hpp>
#include <iostream>

Application* Singleton<Application>::single = nullptr;
//-------------------------------------------------------------------------------------------
Application::Application()
{
    Init();
}

//-------------------------------------------------------------------------------------------
Application::~Application()
{
    m_pNetService->stop();

    DestoryInterface();

    m_pMysqlStmt->Close();
    delete m_pMysqlStmt;
    delete m_pNetService;
}

//-------------------------------------------------------------------------------------------
void Application::Init()
{
    m_pNetService = new NetService(4);

    m_pMysqlStmt = new MysqlStmt;
    m_pMysqlStmt->Open("127.0.0.1", "root", "123456", "accdb");
    InitInterface();

    m_pNetService->start(DATEBASEPORT);
}

//-------------------------------------------------------------------------------------------
void Application::InitInterface()
{
    m_pDBPlayer = new DBPlayer;
	m_pDBObject = new DBObject;
}

void Application::DestoryInterface()
{
    delete m_pDBPlayer;
	delete m_pDBObject;
}

//-------------------------------------------------------------------------------------------
void Application::update(double diff)
{
    m_pNetService->update();
    m_pMysqlStmt->Update();
}

//-------------------------------------------------------------------------------------------
void Application::OnConnect(ConnectPtr&pConnect)
{

}

//-------------------------------------------------------------------------------------------
void Application::OnDisConnect(ConnectPtr& pConnect)
{

}