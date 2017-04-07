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
		* @brief		记录一条日志
		*/
		void LogMessage(const char* format, ...);


		/**
		* @brief		设置文件大小限制
		* @param[in]	maxsize[int32] : 最大文件限制
		* @return		void
		* @remarks
		*/
		inline void SetMaxSize(int maxsize);

	protected:

		/**
		* @brief		初始化log目录
		*/
		void init();

		/**
		* @brief		产生一个日志文件
		*/
		bool createLogFile();

	private:
		std::string m_LogName;				// 日志文件名
		FILE* m_hFile;						// 当前要写入的文件指针
		long long m_MaxFileSize;			// 日志文件当前的容量
		long long m_CurFileSize;			// 当前文件大小
	};
}
