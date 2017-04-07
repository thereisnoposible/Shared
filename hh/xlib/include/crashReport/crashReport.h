#pragma  once
#include "Platform.h"
#ifdef ZS_WINOS
#include <windows.h>

namespace xlib
{
	long  __stdcall  CrashCallBack(_EXCEPTION_POINTERS* pExInfo, char* sername);

	/*安放crash定位*/
	void setCrashReport();

	LONG CrashHandler(EXCEPTION_POINTERS *pException);
}


#endif

