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
		return a->time(0) > b->time(0);
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
		while (!Timers.empty() && (Timers.back()->CouldUpdate(_interval) || Timers.back()->IsRelease()))
		{
			TMHANDLE pTimer = Timers.back();
			//std::pop_heap(Timers.begin(), Timers.end(), Comparator());
			Timers.pop_back();
			if (!pTimer->IsRelease())
			{
				pTimer->update(_interval);
			}

			if (!pTimer->IsRelease())
			{
				Timers.push_back(pTimer);
				min_heap();
				//std::push_heap(Timers.begin(), Timers.end(), Comparator());
			}
			else
			{
				delete pTimer;
			}
		}
	}

	TMHANDLE AddIntervalTimer(double Interval, std::tr1::function<void(void)> _func)
	{
		TMHANDLE pTimer = new IntervalTimer(Interval, _func, _interval);
		Timers.push_back(pTimer);
		min_heap();
		//std::push_heap(Timers.begin(), Timers.end(), Comparator());
		return pTimer;
	}
	TMHANDLE AddTriggerTimer(int _hour, int _minute, int _second, std::tr1::function<void(void)> _func)
	{
		TMHANDLE pTimer = new TriggerTimer(_hour, _minute, _second, _func, _interval);
		Timers.push_back(pTimer);
		//std::push_heap(Timers.begin(), Timers.end(), Comparator());
		min_heap();
		return pTimer;
	}

	void RemoveTimer(TMHANDLE tHandle)
	{
		if (tHandle != NULL)
			tHandle->release();
	}

private:
	void min_heap()
	{
		if (Timers.size() < 2)
			return;

		int min_pos = 0;
		int end = Timers.size() - 1;

		for (int i = 1; i < Timers.size(); i++)
		{
			if (Timers[min_pos]->time(_interval) > Timers[i]->time(_interval))
			{
				min_pos = i;
			}
		}

		if (min_pos != end)
		{
			TMHANDLE temp = Timers[min_pos];
			Timers[min_pos] = Timers[end];
			Timers[end] = temp;
		}
	}

private:
	std::vector<TMHANDLE> Timers;
	double _interval;
};