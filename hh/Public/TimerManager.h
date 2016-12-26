#pragma once
#include "Timer.h"
#include <hash_map>
#include <chrono>
#include "Singleton.h"
typedef  Timer* TMHANDLE;

class Comparator
{
public:
	bool operator()(const Timer* a, const Timer* b)
	{
		return a->time() > b->time();
	}
};

class TimerManager : public Singleton<TimerManager>
{
public:
	TimerManager() : _interval(0)
	{
		_create_time = std::chrono::steady_clock::now();
		_update_time = _create_time;
	}
	~TimerManager()
	{
		for (int i = 0; i < (int)Timers.size(); i++)
		{
			delete Timers[i];
			Timers[i] = nullptr;
		}
	}

	void Update()
	{
		_update_time = std::chrono::steady_clock::now();
		std::chrono::duration<double> usetime = std::chrono::duration_cast<std::chrono::duration<double>>(_update_time - _create_time);
		_interval = usetime.count();
        while (!Timers.empty() && (Timers[0]->Passed(_interval) || Timers[0]->IsRelease()))
        {
            TMHANDLE pTimer = Timers[0];
            std::pop_heap(Timers.begin(), Timers.end(), Comparator());
            Timers.pop_back();
            if (!pTimer->IsRelease())
            {
                pTimer->update(_interval);
                pTimer->_loop_count -= 1;
            }

            if (pTimer->_loop_count == 0)
            {
                delete pTimer;
            }

			if (!pTimer->IsRelease())
			{
				Timers.push_back(pTimer);
				std::push_heap(Timers.begin(), Timers.end(), Comparator());
			}
			else
			{
				delete pTimer;
			}
		}
	}

    TMHANDLE AddIntervalTimer(double Interval, std::tr1::function<void(void)> _func)
	{
        TMHANDLE pTimer = new IntervalTimer(Interval, _func, -1, _interval);
		Timers.push_back(pTimer);
		std::push_heap(Timers.begin(), Timers.end(), Comparator());
		return pTimer;
	}

    void AddIntervalTimer(double Interval, std::tr1::function<void(void)> _func, long long loop_count)
    {
        TMHANDLE pTimer = new IntervalTimer(Interval, _func, loop_count, _interval);
        Timers.push_back(pTimer);
        std::push_heap(Timers.begin(), Timers.end(), Comparator());
    }

	TMHANDLE AddTriggerTimer(int _hour, int _minute, int _second, std::tr1::function<void(void)> _func)
	{
        TMHANDLE pTimer = new TriggerTimer(_hour, _minute, _second, _func, -1, _interval);
		Timers.push_back(pTimer);
		std::push_heap(Timers.begin(), Timers.end(), Comparator());
		return pTimer;
	}

    void AddTriggerTimer(int _hour, int _minute, int _second, std::tr1::function<void(void)> _func, long long loop_count)
    {
        TMHANDLE pTimer = new TriggerTimer(_hour, _minute, _second, _func, loop_count, _interval);
        Timers.push_back(pTimer);
        std::push_heap(Timers.begin(), Timers.end(), Comparator());
    }

	void RemoveTimer(TMHANDLE tHandle)
	{
		if (tHandle != NULL)
			tHandle->release();
	}

private:

private:
	std::vector<TMHANDLE> Timers;
	double _interval;

	std::chrono::steady_clock::time_point _create_time;
	std::chrono::steady_clock::time_point _update_time;
};