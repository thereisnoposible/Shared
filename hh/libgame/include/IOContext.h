#pragma once

#define MAX_BUF         1024

enum IO_OPERATION
{
	IO_READ,			/**< ��ȡ�¼� */
	IO_WRITE,			/**< д���¼� */
	IO_END,				/**< ���ӶϿ� */
};

typedef struct _IOContext
{
	WSAOVERLAPPED ol;
	WSABUF wsabuf;
	int8 IoOperation;
}IOContext, *PIOContext;

typedef xlist<PIOContext> IOContextCollect;