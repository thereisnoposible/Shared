/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		LOGSERVICE.H
** @brief		��־����
**  
** Details	
**  
** @version	1.0
** @author		yuyi
** @date		2014/8/22 9:18
**  
** �޶�˵����	����汾 
** 
****************************************************************************/

#pragma once

#include "Singleton.h"
#include "CircularQueue.h"
#include <thread>

#define LOG_BUF_LEN 1024*100		// ÿ����־�Ĵ�С
#define FOLDERNAME "log"			// �����־�ļ�����


/**
* @brief		��־������
* @details		
* @author		zengxianfan
* @date			2014/8/22 9:22
* @note			�� 
*/ 
class XClass LogService : public Singleton<LogService>
{
public:
	LogService(const xstring& logname);
	~LogService(void);

public:
	/**
	* @brief		��¼һ����־
	*/
	void LogMessage(const char* format, ...);


	/**
	* @brief		�����ļ���С����
	* @param[in]	maxsize[int32] : ����ļ�����
	* @return		void
	* @remarks		
	*/
	inline void SetMaxSize(int32 maxsize);

protected:

	/**
	* @brief		��ʼ��logĿ¼
	*/
	void init();

	/**
	* @brief		����һ����־�ļ�
	*/
	bool createLogFile();

	void ThreadProc(void);

private:
	xstring m_LogName;				// ��־�ļ���
	FILE* m_hFile;					// ��ǰҪд����ļ�ָ��
	int64 m_MaxFileSize;			// ��־�ļ���ǰ������
	int64 m_CurFileSize;			// ��ǰ�ļ���С

	boost::mutex m_Mutex;

	char* _pszLog;

	bool m_bActive;
	CCircularQueue<xstring> m_CQueue;	//ѭ������
	std::thread m_thread;
};

