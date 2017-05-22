/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		Timer.h
** @brief	��ʱ������������ʵ��
**  
** Details	
**  
** @version	1.0
** @author	
** @date	2014/7/10 15:03 
**  
** �޶�˵����	����汾 
** 
****************************************************************************/

#pragma  once

#include <time.h>
#include "Singleton.h"


class TimerManager;


/**
* @brief			��ʱ��
* @details			�����ʱ���� ������ʱ�������ڶ���ͳһ��handle
* @author			��t 
* @date				2014/7/10 21:18
* @note				�� 
*/ 
class Timer
{
	friend class TimerManager;

public:
	//�ص���������
	typedef boost::function<void () > handle;

	double getRelativeTime() { return _relativeTime; }

	void release(){ _bRelease = true; }

	bool isRelease(){ return _bRelease; }

	/**
	* \fn			~Timer()
	* @brief		��������
	*/
	virtual ~Timer();

protected:
	/**
	* \fn			Timer()
	* @brief		���캯��
	* @param[in]	enType[TimerType] : ��ʱ������
	* @param[in]	callBack[handle] : �ص�����
	*/
	Timer(handle callBack);

protected:
	/**
	* \fn			update()
	* @brief		��ʱ�����º���
	* @param[in]	fInterval[float] : ʱ����
	* @return		bool : true=���� false=δ����
	* @remarks		
	*/
	bool update(float fInterval);

	void setRelativeTime(double t);

	

	/**
	* \fn			reset
	* @brief		���ö�ʱ��
	* @return		void
	* @remarks
	*/
	virtual void reset() = 0;


	/**
	* \fn			onTimer()
	* @brief		������ʱ��
	* @return		void
	* @remarks		
	*/
	void onTimer();

protected:
	double _relativeTime;			/** ���ʱ�� */

private:
	handle m_callBack;			/** �ص����� */ 

	bool _bRelease;
	
};


typedef std::vector<Timer*> TimerPtrVector;
typedef std::set<Timer*>	TimerPtrSet;


/**
* @brief		�����ʱ��
* @details		�Թ̶�ʱ�������ϵ���
* @author		
* @date			2014/7/10 15:12
* @note			�� 
*/ 
class  IntervalTimer : public Timer
{
	friend class TimerManager;

//��������������ȫ�������������ⲿ����
protected:
	/**
	* \fn				IntervalTimer
	* @brief		���캯��
	* @details		ָ��ʱ��������
	* @param[in]	fInterval[float] : ���ʱ��
	* @remarks		
	*/
	IntervalTimer( float fInterval, handle callBack);


	/**
	* \fn				~IntervalTimer
	* @brief		��������
	* @remarks		
	*/
	virtual ~IntervalTimer();


	/**
	* \fn			reset
	*/
	virtual void reset();


private:
	float		m_fInterval;					/** ���ʱ�� */
};


//////////////////////////////////////////////////////////////////////////

/**
* @brief		������ʱ��
* @details		��ʱ���봥��
* @author		��t 
* @date			2014/7/10 15:51
* @note			�� 
*/
class TriggerTimer : public Timer
{
	friend class TimerManager;

protected:
	/**
	* \fn				TriggerTimer()
	* @brief		���캯��
	* @param[in]	
	* @remarks		
	*/
	TriggerTimer(int32 hour, int32 min, int32 sec,handle callBack);

	/**
	* \fn				~TriggerTimer()
	* @brief		��������
	* @remarks		
	*/
	virtual ~TriggerTimer();


	virtual void reset();

private:
	/**
	* @brief		����ʱ���ȡ��������
	* @details		hour*60*60 + minute*60 + second
	* @param[in]	t[time_t] : ʱ��
	* @return		int32 : ����㿪ʼ������������
	* @remarks		
	*/
	int32 getSeconds(time_t t);
};
