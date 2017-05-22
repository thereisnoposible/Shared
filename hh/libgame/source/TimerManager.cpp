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
	//����
	IntervalTimer* pTimer = new IntervalTimer(fInterval,callBack);

	CHECKERROR(_minHeap.Insert(pTimer));

	return pTimer;
}


//-----------------------------------------------------------------------------------------------
TMHANDLE TimerManager::AddTriggerTimer(int32 hour, int32 min, int32 sec, Timer::handle callBack)
{
	//����
	TriggerTimer* pTimer = new TriggerTimer(hour,min,sec,callBack);

    CHECKERROR(_minHeap.Insert(pTimer));

	return pTimer;
}

//------------------------------------------------------------------------------------------------------
void  TimerManager::RemoveTimer(TMHANDLE hTimer)
{
	CHECKERRORANDRETURN(hTimer!=NULL);

	//���ö�ʱ��Ϊɾ��״̬�����´�������������ö�ʱ��
	hTimer->release();
}


//------------------------------------------------------------------------------------------------------
void  TimerManager::Update(float fInterval)
{
	double fCurrTime = Helper::GetFrequencyTime();

	fInterval = (float)(fCurrTime - _tmReferTime);

	Timer* pTimer = NULL;
	while (_minHeap.FindMin(pTimer))
	{//�ҵ�һ����ʱ��
		CHECKERRORANDRETURN(pTimer != NULL);

		if (pTimer->isRelease())
		{
			_minHeap.RemoveMin();
			delete pTimer;
			pTimer = NULL;

			continue;
		}
		if (pTimer->update(fInterval))
		{//�Ѿ�������������������¼���
			_minHeap.RemoveMin();

			//���¼ӽ���С����
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