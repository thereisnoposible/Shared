#pragma  once


#include "pch.h"
#include "helper.h"
#include "BaseApp.h"
#include "TimerManager.h"
#include "google/protobuf/stubs/common.h"
#include "crashReport.h"

template<> BaseApp*  Singleton<BaseApp>::s_pInstance=NULL;

//---------------------------------------------------------------------------------------
BaseApp::BaseApp(xstring appname)
:m_Name(appname), m_pWorkThread(NULL), m_Quit(false), m_bTerminate(false), _msgLevel(msgDebug)
{
	Helper::setCurrentWorkPath();

#ifdef _DEBUG
	Helper::EnableMemLeakCheck();
#endif

	m_pLogService = new LogService(m_Name);
	m_pTimerManager = new TimerManager();
}



//---------------------------------------------------------------------------------------
BaseApp::~BaseApp()
{
	SafeDelete(m_pWorkThread);
	SafeDelete(m_pTimerManager);
	SafeDelete(m_pLogService);
}


//---------------------------------------------------------------------------------------
void  BaseApp::Run()
{
	SafeDelete(m_pWorkThread);

	//帧驱动线程
	m_pWorkThread=new boost::thread(boost::bind(&BaseApp::workThreadfun,this));

	return ;
}


//---------------------------------------------------------------------------------------
void  BaseApp::update(float fInterval)
{
	//定时器管理
	TimerManager::GetSingleton().Update(fInterval);

	return ; 
}


//---------------------------------------------------------------------------------------
void BaseApp::quit()
{
	m_Quit=true;

	//等待线程退出
	if(m_pWorkThread!=NULL) 
		m_pWorkThread->join();
}


//---------------------------------------------------------------------------------------
void  BaseApp::workThreadfun()
{
	//调用初始化接口
	//初始化随机数种子
	srand((unsigned int)time(NULL));

#ifdef _WIN32
	__try
	{
#endif

	bool b=initialize();

	if (!b)
		return;

#ifdef _WIN32
	}
	__except (CrashHandler(GetExceptionInformation()))
	{
		return;
	}
#endif

	double fLast =Helper::GetFrequencyTime();
	while(m_Quit==false)
	{
		double fCurr =Helper::GetFrequencyTime();
		double fSpan = fCurr - fLast;
		//在这里控制帧率
		if( fSpan < 0.010 )
		{
			Sleep(1);
		}
		fLast = fCurr;

#ifdef _WIN32
		__try
		{
#endif
			update((float)fSpan);				//派生类型将重载该函数
#ifdef _WIN32
		}
		__except (CrashHandler(GetExceptionInformation()))
		{

		}
#endif
	}

	//必须在派生对象还没有销毁时调用
	LogService::GetSingleton().LogMessage("application exit!");

#ifdef _WIN32
	__try
	{
#endif
	destroy();

#ifdef _WIN32
	}
	__except (CrashHandler(GetExceptionInformation()))
	{

	}
#endif


}

//-------------------------------------------------------------------------------------------
void BaseApp::SetDisplayLevel(Config* pConfig)
{
	_msgLevel = (MessageLevel)(pConfig->GetIntValue("DisplayLevel", "level"));
}