// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Tools.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO:  �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
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