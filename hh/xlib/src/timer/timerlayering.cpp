#ifdef _WIN32
#include <stddef.h>
#else
#include <stddef.h>
#include <time.h>    
unsigned long timeGetTimeLinux()  
{  
	unsigned int uptime = 0;  
	struct timespec on;  
	if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)  
		uptime = on.tv_sec*1000 + on.tv_nsec/1000000;  
	return uptime;  
}
#endif

#include "../include/timer/timerlayering.h"




#define time_after_eq(a,b) ((long)(a) - (long)(b) >= 0) 
#define INDEX(N) ((last_tick_ >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

#ifdef _WIN32
#pragma comment(lib, "winmm.lib")
#endif

#ifdef ZS_WINOS
#include <windows.h>
#endif

using namespace xlib;

CTimerLayering::CTimerLayering(void)
{
	last_tick_ = 0;
	reactor_ = 0;
}

CTimerLayering::~CTimerLayering(void)
{
}

bool CTimerLayering::Init(ITimerReactor *reactor, bool is_exact)
{
	if (!reactor)
	{
		return false;
	}
    else
    {
        reactor_ = reactor;
    }

    //pool_.Init(1);

	for (int32 i = 0; i < TVN_SIZE; i++)
	{
		INIT_LIST_HEAD(tv5_.vec+i);
		INIT_LIST_HEAD(tv4_.vec+i);
		INIT_LIST_HEAD(tv3_.vec+i);
		INIT_LIST_HEAD(tv2_.vec+i);
	}

	for (int32 i = 0; i < TVR_SIZE; i++)
	{
		INIT_LIST_HEAD(tv1_.vec + i);
	}
    is_exact_ = is_exact;
	
#ifdef _WIN32
	last_tick_ = timeGetTime();
#else
	last_tick_ = timeGetTimeLinux();
#endif


    return true;
}

void CTimerLayering::UnInit(void)
{
    //pool_.Destroy();
}

HTIMER CTimerLayering::AddTimer(int32 elapse, int32 count, void *data)
{
	if (elapse <= 0)
	{
		return NULL;
	}

    STimerInfo *info = pool_.construct();
    if (!info)
    {
        return 0;
    }

	info->expires = last_tick_ + elapse;
	info->elapse	= elapse;
    info->count    = count;
    info->data     = data;

    __AddTimer(*info);

    return info;
}

bool CTimerLayering::AdjustTimer(HTIMER handle, int32 elapse, int32 count, void *data)
{
    STimerInfo *info = __Handle2Info(handle);

    if (!info)
    {
        return false;
    }

    if (info->expires == last_tick_ + elapse && __TimerPending(*info))
        return true;

    if (__TimerPending(*info))
    {
        __DetachTimer(*info, true);

        info->expires = last_tick_ + elapse;
        info->elapse = elapse;
        info->count = count;

        if (data)
            info->data = data;

        __AddTimer(*info);
    }
    else
    {
        info->expires = last_tick_ + elapse;
        info->elapse = elapse;
        info->count = count;

        if (data)
            info->data = data;
    }

    return true;
}

void CTimerLayering::RemoveTimer(HTIMER handle)
{
    STimerInfo *info = __Handle2Info(handle);
    if (!info)
    {
        return;
    }

    if (__TimerPending(*info))
    {
        __DetachTimer(*info, true);
        pool_.destroy(info);
    }
    else
    {
        info->count = 0;
    }
}

int32 CTimerLayering::RemainCount(HTIMER handle)
{
    STimerInfo *info = __Handle2Info(handle);
    if (!info)
    {
        return 0;
    }

    return info->count;
}

bool CTimerLayering::Update(int32 elapse)
{
    if (is_exact_)
    {
        return __ExactUpdate();
    }

    return __NormalUpdate(elapse);
}

void CTimerLayering::Release(void)
{
	UnInit();
	delete this;
}

void CTimerLayering::__AddTimer(STimerInfo &info)
{
	uint32 expires = info.expires;

	uint32 idx = expires - last_tick_;
	struct list_head* vec;

	if (idx < TVR_SIZE)
	{
		int32 i = expires & TVR_MASK;
		vec = tv1_.vec + i;
	}
	else if (idx < 1 << (TVR_BITS + TVN_BITS))
	{
		int32 i = (expires >> TVR_BITS) & TVN_MASK;
		vec = tv2_.vec + i;
	}
	else if (idx < 1 <<(TVR_BITS + 2*TVN_BITS))
	{
		int32 i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
		vec = tv3_.vec + i;
	}
	else if (idx < 1 <<(TVR_BITS + 3*TVN_BITS))
	{
		int32 i = (expires >> (TVR_BITS + 2*TVN_BITS)) & TVN_MASK;
		vec = tv4_.vec + i;
	}
	else if ((int32)idx < 0)
	{
		/*  
		 * Can happen if you add a timer with expires == jiffies,  
		 * or you set a timer to go off in the past  
		 */
		vec = tv1_.vec + (last_tick_ & TVR_MASK);
	}
	else
	{
		/* If the timeout is larger than 0xffffffff on 64-bit  
		 * architectures then we use the maximum timeout:  
		 */
		int32 i;
		if (idx > 0xffffffffUL)
		{
			idx = 0xffffffffUL;
			expires = idx + last_tick_;
		}

		i = (expires >> (TVR_BITS + 3*TVN_BITS)) & TVN_MASK;

		vec = tv5_.vec + i;
	}

	list_add_tail(&info.entry, vec);
}

#ifdef _DEBUG
#include <assert.h>
#endif

int32 CTimerLayering::__Cascade(TVec* tv, int32 index)
{
	STimerInfo* timer;
	STimerInfo* tmp;
	struct list_head tv_list;

	list_replace_init(tv->vec + index, &tv_list);

    for (timer = __GetTimerInfoByEntry(tv_list.next), tmp = __GetTimerInfoByEntry(timer->entry.next); &timer->entry != (&tv_list);
        timer = tmp, tmp = __GetTimerInfoByEntry(tmp->entry.next))
    {
        __AddTimer(*timer);
    }

	return index;
}

bool CTimerLayering::__TimerPending(STimerInfo& info)
{
    return info.entry.next != NULL;
}

void CTimerLayering::__DetachTimer(STimerInfo &info, bool bClearPending)
{
    struct list_head *entry = &info.entry;

    __list_del(entry->prev, entry->next);

    if (bClearPending)
        entry->next = NULL;

    entry->prev = (struct list_head *)LIST_POISON2;
}

STimerInfo *CTimerLayering::__GetTimerInfoByEntry(list_head *head)
{
    const struct list_head* ptr = head;
    return (STimerInfo*)((char*)ptr - offsetof(STimerInfo, entry));
}

bool CTimerLayering::__ExactUpdate(void)
{
    bool is_process = false;

#ifdef _WIN32
	uint32 tick = timeGetTime();
#else
	uint32 tick = timeGetTimeLinux();
#endif

    STimerInfo *info;

    while (tick != last_tick_)
    {
        struct list_head work_list;
        struct list_head *head = &work_list;

        int32 index = last_tick_ & TVR_MASK;

        if (!index &&
            (!__Cascade(&tv2_, INDEX(0))) &&
            (!__Cascade(&tv3_, INDEX(1))) &&
            !__Cascade(&tv4_, INDEX(2)))
            __Cascade(&tv5_, INDEX(3));

        list_replace_init(tv1_.vec+ index, &work_list);

        while (!list_empty(head))
        {
            is_process = true;

            info = __GetTimerInfoByEntry(head->next);

            __DetachTimer(*info, true);

            if (0 == info->count)
            {
                pool_.destroy(info);
            }
            else
            {
                if (info->count > 0)
                {
                    --info->count;
                }

                reactor_->OnTimer(info, info->data);

                if (0 == info->count)
                {
					pool_.destroy(info);
                }
                else
                {
#ifdef _WIN32
                    info->expires = timeGetTime() + info->elapse;
#else
					info->expires = timeGetTimeLinux() + info->elapse;
#endif

                    __AddTimer(*info);
                }
            }

        }

        ++last_tick_;
    }


    return is_process;
}

bool CTimerLayering::__NormalUpdate(int32 elapse)
{
    bool is_process = false;
   
#ifdef _WIN32
	uint32 tick = timeGetTime();
#else
	uint32 tick = timeGetTimeLinux();
#endif

    uint32 cur_tick = tick;
    bool is_time_out = false;

    STimerInfo *info = 0;

    while (tick != last_tick_)
    {
        struct list_head work_list;
        struct list_head *head = &work_list;

        int32 index = last_tick_ & TVR_MASK;

        if (!index &&
            (!__Cascade(&tv2_, INDEX(0))) &&
            (!__Cascade(&tv3_, INDEX(1))) &&
            !__Cascade(&tv4_, INDEX(2)))
            __Cascade(&tv5_, INDEX(3));

        list_replace_init(tv1_.vec+ index, &work_list);

        while (!list_empty(head))
        {
            is_process = true;

            info = __GetTimerInfoByEntry(head->next);

            __DetachTimer(*info, true);

            if (0 == info->count)
            {
				pool_.destroy(info);
            }
            else
            {
                if (elapse > 0)
                {
#ifdef _WIN32
                    cur_tick = timeGetTime();
#else
					cur_tick = timeGetTimeLinux();
#endif

                    if (!is_time_out)
                    {
                        if (cur_tick - tick > (uint32)elapse)
                        {
                            is_time_out = true;
                        }
                    }
                }

                if (is_time_out)
                {
                    info->expires = cur_tick;

                    __AddTimer(*info);
                }
                else
                {
                    if (info->count > 0)
                    {
                        --info->count;
                    }

                    reactor_->OnTimer(info, info->data);

                    if (0 == info->count)
                    {
						pool_.destroy(info);
                    }
                    else
                    {
                        info->expires = cur_tick + info->elapse;

                        __AddTimer(*info);
                    }
                }
            }

        }

        ++last_tick_;

        if (is_time_out)
        {
            return is_process;
        }
    }

    return is_process;
}
