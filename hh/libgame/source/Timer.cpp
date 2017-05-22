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
	{//���ʱ����ڶ�ʱ���趨�����ʱ��
		onTimer();

		//�����趨���ʱ��
		reset();

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// IntervalTimer : �����ʱ��
//////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
IntervalTimer::IntervalTimer( float fInterval,handle callBack)
	: Timer(callBack),m_fInterval(fInterval)
{
	double fCurrTime = Helper::GetFrequencyTime();
	double fReferTime = TimerManager::GetSingleton().GetReferTime();

	//��ǰʱ�������ʱ��Ĳ�ֵ�����϶�ʱ�����������һ�δ��������ʱ��
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
	//��ǰ����ʱ�䣬�ټ���һ��ʱ������������һ�δ��������ʱ��
	_relativeTime += m_fInterval;
}


//////////////////////////////////////////////////////////////////////////
// Trigger : ������
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------
TriggerTimer::TriggerTimer(int32 hour, int32 min, int32 sec,handle callBack)
	: Timer(callBack)
{
	time_t referTimeSnap = TimerManager::GetSingleton().GetReferTimesnap();
	time_t curTimer = time(NULL);

	//����ο�ʱ���ʱ��������λ�� ��
	int32 referInterval = curTimer - referTimeSnap;


	int32 triggerInDay = SECONDS_FROM_ZERO(hour, min, sec);
	int32 curInDay = getSeconds(curTimer);
	if (triggerInDay > curInDay)
	{
		//����ʱ��=����ʱ��-��ǰʱ�� + ����ǰʱ�� - �ο�ʱ�䣩
		//����Ϊ�� ����ʱ�䵽��ǰʱ���ʱ���� + ��ǰʱ�䵽�ο�ʱ��ļ��
		//������������һ�δ�����ο�ʱ���������
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
