#include "pch.h"
#include "helper.h"
#include "Timer.h"


#define SECONDS_FROM_ZERO(hour,min,sec) (hour*60*60+min*60+sec)


//----------------------------------------------------------------------
Timer::Timer(handle callBack)
: m_callBack(callBack), _bRelease(false)
{

}


//----------------------------------------------------------------------
Timer::~Timer()
{

}

//----------------------------------------------------------------------
void Timer::onTimer()
{
	if(!m_callBack.empty())
		m_callBack();
}

//----------------------------------------------------------------------
void Timer::setRelativeTime(double t)
{
	_relativeTime = t;
}

//----------------------------------------------------------------------
bool Timer::update(float fInterval)
{
	if (fInterval >= _relativeTime)
	{//间隔时间大于定时器设定的相对时间
		onTimer();

		//重新设定相对时间
		reset();

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// IntervalTimer : 间隔定时器
//////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
IntervalTimer::IntervalTimer( float fInterval,handle callBack)
	: Timer(callBack),m_fInterval(fInterval)
{
	double fCurrTime = Helper::GetFrequencyTime();
	double fReferTime = TimerManager::GetSingleton().GetReferTime();

	//当前时间与参照时间的差值，加上定时间隔，就是下一次触发的相对时间
	_relativeTime = fCurrTime - fReferTime + fInterval;

	setRelativeTime(_relativeTime);
}


//----------------------------------------------------------------------
IntervalTimer::~IntervalTimer()
{
	
}


//----------------------------------------------------------------------
void IntervalTimer::reset()
{
	//当前触发时间，再加上一个时间间隔，就是下一次触发的相对时间
	_relativeTime += m_fInterval;
}


//////////////////////////////////////////////////////////////////////////
// Trigger : 触发器
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------
TriggerTimer::TriggerTimer(int32 hour, int32 min, int32 sec,handle callBack)
	: Timer(callBack)
{
	time_t referTimeSnap = TimerManager::GetSingleton().GetReferTimesnap();
	time_t curTimer = time(NULL);

	//距离参考时间的时间间隔，单位： 秒
	int32 referInterval = curTimer - referTimeSnap;


	int32 triggerInDay = SECONDS_FROM_ZERO(hour, min, sec);
	int32 curInDay = getSeconds(curTimer);
	if (triggerInDay > curInDay)
	{
		//触发时间=触发时间-当前时间 + （当前时间 - 参考时间）
		//意义为： 触发时间到当前时间的时间间隔 + 当前时间到参考时间的间隔
		//计算结果就是下一次触发离参考时间的秒数！
		_relativeTime = referInterval + (triggerInDay - curInDay);
	}
	else
	{
		_relativeTime = referInterval + (SECONDS_FROM_ZERO(24, 00, 00) - curInDay + triggerInDay);
	}
}



//-----------------------------------------------------------------------------------------------
TriggerTimer::~TriggerTimer()
{

}


//-----------------------------------------------------------------------------------------------
void TriggerTimer::reset()
{
	_relativeTime += 24 * 60 * 60;
}


//-----------------------------------------------------------------------------------------------
int32 TriggerTimer::getSeconds(time_t t)
{
	int32 year,mon,day,hour,min,sec;
	Helper::SplitTimet(t,year,mon,day,hour,min,sec);

	int32 seconds = SECONDS_FROM_ZERO(hour,min,sec);

	return seconds;
}
