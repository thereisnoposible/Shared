#pragma  once
#ifdef _WIN32

long  __stdcall  CrashCallBack(_EXCEPTION_POINTERS* pExInfo, char* sername);

/*安放crash定位*/
void setCrashReport();

LONG CrashHandler(EXCEPTION_POINTERS *pException);

#endif

