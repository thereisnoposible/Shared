/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		TimerManager.h
** @brief		��ʱ������
**  
** Details	
**  
** @version	1.0
** @author	
** @date	2014/7/10 21:36 
**  
** �޶�˵����	����汾 
** 
****************************************************************************/



#pragma once

#include "Timer.h"
#include "Heap.h"

typedef Timer* TMHANDLE;


/**
* @brief		��ʱ��������
* @details			
* @author		��t
* @date			2014/7/10 21:36
* @note			�� 
*/ 
class XClass TimerManager : public Singleton<TimerManager>
{
public:
	TimerManager();
	virtual ~TimerManager();


public:
	/**
	* @brief		����һ����ʱ��
	* @param[in]	fInterval : [float] ʱ����
	* @param[in]	ontime : [Timer::handle] �ص�����
	* @remarks		
	*/
	TMHANDLE AddIntervalTimer(float fInterval,Timer::handle callBack);


	/**
	* @brief		����һ��������
	* @param[in]	hour : [int32] Сʱ
	* @param[in]	min : [int32] ����
	* @param[in]	sec : [int32] ��
	* @param[in]	ontime : [Timer::handle] �ص�����
	* @remarks		
	*/
	TMHANDLE AddTriggerTimer(int32 hour, int32 min, int32 sec, Timer::handle callBack);


	/**
	* \fn				removeTimer
	* @brief		�Ƴ�һ����ʱ��
	* @param[in]	void* : ��ʱ������ָ��
	* @return		  void
	*/
	void  RemoveTimer(TMHANDLE hTimer);


	/**
	* \fn				Update
	* @brief		���Ӷ�ʱ���ľ���ʱ��
	* @param[in]	float : ������ʱ��
	* @return		  void
	*/
	void  Update(float fInterval);

	double GetReferTime(){ return _tmReferTime; }

	time_t GetReferTimesnap() { return _tmReferTimesnap; }

    int32 GetCount();

protected:
	double _tmReferTime;					/**< ����ʱ�� */
	time_t _tmReferTimesnap;					/**< ����ʱ��� */

	MinHeap _minHeap;
};
