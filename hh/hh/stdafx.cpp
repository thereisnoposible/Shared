#include "stdafx.h"
#include "NetService.cpp"
#include "Helper.cpp"
#include "TypeTable.cpp"
#include "TimerManager.cpp"
#include "WebSocketProtocol.cpp"
#include "base64.cpp"
#include "json/json_reader.cpp"
#include "json/json_value.cpp"
#include "json/json_writer.cpp"
#include "DBService.cpp"
#include "DBResult.cpp"
#include "DBSqlThread.cpp"
#include "NetClient.cpp"

#ifdef _DEBUG
//#pragma comment(lib,"../lib/libgame_d.lib")
#pragma comment(lib,"../lib/libprotobuf_d.lib")
#pragma comment(lib,"../lib/libmysql.lib")
#else
#pragma comment(lib,"../lib/libgame.lib")
#pragma comment(lib,"../lib/libprotobuf.lib")
#pragma comment(lib,"../lib/libmysql.lib")
#endif // DEBUG