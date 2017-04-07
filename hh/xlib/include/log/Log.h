#pragma once

#include "../include/singleton/Singleton.h"
#include <string>

namespace xlib
{
	class LogService : public Singleton<LogService>
	{
	public:
		LogService(const std::string& logname);
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
		inline void SetMaxSize(int maxsize);

	protected:

		/**
		* @brief		��ʼ��logĿ¼
		*/
		void init();

		/**
		* @brief		����һ����־�ļ�
		*/
		bool createLogFile();

	private:
		std::string m_LogName;				// ��־�ļ���
		FILE* m_hFile;						// ��ǰҪд����ļ�ָ��
		long long m_MaxFileSize;			// ��־�ļ���ǰ������
		long long m_CurFileSize;			// ��ǰ�ļ���С
	};
}
