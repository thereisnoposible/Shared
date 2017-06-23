#pragma once

#include "../include/define/define.h"
namespace xlib
{
#define MAKEVERSION(b1, b2, b3, b4) \
    ((LIBVERSION)(((uint8)(b1))|(((LIBVERSION)((uint8)(b2)))<<8)|(((LIBVERSION)((uint8)(b3)))<<16)|(((LIBVERSION)((uint8)(b4)))<<24)))

	const LIBVERSION  timer_libversion = MAKEVERSION(1, 0, 0, 0);

	class XDLL ITimerReactor
	{
	public:
		virtual void OnTimer(HTIMER handle, void *data) = 0;

	};

	class XDLL ITimerComponent
	{
	public:
		virtual HTIMER AddTimer(int32 elapse, int32 count, void *data = 0) = 0;
		virtual bool AdjustTimer(HTIMER handle, int32 elapse, int32 count, void *data = 0) = 0;
		virtual void RemoveTimer(HTIMER handle) = 0;

		virtual int32 RemainCount(HTIMER handle) = 0;

		virtual bool Update(int32 elapse = -1) = 0;
		virtual void Release(void) = 0;

	};

	ITimerComponent *CreateTimerComponent(ITimerReactor *reactor, bool exact = false, LIBVERSION ver = timer_libversion);
}
