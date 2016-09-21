#pragma once
#include "Timer.h"
#include <hash_map>
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

	}
	~TimerManager()
	{
		for (int i = 0; i < (int)Timers.size(); i++)
		{
			delete Timers[i];
			Timers[i] = nullptr;
		}
	}

	void Update(double diff)
	{
		_interval += diff;
		while (!Timers.empty() && (Timers.front()->CouldUpdate(_interval) || Timers.front()->IsRelease()))
		{
			TMHANDLE pTimer = Timers.front();
			std::pop_heap(Timers.begin(), Timers.end(), Comparator());
			Timers.pop_back();
			if (!pTimer->IsRelease())
			{
				pTimer->update(_interval);
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
		TMHANDLE pTimer = new IntervalTimer(Interval, _func);
		Timers.push_back(pTimer);
		return pTimer;
	}
	TMHANDLE AddTriggerTimer(int _hour, int _minute, int _second, std::tr1::function<void(void)> _func)
	{
		TMHANDLE pTimer = new TriggerTimer(_hour, _minute, _second, _func);
		Timers.push_back(pTimer);
		return pTimer;
	}

	void RemoveTimer(TMHANDLE tHandle)
	{
		if (tHandle != NULL)
			tHandle->release();
	}

private:
	std::vector<TMHANDLE> Timers;
	double _interval;
};