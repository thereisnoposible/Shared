// stdafx.cpp : 只包括标准包含文件的源文件
// Account.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO:  在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
#include "NetService.cpp"
#include "Helper.cpp"
#include "TimerManager.cpp"
#include "WebSocketProtocol.cpp"
#include "base64.cpp"
#include "json/json_reader.cpp"
#include "json/json_value.cpp"
#include "json/json_writer.cpp"
#include "DBService.cpp"
#include "DBResult.cpp"
#include "DBSqlThread.cpp"
#include "Log.cpp"

#ifdef _DEBUG
#pragma comment(lib,"../lib/libprotobuf_d.lib")
#pragma comment(lib,"../lib/libmysql.lib")
#else
#pragma comment(lib,"../lib/libprotobuf.lib")
#pragma comment(lib,"../lib/libmysql.lib")
#endif // DEBUG