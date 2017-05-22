/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		TimerManager.h
** @brief		定时器管理
**  
** Details	
**  
** @version	1.0
** @author	
** @date	2014/7/10 21:36 
**  
** 修订说明：	最初版本 
** 
****************************************************************************/



#pragma once

#include "Timer.h"
#include "Heap.h"

typedef Timer* TMHANDLE;


/**
* @brief		定时器管理类
* @details			
* @author		余祎
* @date			2014/7/10 21:36
* @note			无 
*/ 
class XClass TimerManager : public Singleton<TimerManager>
{
public:
	TimerManager();
	virtual ~TimerManager();


public:
	/**
	* @brief		增加一个定时器
	* @param[in]	fInterval : [float] 时间间隔
	* @param[in]	ontime : [Timer::handle] 回调函数
	* @remarks		
	*/
	TMHANDLE AddIntervalTimer(float fInterval,Timer::handle callBack);


	/**
	* @brief		增加一个触发器
	* @param[in]	hour : [int32] 小时
	* @param[in]	min : [int32] 分钟
	* @param[in]	sec : [int32] 秒
	* @param[in]	ontime : [Timer::handle] 回调函数
	* @remarks		
	*/
	TMHANDLE AddTriggerTimer(int32 hour, int32 min, int32 sec, Timer::handle callBack);


	/**
	* \fn				removeTimer
	* @brief		移除一个定时器
	* @param[in]	void* : 定时器对象指针
	* @return		  void
	*/
	void  RemoveTimer(TMHANDLE hTimer);


	/**
	* \fn				Update
	* @brief		增加定时器的经过时间
	* @param[in]	float : 经过的时长
	* @return		  void
	*/
	void  Update(float fInterval);

	double GetReferTime(){ return _tmReferTime; }

	time_t GetReferTimesnap() { return _tmReferTimesnap; }

    int32 GetCount();

protected:
	double _tmReferTime;					/**< 参照时间 */
	time_t _tmReferTimesnap;					/**< 参照时间戳 */

	MinHeap _minHeap;
};
