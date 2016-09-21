#pragma once
#include <functional>
#include <time.h>
#include <algorithm>

class Timer
{
public:
	Timer() : bRelease(false){}
	virtual ~Timer(){}
	virtual void update(double diff)
	{
		if (diff - _current > _interval)
		{
			Reset();
			func();
		}
	}

	int time() const
	{
		return (int)(_interval - _current);
	}

	bool CouldUpdate(double diff)
	{
		return diff - _current > _interval;
	}

	void release(){ bRelease = true; }

	bool IsRelease(){ return bRelease; }

	double _interval; /**< TODO */
	double _current; /**< TODO */
	std::tr1::function<void(void)> func;
private:
	inline bool Passed() const { return _current >= _interval; }
	inline void Reset()
	{
		_current += _interval;
	}

	bool bRelease;
};

class IntervalTimer : public Timer
{
public:

	IntervalTimer(double Interval, std::tr1::function<void(void)>& _func)
	{
		_current = 0;
		_interval = Interval;
		func = _func;
	}

	//void Update(double diff)
	//{
	//	Timer::update(diff);
	//}
};

class TriggerTimer : public Timer
{
public:
	TriggerTimer(int _hour, int _minute, int _second, std::tr1::function<void(void)>& _func)
	{
		func = _func;
		_interval = 24 * 60 * 60;
		time_t tNow = ::time(NULL);
		tm tmCurr;
		localtime_s(&tmCurr,&tNow);
		int count = tmCurr.tm_hour * 60 * 60;
		count += tmCurr.tm_min * 60;
		count += tmCurr.tm_isdst;
		count -= _hour * 60 * 60;
		count -= _minute * 60;
		count -= _second;
		if (count < 0)
			_current = _interval + count;
		else
			_current = _interval - count;
	}
	//void Update(double diff)
	//{
	//	Timer::update(diff);
	//}
};