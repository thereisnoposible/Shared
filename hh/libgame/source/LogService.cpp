#include "pch.h"
#include "helper.h"
#include "LogService.h"
#include <functional>



template<>
LogService* Singleton<LogService>::s_pInstance = NULL;

//-------------------------------------------------------------------------------------
LogService::LogService(const xstring& logname)
	: m_LogName(logname), m_hFile(NULL), m_MaxFileSize(1024 * 1024 * 10), m_CurFileSize(0), m_bActive(true)
{
	_pszLog = new char[LOG_BUF_LEN];
	init();

	m_thread = std::thread(std::bind(&LogService::ThreadProc, this));
}


//-------------------------------------------------------------------------------------
LogService::~LogService(void)
{
	if (NULL != m_hFile)
	{
		fclose(m_hFile);
		m_hFile = NULL;
	}
	SafeDelete(_pszLog);

	m_bActive = false;

	m_thread.join();
}


//-------------------------------------------------------------------------------------
void LogService::SetMaxSize(int32 maxsize)
{
	m_MaxFileSize = maxsize;
}


//-------------------------------------------------------------------------------------
void LogService::init()
{
#ifdef _WIN32
	if(_access(FOLDERNAME, 0))		//判断当前工程中log文件夹是否存在
	{
		_mkdir(FOLDERNAME);
	}
#else
    if(NULL==opendir(FOLDERNAME))
    {
       mkdir(FOLDERNAME,0775);
    }
#endif
}

//-------------------------------------------------------------------------------------
bool LogService::createLogFile()
{
	m_CurFileSize = 0;

	if(m_hFile != NULL)
	{
		fclose(m_hFile);
		m_hFile = NULL;
	}

	//得到新的文件名
	time_t tNow = time(NULL);

	struct tm* ptm;

	#ifdef _WIN32
	struct tm timeinfo;
	localtime_s(&timeinfo,&tNow);
	ptm = &timeinfo;
    #else
	ptm = localtime(&tNow);
    #endif // _WIN32

	char szTime[TIME_LENGTH] = {0};
	strftime(szTime,TIME_LENGTH,"%Y%m%d_%H%M%S",ptm);

	xstringstream ss;
	ss <<  FOLDERNAME << "/" << m_LogName << szTime << ".log";

	xstring filename = ss.str();
#ifdef _WIN32
	m_hFile = _fsopen(filename.c_str(), "a+", _SH_DENYWR);
#else
	m_hFile = fopen(filename.c_str(),"a+");
#endif
	if(m_hFile != NULL)
		return false;

	return true;
}


//-------------------------------------------------------------------------------------
void LogService::LogMessage(const char* format, ...)
{
	// 拼接字符串
	//char szLog[LOG_BUF_LEN]={0};

	boost::mutex::scoped_lock oLock(m_Mutex);

	memset(_pszLog, 0, LOG_BUF_LEN);

	va_list argptr;
	va_start(argptr, format);
	#ifdef _WIN32
	vsnprintf_s(_pszLog, LOG_BUF_LEN - 1, LOG_BUF_LEN - 1, format, argptr);
	#else
	vsnprintf(szLog, LOG_BUF_LEN - 1, format, argptr);
	#endif

	va_end(argptr);

	time_t tNowTime = time(NULL);
	xstringstream ss;
	ss << Helper::Timet2String(tNowTime) << "\t" << _pszLog << "\n";

	m_CQueue.Offer(ss.str());
}

//-------------------------------------------------------------------------------------
void LogService::ThreadProc(void)
{
	while (m_bActive)
	{
		while (1)
		{
			if (NULL == m_hFile)
				createLogFile();

			if (NULL == m_hFile) break;

			if (m_CQueue.Get() == NULL)
				break;

			xstring str = m_CQueue.Poll();

			fwrite(str.c_str(), str.length(), 1, m_hFile);
			fflush(m_hFile);

			m_CurFileSize += str.length();
			if (m_CurFileSize >= m_MaxFileSize)
			{//换新文件
				createLogFile();
			}
		}

		Sleep(1);
	}

}