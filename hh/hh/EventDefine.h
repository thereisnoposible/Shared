#ifndef _EVENT_DEFINE_H_
#define _EVENT_DEFINE_H_
//#include "../Public.h"
//#include "../SmartPtr/IntrusivePtr.h"
#include <memory>

#define EVENT_SYSTEM_READ_NET 0x01000001 // 接收到字节流过来的事件
#define EVENT_SYSTEM_SEND_NET 0x81000001 // 发送到字节流到的调度服务器的事件

namespace ZeroSpace
{
	class CEventHandlerBase;
	typedef std::shared_ptr< CEventHandlerBase > EventHandlerPtr;
}

#endif // !_EVENT_DEFINE_H_