/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		Timer.h
** @brief	定时器及触发器的实现
**  
** Details	
**  
** @version	1.0
** @author	
** @date	2014/7/10 15:03 
**  
** 修订说明：	最初版本 
** 
****************************************************************************/

#pragma  once

#include <time.h>
#include "Singleton.h"


class TimerManager;


/**
* @brief			定时器
* @details			间隔定时器， 触发定时器，用于定义统一的handle
* @author			余祎 
* @date				2014/7/10 21:18
* @note				无 
*/ 
class Timer
{
	friend class TimerManager;

public:
	//回调函数类型
	typedef boost::function<void () > handle;

	double getRelativeTime() { return _relativeTime; }

	void release(){ _bRelease = true; }

	bool isRelease(){ return _bRelease; }

	/**
	* \fn			~Timer()
	* @brief		析构函数
	*/
	virtual ~Timer();

protected:
	/**
	* \fn			Timer()
	* @brief		构造函数
	* @param[in]	enType[TimerType] : 定时器类型
	* @param[in]	callBack[handle] : 回调函数
	*/
	Timer(handle callBack);

protected:
	/**
	* \fn			update()
	* @brief		定时器更新函数
	* @param[in]	fInterval[float] : 时间间隔
	* @return		bool : true=触发 false=未触发
	* @remarks		
	*/
	bool update(float fInterval);

	void setRelativeTime(double t);

	

	/**
	* \fn			reset
	* @brief		重置定时器
	* @return		void
	* @remarks
	*/
	virtual void reset() = 0;


	/**
	* \fn			onTimer()
	* @brief		触发定时器
	* @return		void
	* @remarks		
	*/
	void onTimer();

protected:
	double _relativeTime;			/** 相对时间 */

private:
	handle m_callBack;			/** 回调函数 */ 

	bool _bRelease;
	
};


typedef std::vector<Timer*> TimerPtrVector;
typedef std::set<Timer*>	TimerPtrSet;


/**
* @brief		间隔定时器
* @details		以固定时间间隔不断调用
* @author		
* @date			2014/7/10 15:12
* @note			无 
*/ 
class  IntervalTimer : public Timer
{
	friend class TimerManager;

//构造与析构函数全部保护，避免外部操作
protected:
	/**
	* \fn				IntervalTimer
	* @brief		构造函数
	* @details		指定时间间隔调用
	* @param[in]	fInterval[float] : 间隔时间
	* @remarks		
	*/
	IntervalTimer( float fInterval, handle callBack);


	/**
	* \fn				~IntervalTimer
	* @brief		析构函数
	* @remarks		
	*/
	virtual ~IntervalTimer();


	/**
	* \fn			reset
	*/
	virtual void reset();


private:
	float		m_fInterval;					/** 间隔时间 */
};


//////////////////////////////////////////////////////////////////////////

/**
* @brief		触发定时器
* @details		按时分秒触发
* @author		余祎 
* @date			2014/7/10 15:51
* @note			无 
*/
class TriggerTimer : public Timer
{
	friend class TimerManager;

protected:
	/**
	* \fn				TriggerTimer()
	* @brief		构造函数
	* @param[in]	
	* @remarks		
	*/
	TriggerTimer(int32 hour, int32 min, int32 sec,handle callBack);

	/**
	* \fn				~TriggerTimer()
	* @brief		析构函数
	* @remarks		
	*/
	virtual ~TriggerTimer();


	virtual void reset();

private:
	/**
	* @brief		根据时间获取当天秒数
	* @details		hour*60*60 + minute*60 + second
	* @param[in]	t[time_t] : 时间
	* @return		int32 : 从零点开始所经历的秒数
	* @remarks		
	*/
	int32 getSeconds(time_t t);
};
