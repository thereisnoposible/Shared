#pragma  once
#ifdef _WIN32

long  __stdcall  CrashCallBack(_EXCEPTION_POINTERS* pExInfo, char* sername);

/*����crash��λ*/
void setCrashReport();

LONG CrashHandler(EXCEPTION_POINTERS *pException);

#endif

