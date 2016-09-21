#include "stdafx.h"
#include "NetService.cpp"
#include "NetClient.cpp"
#include "Helper.cpp"
#include "../hh/Log.cpp"
#include "base64.cpp"
#include "json/json_reader.cpp"
#include "json/json_value.cpp"
#include "json/json_writer.cpp"
#include "WebSocketProtocol.cpp"
#include "TimerManager.cpp"


#ifdef _DEBUG
#pragma comment(lib,"../lib/libgame_d.lib")
#pragma comment(lib,"../lib/libprotobuf_d.lib")
#else
#pragma comment(lib,"../lib/libgame.lib")
#pragma comment(lib,"../lib/libprotobuf.lib")
#endif // DEBUG