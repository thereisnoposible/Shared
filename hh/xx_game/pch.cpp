// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// LogicApp.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "pch.h"

// TODO:  �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������

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