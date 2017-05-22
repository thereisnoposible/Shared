#include "../include/log/Log.h"
#include <io.h>
#include <stdarg.h>

namespace xlib
{
	LogService::LogService(const char*  logname) : m_hFile(NULL)
	{
		m_LogName = logname;
		createLogFile();
	}

	LogService::~LogService()
	{
		if (m_hFile != NULL)
		{

			fclose(m_hFile);
			m_hFile = NULL;
		}
	}

	void LogService::LogMessage(const char* format, ...)
	{
		fopen_s(&m_hFile, m_LogName.c_str(), "a+");

		if (m_hFile == NULL)
			return;
		int size = _filelength(_fileno(m_hFile));
		if (size > 4096)
		{
			int z = m_LogName.find(".log");
			m_LogName.replace(z, 0, "1");
		}
		va_list argp;                   /* 定义保存函数参数的结构 */
		int* argno = 0;                  /* 纪录参数个数 */
		char *para;                     /* 存放取出的字符串参数 */

		/* argp指向传入的第一个可选参数，    msg是最后一个确定的参数 */
		va_start(argp, format);

		std::string logmessa;
		const char* plocal = format;
		while (*plocal != '\0')
		{

			if (*plocal == '%' && *(plocal + 1) == 's')
			{
				para = va_arg(argp, char *);
				logmessa += para;
				plocal += 2;
				continue;
			}
			if (*plocal == '%' && *(plocal + 1) == 'd')
			{
				argno = va_arg(argp, int*);
				char a[10];
				sprintf_s(a, "%d", argno);
				logmessa += a;
				plocal += 2;
				continue;
			}

			logmessa += *plocal;
			plocal++;
		}

		va_end(argp);
		fwrite(logmessa.c_str(), logmessa.length(), 1, m_hFile);
		fclose(m_hFile);
		m_hFile = NULL;
	}

	inline void LogService::SetMaxSize(int maxsize)
	{

	}

	bool LogService::createLogFile()
	{

		return true;
	}
}
