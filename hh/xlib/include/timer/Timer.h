#pragma once
#include <functional>
#include <time.h>
#include <algorithm>

namespace xlib{

	class Timer
	{
	public:
		Timer() : bRelease(false){}
		virtual ~Timer(){}
		virtual void update(double now)
		{
			if (Passed(now))
			{
				Reset();
				func();
			}
		}

		double time() const
		{
			return _next;
		}

		inline bool Passed(double now) const { return now >= _next; }
		void release(){ bRelease = true; }

		bool IsRelease(){ return bRelease; }

		double _interval; /**< TODO */
		double _next; /**< TODO */
		std::tr1::function<void(void)> func;
		long long _loop_count;
	private:
		inline void Reset()
		{
			_next += _interval;
		}

		bool bRelease;
	};

	class IntervalTimer : public Timer
	{
	public:

		IntervalTimer(double Interval, std::tr1::function<void(void)>& _func, long long loop_count, double now_interval = 0)
		{
			_next = now_interval + Interval;
			_interval = Interval;
			func = _func;
			_loop_count = loop_count;
		}

		//void Update(double diff)
		//{
		//	Timer::update(diff);
		//}
	};

	class TriggerTimer : public Timer
	{
	public:
		TriggerTimer(int _hour, int _minute, int _second, std::tr1::function<void(void)>& _func, long long loop_count, double now_interval = 0)
		{
			func = _func;
			_interval = 24 * 60 * 60;
			time_t tNow = ::time(NULL);
			tm tmCurr;
			localtime_s(&tmCurr, &tNow);
			int count = tmCurr.tm_hour * 60 * 60;
			count += tmCurr.tm_min * 60;
			count += tmCurr.tm_sec;
			count -= _hour * 60 * 60;
			count -= _minute * 60;
			count -= _second;
			if (count < 0)	//Î´À´
				_next = now_interval - count;
			else//¹ýÈ¥
				_next = now_interval + _interval - count;

			_loop_count = loop_count;
		}
		//void Update(double diff)
		//{
		//	Timer::update(diff);
		//}
	};
} //namespace xlib
