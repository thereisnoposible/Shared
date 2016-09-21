#pragma once

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
#include "BaseModule.h"
#include "Enum.h"
#include "TypeTable.h"
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