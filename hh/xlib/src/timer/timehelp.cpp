#include "../include/timer/timehelp.h"

#ifdef ZS_WINOS
#include <windows.h>
#endif

using namespace xlib;

CLocalTime CLocalTime::cur_time_;
long CLocalTime::time_zone_;

CLocalTime::CLocalTime(void)
: t_(0), tick_(::GetTickCount())
{
    if (this == &cur_time_)
    {
        t_ = time(NULL);
#ifdef _WIN32
        _tzset();
        _get_timezone(&time_zone_);
#else
        tzset();
        time_zone_ = timezone;
#endif
    }
}

void CLocalTime::Refresh()
{
    uint32 now_tick = GetTickCount();
    if (now_tick - tick_ >= 1000)
    {
        tick_ = now_tick;
        t_ = time(NULL);
    }
}

void CLocalTime::Set(time_t tm)
{
    t_ = tm;
    tick_ = ::GetTickCount();
}

CLocalTime::~CLocalTime(void)
{

}

uint32 CLocalTime::Hours(void)
{
    return (uint32)t_ / 3600;
}

uint32 CLocalTime::Days(void)
{
    return (uint32)(t_ - time_zone_) / 86400;
}

uint32 CLocalTime::Weeks(void)
{
    //1970年1月1日是星期四
    return(Days() + 3) / 7;
}

uint32 CLocalTime::Months(void)
{
    struct tm *t = localtime(&t_);
    return (t->tm_year - 70) * 12 + t->tm_mon + 1;
}

uint32 CLocalTime::Years(void)
{
    struct tm *t = localtime(&t_);
    return t->tm_year - 70;
}

uint32 CLocalTime::SecondInMinute(void)
{
    return localtime(&t_)->tm_sec;
}

uint32 CLocalTime::MinuteInHour(void)
{
    return localtime(&t_)->tm_min;
}

uint32 CLocalTime::HourInDay(void)
{
    return localtime(&t_)->tm_hour;
}

uint32 CLocalTime::DayInWeek(void)
{
    return localtime(&t_)->tm_wday;
}

uint32 CLocalTime::MonthInYear(void)
{
    return localtime(&t_)->tm_mon;
}

time_t CLocalTime::WeekDay2Time(uint32 week_day)
{
    return GetWeekTime(GetNowTime().Weeks()) + (week_day - 1) * 86400;
}

uint32 CLocalTime::Time2WeekDay(time_t tm)
{
    uint32 wday = localtime(&tm)->tm_wday;
    return wday ? wday : 7;
}

// 获取第N天的开始时的time
uint32 CLocalTime::GetDayTime(uint32 day)
{
    return day * 86400 + time_zone_;
}

// 获取第N周的开始时的time
uint32 CLocalTime::GetWeekTime(uint32 week)
{
    return (week - 1) * 86400 * 7 + 4 * 86400 + time_zone_;
}

// 获取第N月的开始时的time
uint32 CLocalTime::GetMonthTime(uint32 month)
{
    int m = (month - 1) % 12;
    int y = (month - 1) / 12;
    struct tm t = {0, 0, 0, 1, m, y + 70};
    return (uint32)mktime(&t);
}

uint32 CLocalTime::GetYearTime(uint32 year)
{
    struct tm t = {0, 0, 0, 1, 0, (int)year + 70};
    return (uint32)mktime(&t);
}
