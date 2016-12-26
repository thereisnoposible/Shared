#ifdef WIN32

#pragma warning(disable: 4996)
#include <windows.h>
#include <Dbghelp.h>
#pragma auto_inline (off)
#pragma comment( lib, "DbgHelp" )


#include <stdio.h>


#include "crashReport.h"



LPTOP_LEVEL_EXCEPTION_FILTER g_preFilter;


long   __stdcall  CrashCallBack(_EXCEPTION_POINTERS* pExInfo)
{
	struct tm *pTime;
	time_t ctTime;
	time(&ctTime);
	pTime = localtime( &ctTime );
	TCHAR tem[256];
	memset(tem,0,256);
	sprintf(tem,("%d-%d-%d_%d-%d-%d.dmp"),
		1900+pTime->tm_year,1+pTime->tm_mon,pTime->tm_mday,pTime->tm_hour,pTime->tm_min,pTime->tm_sec);



	/*WCHAR szWideProgID[256];
	MultiByteToWideChar(CP_ACP,0,tem,strlen(tem),szWideProgID,sizeof(szWideProgID));*/



	//HANDLE hFile = ::CreateFile( (LPCSTR)szWideProgID, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hFile = ::CreateFile( tem, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo;
		einfo.ThreadId = ::GetCurrentThreadId();
		einfo.ExceptionPointers = pExInfo;
		einfo.ClientPointers = FALSE;
		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, &einfo, NULL, NULL);
		::CloseHandle(hFile);
	}

	if(g_preFilter)
		return g_preFilter(pExInfo);
	else
		return EXCEPTION_CONTINUE_SEARCH; 


}

LONG CrashHandler(EXCEPTION_POINTERS *pException)
{
	// ����Dump�ļ�  
	//  
	CrashCallBack(pException);

	return EXCEPTION_EXECUTE_HANDLER;
}

#endif

void  setCrashReport()
{
#ifdef WIN32
	g_preFilter = SetUnhandledExceptionFilter(CrashCallBack);
#endif
}
