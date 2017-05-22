#pragma  once
#ifdef _WIN32

XClass long  __stdcall  CrashCallBack(_EXCEPTION_POINTERS* pExInfo);

/*安放crash定位*/
XClass void setCrashReport();

XClass LONG CrashHandler(EXCEPTION_POINTERS *pException);

#endif

