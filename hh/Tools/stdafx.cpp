// stdafx.cpp : 只包括标准包含文件的源文件
// Tools.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO:  在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
#include "google/protobuf/stubs/common.h"
#include "google/protobuf/descriptor.h"

#ifdef _DEBUG
//#pragma comment(lib,"../lib/libgame_d.lib")
#pragma comment(lib,"lib/libprotobuf_d.lib")
#pragma comment(lib,"lib/libmysql.lib")
#else
#pragma comment(lib,"../lib/libgame.lib")
#pragma comment(lib,"../lib/libprotobuf.lib")
#pragma comment(lib,"../lib/libmysql.lib")
#endif // DEBUG