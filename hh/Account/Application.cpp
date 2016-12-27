#include "stdafx.h"
#include "Application.h"
#include <random>
#include <boost/thread.hpp>
#include <iostream>

Application* Singleton<Application>::single = nullptr;
//-------------------------------------------------------------------------------------------
Application::Application()
{
	m_pNetService = new NetService(4);

	m_pNetService->RegistObserver(this);

	m_pTimerManager = new TimerManager();

	m_pLogServer = new LogService("Account");

	m_pLoginManager = new LoginManager;

    m_pNetService->start(ACCOUNTPORT);
}
//-------------------------------------------------------------------------------------------
Application::~Application()
{
    m_pNetService->stop();

	delete m_pLogServer;
	m_pLogServer = nullptr;

	delete m_pTimerManager;
	m_pTimerManager = nullptr;

	delete m_pNetService;
	m_pNetService = nullptr;

	delete m_pLoginManager;
	m_pLoginManager = nullptr;
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

}

//-------------------------------------------------------------------------------------------
void Application::OnConnect(ConnectPtr&pConnect)
{

}

//-------------------------------------------------------------------------------------------
void Application::OnDisConnect(ConnectPtr& pConnect)
{

}
