#pragma once

#include "../include/define/define.h"
#include <string>
#include <time.h>
#include <stdio.h>

namespace xlib
{

#ifndef WIN32
	extern unsigned int GetTickCount(void);
	extern unsigned int timeGetTime(void);
#endif

	class XDLL CLocalTime
	{
	public:
		CLocalTime(void);
		~CLocalTime(void);

		//刷新获取最新本地时间
		void Refresh(void);

		//设置时间
		void Set(time_t tm);

		//返回从1970年1月1日0时0分0到现在经过的秒数(UTC 时间)
		inline uint32 Time(void) { return (uint32)t_; }
		//返回从1970年1月1日0时0分0到现在经过的小时数(UTC 时间)
		uint32 Hours(void);
		//从1970年1月1日0时0分0到现在经过的天数(考虑本地时区)
		uint32 Days(void);
		//从UTC 1970年1月1日0时0分0到现在经过的星期数(考虑本地时区)
		uint32 Weeks(void);
		//从UTC 1970年1月1日0时0分0到现在经过的月数
		uint32 Months(void);
		//从UTC 1970年1月1日0时0分0到现在经过的年数
		uint32 Years(void);

		//秒 取值区间[0, 59]
		uint32 SecondInMinute(void);
		//分 取值区间[0, 59]
		uint32 MinuteInHour(void);
		//时 取值区间[0, 23]
		uint32 HourInDay(void);
		//星期 取值区间[0, 6]其中0代表星期天，1代表星期一
		uint32 DayInWeek(void);
		//月 取值区间[0, 11]从一月开始，0代表一月
		uint32 MonthInYear(void);

	protected:
	private:
		time_t  t_;
		uint32  tick_;

	private:
		static long       time_zone_;
		static CLocalTime cur_time_;

	public:
		//获取本周指定星期日子到1970年1月1日8时0分0经过的秒数(UTC时间),取值区间[1, 7]其中7代表星期天，1代表星期一
		static time_t WeekDay2Time(uint32 dwWeekDay);

		//将从1970年1月1日0时0分0经过的秒数(UTC时间)转换星期,取值区间[1, 7]其中7代表星期天，1代表星期一
		static uint32 Time2WeekDay(time_t tm);

		// 获取第N天的开始时的time
		static uint32 GetDayTime(uint32 day);

		// 获取第N周的开始时的time  按中国习惯，这里用星期一00:00:00(当地时间)作为每周的开始
		static uint32 GetWeekTime(uint32 week);

		// 获取第N月的开始时的time  这里用每月1号00:00:00(当地时间)作为每月的开始
		static uint32 GetMonthTime(uint32 month);

		// 获取第N年的开始时的time  这里用每年1月1号00:00:00(当地时间)作为每月的开始
		static uint32 GetYearTime(uint32 year);

		// 刷新当前时间
		inline static void RefreshNowTime(void) { cur_time_.Refresh(); }

		// 获取当前时间(UTC时间)
		inline static CLocalTime &GetNowTime(void) { return cur_time_; }

		// 获取时区，单位是秒
		inline static long GetTimeZone(void) { return time_zone_; }
	};

}
	