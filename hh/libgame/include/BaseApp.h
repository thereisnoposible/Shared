/**
 * Copyright (C) 2014, All right reserved.
 * \file			BaseApp.h
 * \version			1.0
 * \author		
 * \brief			应用程序类基类
 * \detail		
 */

#pragma  once

#include "Singleton.h"
#include "TimerManager.h"
#include "LogService.h"
#include "Config.h"


//////////////////////////////////////////////////
/// \enum MessageLevel
/// BaseApp::PrintMessage调试开关
//////////////////////////////////////////////////
enum MessageLevel
{
	msgDebug = 0,		/**< 调试级别信息，平常不显示 */
	msgInfo,			/**< 显示信息 */
	msgError,			/**< 错误信息 */
};


class XClass BaseApp :public Singleton<BaseApp> 
{
public:
	BaseApp(xstring appname);
	virtual ~BaseApp();

public:
	/**
	* \fn			Run
	* @brief		启动工作线程
	* @details		派生类需要调用父类的该函数,驱动update
	* @return		  
	* @remarks		
	*/
	virtual void Run();


	/** 
	* @brief		在屏幕上打印信息
	* @details		纯虚函数，派生类实现
	* @return		void
	* @remarks	
	*/
	virtual void PrintMessage(MessageLevel level, const char* message, ...) = 0;


	/**
	* \fn				SetStatus
	* @brief		设置状态栏显示
	* @param[in]	message[xstring]  : 显示内容
	* @return		  void
	* @remarks		
	*/
	virtual void SetStatus(const xstring message) = 0;


	/**
	* @brief		获取应用程序名
	* @details		返回由构造函数传入的名称
	* @return		xstring : 应用程序名称
	*/
	xstring GetAppName(){return m_Name;}


	/**
	* @brief		结束运行
	*/
	void Terminate() { m_bTerminate = true; }

	/**
	* @brief		设置显示级别
	*/
	void SetDisplayLevel(Config* pConfig);


protected:		//interface
	/** 
	* @brief		初始化工作
	* @details		初始化所需对象，纯虚函数
	* @return		bool  
	* @remarks	
	*/
	virtual bool  initialize() = 0;


	/**
	* @brief		销毁所有资源
	* @details		纯虚函数
	* @remarks	
	*/
	virtual void destroy() = 0;



	/**
	* @brief		退出应用程序
	* @details		结束工作线程
	* @remarks		
	*/
	void quit();
	

	/**
	* \fn				update
	* @brief		每帧调用
	* @details		完成定时器管理工作
	* @remarks		
	*/
	virtual void update(float fInterval);


private:
	
	/**
	* @brief		线程工作函数
	* @details		负责驱动刷帧
	* @return		void
	* @remarks		
	*/
	void workThreadfun();


protected:
	xstring					m_Name;					/** 应用程序名 */ 
	boost::thread *			m_pWorkThread;			/** 帧驱动线程 */ 
	bool					m_Quit;					/** 退出控制变量 */ 

	TimerManager*			m_pTimerManager;		/** 定时器管理对象 */ 

	bool					m_bTerminate;			/** 是否中断运行 */

	LogService*				m_pLogService;			/** 日志服务对象 */

	MessageLevel			_msgLevel;				/**< 界面打印消息级别 */
};

