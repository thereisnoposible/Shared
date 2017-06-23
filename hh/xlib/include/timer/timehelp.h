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

		//ˢ�»�ȡ���±���ʱ��
		void Refresh(void);

		//����ʱ��
		void Set(time_t tm);

		//���ش�1970��1��1��0ʱ0��0�����ھ���������(UTC ʱ��)
		inline uint32 Time(void) { return (uint32)t_; }
		//���ش�1970��1��1��0ʱ0��0�����ھ�����Сʱ��(UTC ʱ��)
		uint32 Hours(void);
		//��1970��1��1��0ʱ0��0�����ھ���������(���Ǳ���ʱ��)
		uint32 Days(void);
		//��UTC 1970��1��1��0ʱ0��0�����ھ�����������(���Ǳ���ʱ��)
		uint32 Weeks(void);
		//��UTC 1970��1��1��0ʱ0��0�����ھ���������
		uint32 Months(void);
		//��UTC 1970��1��1��0ʱ0��0�����ھ���������
		uint32 Years(void);

		//�� ȡֵ����[0, 59]
		uint32 SecondInMinute(void);
		//�� ȡֵ����[0, 59]
		uint32 MinuteInHour(void);
		//ʱ ȡֵ����[0, 23]
		uint32 HourInDay(void);
		//���� ȡֵ����[0, 6]����0���������죬1��������һ
		uint32 DayInWeek(void);
		//�� ȡֵ����[0, 11]��һ�¿�ʼ��0����һ��
		uint32 MonthInYear(void);

	protected:
	private:
		time_t  t_;
		uint32  tick_;

	private:
		static long       time_zone_;
		static CLocalTime cur_time_;

	public:
		//��ȡ����ָ���������ӵ�1970��1��1��8ʱ0��0����������(UTCʱ��),ȡֵ����[1, 7]����7���������죬1��������һ
		static time_t WeekDay2Time(uint32 dwWeekDay);

		//����1970��1��1��0ʱ0��0����������(UTCʱ��)ת������,ȡֵ����[1, 7]����7���������죬1��������һ
		static uint32 Time2WeekDay(time_t tm);

		// ��ȡ��N��Ŀ�ʼʱ��time
		static uint32 GetDayTime(uint32 day);

		// ��ȡ��N�ܵĿ�ʼʱ��time  ���й�ϰ�ߣ�����������һ00:00:00(����ʱ��)��Ϊÿ�ܵĿ�ʼ
		static uint32 GetWeekTime(uint32 week);

		// ��ȡ��N�µĿ�ʼʱ��time  ������ÿ��1��00:00:00(����ʱ��)��Ϊÿ�µĿ�ʼ
		static uint32 GetMonthTime(uint32 month);

		// ��ȡ��N��Ŀ�ʼʱ��time  ������ÿ��1��1��00:00:00(����ʱ��)��Ϊÿ�µĿ�ʼ
		static uint32 GetYearTime(uint32 year);

		// ˢ�µ�ǰʱ��
		inline static void RefreshNowTime(void) { cur_time_.Refresh(); }

		// ��ȡ��ǰʱ��(UTCʱ��)
		inline static CLocalTime &GetNowTime(void) { return cur_time_; }

		// ��ȡʱ������λ����
		inline static long GetTimeZone(void) { return time_zone_; }
	};

}
	