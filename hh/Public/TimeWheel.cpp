#include "TimeWheel.h"

namespace timewheel
{
    void TimeWheel::Init(int day_count, int millsec_count)
    {
        _create_time = std::chrono::steady_clock::now();
        _update_time = _create_time;
        _interval = 0;
        day_point = 0;
        hour_point = 0;
        min_point = 0;
        sec_point = 0;
        msec_point = 0;
        msec_count = 0;

        day_wheel.resize(day_count);
        hour_wheel.resize(24);
        min_wheel.resize(60);
        sec_wheel.resize(60);
        msec_wheel.resize(millsec_count > 1000 ? 1000 : millsec_count);
    }

    Timer* TimeWheel::AddTimer(int hour, int min, int sec, int msec, std::function<void()> cb_func, int interval)
    {
        int diff = GetTimerDiff(hour, min, sec, msec);

        Timer* pTimer = CreateTimer(diff, cb_func, interval, -1);
        return pTimer;
    }

    void TimeWheel::AddTimer(int hour, int min, int sec, int msec, std::function<void()> cb_func, int count, int interval)
    {
        int diff = GetTimerDiff(hour, min, sec, msec);

        CreateTimer(diff, cb_func, interval, count);
    }

    int TimeWheel::GetTimerDiff(int hour, int min, int sec, int msec)
    {
        long long now_interval = 0;
        long long interval = 0;
#ifdef WIN32
        SYSTEMTIME  time;
        GetLocalTime(&time);

        now_interval = time.wHour * 3600000;
        now_interval += time.wMinute * 60000;
        now_interval += time.wSecond * 1000;
        now_interval += time.wMilliseconds;

        interval = hour * 3600000;
        interval += min * 60000;
        interval += sec * 1000;
        interval += msec;
#endif

        int diff = (int)(interval - now_interval);
        if (diff <= 0)
        {
            diff += 86400000;
        }

        return diff;
    }

    Timer* TimeWheel::AddTimer(int interval, std::function<void()> cb_func)
    {
        Timer* pTimer = CreateTimer(interval, cb_func, interval, -1);

        return pTimer;
    }

    void TimeWheel::AddTimer(int interval, std::function<void()> cb_func, int count)
    {
        if (count == 0)
            return;

        CreateTimer(interval, cb_func, interval, count);
    }

    Timer* TimeWheel::CreateTimer(int diff, std::function<void()> cb_func, int interval, int count)
    {
        int day = diff / 86400000;
        int hour = (diff / 3600000) % 24;
        int min = (diff / 60000) % 60;
        int sec = (diff / 1000) % 60;
        int msec = (diff % 1000) * msec_wheel.size() / 1000;

        Timer* pTimer = new Timer;

        if (AddTime(hour, pTimer->hour, hour_point, 24,
            AddTime(min, pTimer->min, min_point, 60,
            AddTime(sec, pTimer->sec, sec_point, 60,
            AddTime(msec, pTimer->msec, msec_point + 1, msec_wheel.size(), false)))))
            day++;
        pTimer->day = day_point + day;

        pTimer->cb_func = cb_func;
        pTimer->interval = interval;
        pTimer->count = count;



        if (pTimer->day != day_point)
        {
            day_wheel[pTimer->day].push_back(pTimer);
            return pTimer;
        }
        if (pTimer->hour != hour_point)
        {
            hour_wheel[pTimer->hour].push_back(pTimer);
            return pTimer;
        }
        if (pTimer->min != min_point)
        {
            min_wheel[pTimer->min].push_back(pTimer);
            return pTimer;
        }
        if (pTimer->sec != sec_point)
        {
            sec_wheel[pTimer->sec].push_back(pTimer);
            return pTimer;
        }

        msec_wheel[pTimer->msec].push_back(pTimer);

        return pTimer;
    }

    void TimeWheel::Update()
    {
        _update_time = std::chrono::steady_clock::now();
        std::chrono::duration<long long, std::ratio<1, 1000>> usetime = std::chrono::duration_cast<std::chrono::duration<long long, std::ratio<1, 1000>>>(_update_time - _create_time);
        _interval = usetime.count();

        long long curr_mec = _interval * msec_wheel.size() / 1000;
        for (long long i = msec_count; i < curr_mec; i++)
        {
            msec_count++;

            msec_point++;
            if (msec_point == msec_wheel.size())
            {
                msec_point = 0;
                sec_point++;
                if (sec_point == sec_wheel.size())
                {
                    sec_point = 0;
                    min_point++;
                    if (min_point == min_wheel.size())
                    {
                        min_point = 0;
                        hour_point++;
                        if (hour_point == hour_wheel.size())
                        {
                            hour_point = 0;
                            day_point++;
                            UpdateDayWheel();
                        }

                        UpdateHourWheel();
                    }
                    UpdateMinWheel();
                }

                UpdateSecWheel();

            }

            UpdateMsecWheel();
        }
    }

    long long TimeWheel::GetTimerTick(Timer* pTimer)
    {
        long long count = pTimer->day * 86400000;
        count += pTimer->hour * 3600000;
        count += pTimer->min * 60000;
        count += pTimer->sec * 1000;
        count = count * msec_wheel.size() / 1000;

        count += pTimer->msec;

        return count;
    }

    bool TimeWheel::AddTime(int t, int&temp, int point, int interval, bool bAdd)
    {
        temp = point + t;

        if (bAdd)
            temp++;

        if (temp >= interval)
        {
            temp -= interval;
            return true;
        }
        return false;
    }

    void TimeWheel::excuteTimer(Timer* pTimer)
    {
        if (pTimer->bRelease)
        {
            delete pTimer;
            return;
        }

        pTimer->cb_func();

        pTimer->count--;
        if (pTimer->count == 0)
        {
            delete pTimer;
            return;
        }

        int day = pTimer->interval / 86400000;
        int hour = (pTimer->interval / 3600000) % 24;
        int min = (pTimer->interval / 60000) % 60;
        int sec = (pTimer->interval / 1000) % 60;
        int msec = (pTimer->interval % 1000) * msec_wheel.size() / 1000;

        if (AddTime(hour, pTimer->hour, pTimer->hour, 24,
            AddTime(min, pTimer->min, pTimer->min, 60,
            AddTime(sec, pTimer->sec, pTimer->sec, 60,
            AddTime(msec, pTimer->msec, pTimer->msec + 1, msec_wheel.size(), false)))))
            day++;
        pTimer->day = day_point + day;

        if (GetTimerTick(pTimer) <= msec_count)
        {
            msec_wheel[msec_point].push_back(pTimer);
            return;
        }

        if (pTimer->day != day_point)
        {
            day_wheel[pTimer->day].push_back(pTimer);
            return;
        }
        if (pTimer->hour != hour_point)
        {
            hour_wheel[pTimer->hour].push_back(pTimer);
            return;
        }
        if (pTimer->min != min_point)
        {
            min_wheel[pTimer->min].push_back(pTimer);
            return;
        }
        if (pTimer->sec != sec_point)
        {
            sec_wheel[pTimer->sec].push_back(pTimer);
            return;
        }

        msec_wheel[pTimer->msec].push_back(pTimer);
    }

    void TimeWheel::UpdateMsecWheel()
    {
        std::list<Timer*>& msec_list = msec_wheel[msec_point];
        while (msec_list.size() != 0)
        {
            Timer* pTimer = msec_list.front();
            msec_list.pop_front();

            excuteTimer(pTimer);
        }
    }

    void TimeWheel::UpdateSecWheel()
    {
        std::list<Timer*>& tlist = sec_wheel[sec_point];
        while (tlist.size() != 0)
        {
            Timer* pTimer = tlist.front();
            tlist.pop_front();

            if (pTimer->msec != 0)
            {
                msec_wheel[pTimer->msec].push_back(pTimer);
                continue;
            }

            excuteTimer(pTimer);
        }
    }

    void TimeWheel::UpdateMinWheel()
    {
        std::list<Timer*>& tlist = min_wheel[min_point];
        while (tlist.size() != 0)
        {
            Timer* pTimer = tlist.front();
            tlist.pop_front();

            if (pTimer->sec != 0)
            {
                sec_wheel[pTimer->sec].push_back(pTimer);
                continue;
            }
            if (pTimer->msec != 0)
            {
                msec_wheel[pTimer->msec].push_back(pTimer);
                continue;
            }

            excuteTimer(pTimer);
        }
    }

    void TimeWheel::UpdateHourWheel()
    {
        std::list<Timer*>& tlist = hour_wheel[hour_point];
        while (tlist.size() != 0)
        {
            Timer* pTimer = tlist.front();
            tlist.pop_front();

            if (pTimer->min != 0)
            {
                min_wheel[pTimer->min].push_back(pTimer);
                continue;
            }
            if (pTimer->sec != 0)
            {
                sec_wheel[pTimer->sec].push_back(pTimer);
                continue;
            }
            if (pTimer->msec != 0)
            {
                msec_wheel[pTimer->msec].push_back(pTimer);
                continue;
            }

            excuteTimer(pTimer);
        }
    }

    void TimeWheel::UpdateDayWheel()
    {
        std::list<Timer*>& tlist = day_wheel[day_point];
        while (tlist.size() != 0)
        {
            Timer* pTimer = tlist.front();
            tlist.pop_front();

            if (pTimer->hour != 0)
            {
                hour_wheel[pTimer->hour].push_back(pTimer);
                continue;
            }
            if (pTimer->min != 0)
            {
                min_wheel[pTimer->min].push_back(pTimer);
                continue;
            }
            if (pTimer->sec != 0)
            {
                sec_wheel[pTimer->sec].push_back(pTimer);
                continue;
            }
            if (pTimer->msec != 0)
            {
                msec_wheel[pTimer->msec].push_back(pTimer);
                continue;
            }

            excuteTimer(pTimer);
        }
    }
}