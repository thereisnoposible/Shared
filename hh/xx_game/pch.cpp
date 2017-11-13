// stdafx.cpp : 只包括标准包含文件的源文件
// LogicApp.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "pch.h"

// TODO:  在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

#ifdef _DEBUG
#pragma comment(lib,"win32/xlib.lib")
#pragma comment(lib,"lib/libprotobuf_d.lib")
#pragma comment(lib,"lib/libeay32.lib")
#pragma comment(lib,"lib/ssleay32.lib")
#else
#pragma comment(lib,"win64/xlib.lib")
#pragma comment(lib,"lib/libprotobuf.lib")
#pragma comment(lib,"lib/libeay64.lib")
#pragma comment(lib,"lib/ssleay64.lib")
//#pragma comment(lib,"lib/libeay32.lib")
//#pragma comment(lib,"lib/ssleay32.lib")
//#pragma comment(lib,"lib/libssl.lib")
#endif // DEBUG

#include "../include/json/json_reader.cpp"
#include "../include/json/json_value.cpp"
#include "../include/json/json_writer.cpp"