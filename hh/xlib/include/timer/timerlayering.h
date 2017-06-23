#pragma once

#include "../define/define.h"
#include "../include/timer/timer_interface.hpp"
#include "../include/timer/list.h"
#include <boost/pool/object_pool.hpp>

#define TVN_BITS 6
#define TVR_BITS 8
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)

#pragma warning (push)

#pragma warning (disable: 4251)

namespace xlib
{

	struct TVec
	{
		struct list_head vec[TVN_SIZE];
	};

	struct TVec_Root
	{
		struct list_head vec[TVR_SIZE];
	};

	struct STimerInfo
	{
		struct list_head entry;
		uint32 expires;
		uint32 elapse;
		uint32 count;
		void *data;
	};

	class XDLL CTimerLayering : public ITimerComponent
	{
	public:
		CTimerLayering(void);
		virtual ~CTimerLayering(void);

	public:
		bool Init(ITimerReactor *reactor, bool is_exact);
		void UnInit(void);
		virtual HTIMER AddTimer(int32 elapse, int32 count, void *data = 0);
		virtual bool AdjustTimer(HTIMER handle, int32 elapse, int32 count, void *data = 0);
		virtual void RemoveTimer(HTIMER handle);
		virtual int32 RemainCount(HTIMER handle);
		virtual bool Update(int32 elapse);
		virtual void Release(void);

	protected:
		bool __ExactUpdate(void);
		bool __NormalUpdate(int32 elapse);
		STimerInfo *__GetTimerInfoByEntry(list_head *head);

		void __AddTimer(STimerInfo& info);
		bool __TimerPending(STimerInfo& info);
		void __DetachTimer(STimerInfo& info, bool bClearPending);

		int32 __Cascade(TVec* tv, int32 index);
		inline STimerInfo *__Handle2Info(HTIMER handle)
		{
			STimerInfo *info = (STimerInfo*)handle;

			if (!info || !pool_.is_from(info))
			{
				return NULL;
			}

			return info;
		}

	protected:
		bool        is_exact_;
		uint32      last_tick_;

		boost::object_pool<STimerInfo>    pool_;
		TVec_Root	            tv1_;
		TVec		            tv2_;
		TVec		            tv3_;
		TVec		            tv4_;
		TVec		            tv5_;

		ITimerReactor           *reactor_;

	};

}

#pragma warning (pop)