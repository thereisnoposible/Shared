// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  在此处引用程序需要的其他头文件
#include "NetMessage.h"
#include "NetService.h"
#include "NetPack.h"
#include "ServerIni.h"
#include "boost/shared_ptr.hpp"
#include "hash_map"
#include "Struct.h"
#include <time.h>
#include <chrono>
#include "Helper.h"
#include "Enum.h"
#include <unordered_map>
#include "json/json.h"
#include "DBService.h"

#define LOGSERVERNAME "../log/Server.log"

#include "Log.h"
#define CHECKERROR(b) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);}
#define CHECKERRORANDRETURN(b) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);return;}
#define CHECKERRORANDRETURNRESULT(b) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);return b;}
#define CHECKERRORANDCONTINUE(b) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);continue;}
#define CHECKERRORANDRETURNVALUE(b,v) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);return v;}