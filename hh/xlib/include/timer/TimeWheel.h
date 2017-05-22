#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <list>
#include <functional>

#include "../include/define/define.h"

namespace xlib{

	namespace timewheel
	{

		struct Timer;
		template class _declspec(dllexport) std::function<void()>;
		template class _declspec(dllexport) std::list<Timer*>;
		template class _declspec(dllexport) std::vector<std::list<Timer*>>;
		template class _declspec(dllexport) std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period>;
		template class _declspec(dllexport) std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>;

		struct XDLL Timer
		{
			Timer() :bRelease(false){}
			int day;
			int hour;
			int min;
			int sec;
			int msec;
			long long interval;
			std::function<void()> cb_func;

			int count;

			bool bRelease;
		};

		struct XDLL TimeWheel
		{
		public:
			enum TimeType
			{
				time_day,
				time_hour,
				time_min,
				time_sec,
				time_msec,
			};

			void Init(int day_count, int millsec_count);
			void Update();

			void RemoveTimer(Timer* pTimer)
			{
				pTimer->bRelease = true;
			}

			Timer* AddTimer(int hour, int min, int sec, int msec, std::function<void()> cb_func, int interval = 86400000);
			Timer* AddTimer(long long interval, std::function<void()> cb_func);

			void AddTimer(int hour, int min, int sec, int msec, std::function<void()> cb_func, int count, int interval = 86400000);
			void AddTimer(long long interval, std::function<void()> cb_func, int count);

		protected:
			inline bool AddTime(int t, int&temp, int point, int interval, bool bAdd);
			inline int GetTimerDiff(int hour, int min, int sec, int msec);
			inline long long GetTimerTick(Timer* pTimer);

			inline Timer* CreateTimer(long long diff, std::function<void()> cb_func, long long interval, int count);

			inline void UpdateMsecWheel();
			inline void UpdateSecWheel();
			inline void UpdateMinWheel();
			inline void UpdateHourWheel();
			inline void UpdateDayWheel();
			inline void excuteTimer(Timer* pTimer);

		private:
			long long _interval; //∫¡√Îº∂

			std::chrono::steady_clock::time_point _create_time;
			std::chrono::steady_clock::time_point _update_time;

			std::vector<std::list<Timer*>> day_wheel;       //»’¬÷
			std::vector<std::list<Timer*>> hour_wheel;      // ±¬÷
			std::vector<std::list<Timer*>> min_wheel;       //∑÷¬÷
			std::vector<std::list<Timer*>> sec_wheel;       //√Î¬÷
			std::vector<std::list<Timer*>> msec_wheel;   //∫¡√Î¬÷

			int day_point;          //»’’Î
			int hour_point;         // ±’Î
			int min_point;          //∑÷’Î
			int sec_point;          //√Î’Î
			int msec_point;      //∫¡√Î’Î

			long long msec_count;

		};
	}

}