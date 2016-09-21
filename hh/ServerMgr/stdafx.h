#pragma once

#include "NetMessage.h"
#include "NetService.h"
#include "NetPack.h"
#include "ServerIni.h"
#include "boost/shared_ptr.hpp"
#include "hash_map"
#include "NetClient.h"
#include "Helper.h"

#define LOGSERVERNAME "../log/Server.log"

#include "../hh/Log.h"
#define CHECKERROR(b) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);}
#define CHECKERRORANDRETURN(b) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);return;}
#define CHECKERRORANDRETURNRESULT(b) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);return b;}
#define CHECKERRORANDCONTINUE(b) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);continue;}
#define CHECKERRORANDRETURNVALUE(b,v) if(!(b)) {LogService::getInstance().LogMessage("error:%s,%d",__FILE__,__LINE__);return v;}