#pragma once

#define MAX_BUF         1024

enum IO_OPERATION
{
	IO_READ,			/**< 读取事件 */
	IO_WRITE,			/**< 写入事件 */
	IO_END,				/**< 连接断开 */
};

typedef struct _IOContext
{
	WSAOVERLAPPED ol;
	WSABUF wsabuf;
	int8 IoOperation;
}IOContext, *PIOContext;

typedef xlist<PIOContext> IOContextCollect;