/**
 * Copyright (C) 2014, All right reserved.
 * \file			BaseApp.h
 * \version			1.0
 * \author		
 * \brief			Ӧ�ó��������
 * \detail		
 */

#pragma  once

#include "Singleton.h"
#include "TimerManager.h"
#include "LogService.h"
#include "Config.h"


//////////////////////////////////////////////////
/// \enum MessageLevel
/// BaseApp::PrintMessage���Կ���
//////////////////////////////////////////////////
enum MessageLevel
{
	msgDebug = 0,		/**< ���Լ�����Ϣ��ƽ������ʾ */
	msgInfo,			/**< ��ʾ��Ϣ */
	msgError,			/**< ������Ϣ */
};


class XClass BaseApp :public Singleton<BaseApp> 
{
public:
	BaseApp(xstring appname);
	virtual ~BaseApp();

public:
	/**
	* \fn			Run
	* @brief		���������߳�
	* @details		��������Ҫ���ø���ĸú���,����update
	* @return		  
	* @remarks		
	*/
	virtual void Run();


	/** 
	* @brief		����Ļ�ϴ�ӡ��Ϣ
	* @details		���麯����������ʵ��
	* @return		void
	* @remarks	
	*/
	virtual void PrintMessage(MessageLevel level, const char* message, ...) = 0;


	/**
	* \fn				SetStatus
	* @brief		����״̬����ʾ
	* @param[in]	message[xstring]  : ��ʾ����
	* @return		  void
	* @remarks		
	*/
	virtual void SetStatus(const xstring message) = 0;


	/**
	* @brief		��ȡӦ�ó�����
	* @details		�����ɹ��캯�����������
	* @return		xstring : Ӧ�ó�������
	*/
	xstring GetAppName(){return m_Name;}


	/**
	* @brief		��������
	*/
	void Terminate() { m_bTerminate = true; }

	/**
	* @brief		������ʾ����
	*/
	void SetDisplayLevel(Config* pConfig);


protected:		//interface
	/** 
	* @brief		��ʼ������
	* @details		��ʼ��������󣬴��麯��
	* @return		bool  
	* @remarks	
	*/
	virtual bool  initialize() = 0;


	/**
	* @brief		����������Դ
	* @details		���麯��
	* @remarks	
	*/
	virtual void destroy() = 0;



	/**
	* @brief		�˳�Ӧ�ó���
	* @details		���������߳�
	* @remarks		
	*/
	void quit();
	

	/**
	* \fn				update
	* @brief		ÿ֡����
	* @details		��ɶ�ʱ��������
	* @remarks		
	*/
	virtual void update(float fInterval);


private:
	
	/**
	* @brief		�̹߳�������
	* @details		��������ˢ֡
	* @return		void
	* @remarks		
	*/
	void workThreadfun();


protected:
	xstring					m_Name;					/** Ӧ�ó����� */ 
	boost::thread *			m_pWorkThread;			/** ֡�����߳� */ 
	bool					m_Quit;					/** �˳����Ʊ��� */ 

	TimerManager*			m_pTimerManager;		/** ��ʱ��������� */ 

	bool					m_bTerminate;			/** �Ƿ��ж����� */

	LogService*				m_pLogService;			/** ��־������� */

	MessageLevel			_msgLevel;				/**< �����ӡ��Ϣ���� */
};

