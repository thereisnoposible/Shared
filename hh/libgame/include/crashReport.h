#pragma  once
#ifdef _WIN32

XClass long  __stdcall  CrashCallBack(_EXCEPTION_POINTERS* pExInfo);

/*����crash��λ*/
XClass void setCrashReport();

XClass LONG CrashHandler(EXCEPTION_POINTERS *pException);

#endif

