#include "Singleton.h"
#include <stdlib.h>
extern "C" void external_wrapper(void* p)
{
	atexit((void(*)())p);
}

void  at_exit(void(*func)())
{
	external_wrapper((void*)func);
}

template<typename T, class ThreadingModel, class CreatePolicy, class LifeTimePolicy>
void Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy>::DestroySingleton()
{
	CreatePolicy::Destroy(si_instance);
	si_instance = NULL;
	si_destroyed = true;
}

template<typename T, class ThreadingModel, class CreatePolicy, class LifeTimePolicy>
T& Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy>::Instance()
{
	if (!si_instance)
	{
		// double-checked Locking pattern
		Guard();

		if (!si_instance)
		{
			if (si_destroyed)
			{
				si_destroyed = false;
				LifeTimePolicy::OnDeadReference();
			}

			si_instance = CreatePolicy::Create();
			LifeTimePolicy::ScheduleCall(&DestroySingleton);
		}
	}

	return *si_instance;
}