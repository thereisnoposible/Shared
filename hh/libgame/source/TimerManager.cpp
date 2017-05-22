#include "pch.h"
#include "TimerManager.h"


template<> TimerManager* Singleton<TimerManager>::s_pInstance=NULL;

//-----------------------------------------------------------------------------------------------
TimerManager::TimerManager()
{
	//m_tmLastTrigger = time(NULL);
	_tmReferTime = Helper::GetFrequencyTime();
	_tmReferTimesnap = time(NULL);
}


//-----------------------------------------------------------------------------------------------
TimerManager::~TimerManager()
{
}


//-----------------------------------------------------------------------------------------------
TMHANDLE TimerManager::AddIntervalTimer(float fInterval,Timer::handle callBack)
{
	//创建
	IntervalTimer* pTimer = new IntervalTimer(fInterval,callBack);

	CHECKERROR(_minHeap.Insert(pTimer));

	return pTimer;
}


//-----------------------------------------------------------------------------------------------
TMHANDLE TimerManager::AddTriggerTimer(int32 hour, int32 min, int32 sec, Timer::handle callBack)
{
	//创建
	TriggerTimer* pTimer = new TriggerTimer(hour,min,sec,callBack);

    CHECKERROR(_minHeap.Insert(pTimer));

	return pTimer;
}

//------------------------------------------------------------------------------------------------------
void  TimerManager::RemoveTimer(TMHANDLE hTimer)
{
	CHECKERRORANDRETURN(hTimer!=NULL);

	//设置定时器为删除状态，在下次遇到则清除掉该定时器
	hTimer->release();
}


//------------------------------------------------------------------------------------------------------
void  TimerManager::Update(float fInterval)
{
	double fCurrTime = Helper::GetFrequencyTime();

	fInterval = (float)(fCurrTime - _tmReferTime);

	Timer* pTimer = NULL;
	while (_minHeap.FindMin(pTimer))
	{//找到一个定时器
		CHECKERRORANDRETURN(pTimer != NULL);

		if (pTimer->isRelease())
		{
			_minHeap.RemoveMin();
			delete pTimer;
			pTimer = NULL;

			continue;
		}
		if (pTimer->update(fInterval))
		{//已经触发，清除掉，并重新加入
			_minHeap.RemoveMin();

			//重新加进最小堆中
			_minHeap.Insert(pTimer);
		}
		else
		{
			break;
		}
	}
}

//------------------------------------------------------------------------------------------------------
int32 TimerManager::GetCount()
{
    return _minHeap.GetCurrSize();
}