#include "stdafx.h"
#include "Application.h"
#include <boost/thread.hpp>

template<> Application* Singleton<Application>::single = NULL;
//-------------------------------------------------------------------------------------------
Application::Application()
{
	m_pNetService = new NetService(4);
	m_pTimerManager = new TimerManager();

	server.ConnectTo(m_pNetService, SERVERADDR, SERVERPORT);

    //std::vector<NetClient> pppp;
    //for (int i = 0; i < 70000; i++)
    //    pppp.push_back(NetClient());

    //for (int i = 0; i < 70000; i++)
    //{
    //    pppp[i].ConnectTo(m_pNetService, "192.168.0.21", 50000);
    //}

	//server.ConnectTo(m_pNetService, "120.92.226.24", 80);
	m_pLogService = new LogService("Client");

	client = new Client;
//	client->CreateAccount();
	boost::thread thr(boost::bind(&Client::update, client));
}

//-------------------------------------------------------------------------------------------
Application::~Application()
{
	delete m_pTimerManager;
	m_pTimerManager = nullptr;

	delete m_pLogService;
	m_pLogService = nullptr;

	delete client;
	client = nullptr;

	delete m_pNetService;
	m_pNetService = nullptr;
}

//-------------------------------------------------------------------------------------------
void Application::run()
{
	NetService::getInstance().run();
}

//-------------------------------------------------------------------------------------------
void Application::update(double diff)
{
	NetService::getInstance().update();

	m_pTimerManager->Update();
}