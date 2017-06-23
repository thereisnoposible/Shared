#include "../include/timer/timer_interface.hpp"
#include "../include/timer/timerlayering.h"

using namespace xlib;

LIBVERSION __timer_libversion = timer_libversion;

static bool CheckVersion(LIBVERSION libversion)
{
	if (__timer_libversion == libversion)
	{
		return true;
	}

	return false;
}

ITimerComponent *CreateTimerComponent(ITimerReactor *reactor, bool exact, LIBVERSION ver)
{
	if (!CheckVersion(ver))
	{
		return 0;
	}

	CTimerLayering *timer_impl = new CTimerLayering();
	if (timer_impl)
	{
		if (!timer_impl->Init(reactor, exact))
		{
			timer_impl->UnInit();
			delete timer_impl;
			timer_impl = 0;
		}
	}

	return timer_impl;
}
