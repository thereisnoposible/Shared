/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		EVENT.H
** @brief		事件实现
**  
** Details	
**  
** @version	1.0
** @author		yuyi
** @date		2014/8/20 9:30
**  
** 修订说明：	最初版本 
** 
****************************************************************************/

#pragma once


class BaseEvent
{
public:
	BaseEvent(int32 evt_id):m_evtid(evt_id){}
	virtual ~BaseEvent(void){}

public:
	const int32 GetEvtID() const {return m_evtid;}

private:
	int32 m_evtid;
};



template<typename T>
class TEvent : public BaseEvent
{
public:
	TEvent(int32 evt_id,const T& data) : BaseEvent(evt_id),m_data(data){}
	virtual ~TEvent(void){}

	const T& GetData() const {return m_data;}

private:
	T m_data;
};


typedef MessageHandle<const BaseEvent&> EventHandle;
