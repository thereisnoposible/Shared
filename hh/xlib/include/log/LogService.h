#pragma once

#include "../include/define/define.h"
#include "../include/CircularQueue.h"
#include <thread>
#include <boost/thread.hpp>

#define LOG_BUF_LEN 1024*100		// ÿ����־�Ĵ�С
#define FOLDERNAME "log"			// �����־�ļ�����

#pragma warning (push)

#pragma warning (disable: 4251)

namespace xlib
{
	class XDLL LogService2
	{
	public:
		LogService2(const std::string& logname);
		~LogService2(void);

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
		std::string m_LogName;				// ��־�ļ���
		FILE* m_hFile;					// ��ǰҪд����ļ�ָ��
		int64 m_MaxFileSize;			// ��־�ļ���ǰ������
		int64 m_CurFileSize;			// ��ǰ�ļ���С

		boost::mutex m_Mutex;

		char* _pszLog;

		bool m_bActive;
		CCircularQueue<std::string> m_CQueue;	//ѭ������
		std::thread m_thread;
	};


}

#pragma warning (pop)