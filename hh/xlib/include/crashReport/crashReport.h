#pragma  once
#include "Platform.h"
//#ifdef ZS_WINOS
#include <windows.h>

#include "../include/define/define.h"

namespace xlib
{
	long  __stdcall CrashCallBack(_EXCEPTION_POINTERS* pExInfo, char* sername);

	/*����crash��λ*/
	void XDLL setCrashReport();

	LONG XDLL CrashHandler(EXCEPTION_POINTERS *pException);
}


//#endif

