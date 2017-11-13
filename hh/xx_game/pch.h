#pragma once

#include <stdio.h>
#include <tchar.h>

// TODO:  在此处引用程序需要的其他头文件
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include <stdio.h>
#include <tchar.h>



// TODO:  在此处引用程序需要的其他头文件

#include "google/protobuf/message.h"

#include "netserver/NetService.h"
#include "netserver/NetClient.h"
#include "singleton/Singleton.h"
#include "timer/TimerManager.h"
#include "crashReport/crashReport.h"
#include "Config.h"
#include "log/LogService.h"
#include "mysql/DBService.h"
#include <list>
#include <map>

extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include "lua/tolua++.h"
}

using namespace xlib;

#define SAFE_DELETE(p) {if(p!=nullptr){delete p;p=nullptr;}}

#define CHECKERROR(b) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);}
#define CHECKERROR_AND_RETURN(b) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);return;}
#define CHECKERROR_AND_RETURNRESULT(b) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);return b;}
#define CHECKERROR_AND_CONTINUE(b) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);continue;}
#define CHECKERROR_AND_RETURNVALUE(b,v) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);return v;}

#define CHECKERROR_AND_RETURN_LOG(request,pPack){																 \
	int32 count = GetTickCount();																					 \
if (!request.ParseFromArray(pPack->getBuffer(), pPack->getBufferSize()))										 \
{																												 \
	count = GetTickCount() - count;																				 \
	sApp.GetLogService().LogMessage("use:%d, size:%d", count, pPack->getBufferSize());							 \
	std::string dest;																							 \
	\
	ctos(reinterpret_cast<const unsigned char*>(pPack->getBuffer()), pPack->getBufferSize(), dest);				 \
	sApp.GetLogService().LogMessage("buf:%s", dest.c_str());													 \
	CHECKERROR(false);																							 \
	return;																										 \
}}																												 \

#define CHECKERROR_PARAM(b,...) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);sApp.GetLogService().LogMessage(__VA_ARGS__);}
#define CHECKERROR_PARAM_AND_RETURN_RESULT(b,...) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);sApp.GetLogService().LogMessage(__VA_ARGS__);return b;}
#define CHECKERROR_PARAM_AND_RETURN_VALUE(b,v,...) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);sApp.GetLogService().LogMessage(__VA_ARGS__);return v;}
#define CHECKERROR_PARAM_AND_CONTINUE(b,...) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);sApp.GetLogService().LogMessage(__VA_ARGS__);continue;}
#define CHECKERROR_PARAM_AND_RETURN(b,...) if(!(b)) {sApp.GetLogService().LogMessage("error:%s,%d",__FILE__,__LINE__);sApp.GetLogService().LogMessage(__VA_ARGS__);return;}


#define xhash_map std::unordered_map
#define xmap std::map
#define xvector std::vector
#define xlist std::list
#define xset std::set