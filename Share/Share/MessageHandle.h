#pragma once
#include <vector>
#include <hash_map>
#include <functional>
using std::vector;
using stdext::hash_map;

template<typename T>
class MessageHandle
{
public:
	MessageHandle(void) {};
	~MessageHandle(void) {};

	typedef std::tr1::function< void(T&) > handle;		//处理函数类型

public:
	/**
	* @brief		注册消息
	* @param[in]	messageid[int32] : 被注册的消息ID
	* @param[in]	fun[handle] : 处理函数
	* @return		void
	* @remarks
	*/
	void RegisterMessage(int messageid, handle fun)
	{
		MessageHandleMap::iterator it = m_MsgHandleMap.find(messageid);
		if (it == m_MsgHandleMap.end())
		{
			HandleVector lstHandle;
			m_MsgHandleMap.insert(std::make_pair(messageid, lstHandle));

			it = m_MsgHandleMap.find(messageid);
		}

		HandleVector& lstHandle = it->second;
		lstHandle.push_back(fun);
	}


	/**
	* @brief		注销消息
	* @param[in]	messageid[int32] : 被注册的消息ID
	* @param[in]	fun[handle] : 处理函数
	* @return		void
	* @remarks
	*/
	void UnregisterMessage(int messageid, handle fun)
	{//由于boost未实现function的比较，故无法实现
		MessageHandleMap::iterator it = m_MsgHandleMap.find(messageid);
		if (it != m_MsgHandleMap.end())
		{
			m_MsgHandleMap.erase(it);
		}
	}


	/**
	* @brief		注册消息范围
	* @param[in]	smessage[int32] : 起始消息ID
	* @param[in]	emessage[int32] : 结束消息ID
	* @param[in]	fun[handle] : 处理函数
	* @return		void
	* @remarks
	*/
	void RegisterMessageRange(int smessage, int emessage, handle fun)
	{
		//遍历查找是否已注册
		MessageRangeHandle rangeHandle(smessage, emessage, fun);
		m_MsgRangeCollect.push_back(rangeHandle);
	}


	/**
	* @brief		注销消息范围
	* @param[in]	smessage[int32] : 起始消息ID
	* @param[in]	emessage[int32] : 结束消息ID
	* @param[in]	fun[handle] : 处理函数
	* @return		void
	* @remarks
	*/
	void UnregisterMessageRange(int smessage, int emessage, handle fun)
	{//由于boost未实现function的比较，故无法实现

	}


	/**
	* @brief		触发消息
	* @details
	* @param[in]	messageid[int32] : 消息ID
	* @param[in]	t[T] : 消息参数
	* @return		bool : true=已处理 false=未处理
	* @remarks
	*/
	bool FireMessage(int messageid, T& t)
	{
		bool bFired = false;
		MessageHandleMap::iterator itMsg = m_MsgHandleMap.find(messageid);
		if (itMsg != m_MsgHandleMap.end())
		{//遍历所有的处理函数
			HandleVector& lstHandle = itMsg->second;
			HandleVector::iterator it = lstHandle.begin();
			HandleVector::iterator itEnd = lstHandle.end();
			for (; it != itEnd; it++)
			{
				(*it)(t);
			}

			bFired = true;
		}

		//遍历区间
		MessageRangeCollect::iterator itRg = m_MsgRangeCollect.begin();
		MessageRangeCollect::iterator itRgEnd = m_MsgRangeCollect.end();
		for (; itRg != itRgEnd; itRg++)
		{
			if ((*itRg).IsInRange(messageid))
			{
				bFired = true;

				(*itRg).fun(t);
			}
		}

		return bFired;
	}

protected:
	typedef vector<handle> HandleVector;					//处理函数队列
	typedef hash_map<int, HandleVector> MessageHandleMap;		//key=消息ID,HandleVector=处理函数队列

	typedef struct tagMessageRangeHandle
	{
		tagMessageRangeHandle(int p_smessage, int p_emessage, handle p_fun)
		{
			smessage = p_smessage;
			emessage = p_emessage;
			fun = p_fun;
		}


		bool IsInRange(int messageid)
		{
			if (messageid >= smessage && messageid <= emessage)
				return true;

			return false;
		}

		int smessage;			//开始消息
		int emessage;			//结束消息
		handle fun;				//处理函数
	}MessageRangeHandle;

	typedef vector<MessageRangeHandle> MessageRangeCollect;

	MessageRangeCollect m_MsgRangeCollect;				/** 区域消息处理队列 */

	MessageHandleMap m_MsgHandleMap;						/** 消息函数map */
};

class BaseEvent
{
public:
	BaseEvent(int evt_id) :m_evtid(evt_id){}
	virtual ~BaseEvent(void){}

public:
	const int GetEvtID() const { return m_evtid; }

private:
	int m_evtid;
};



template<typename T>
class TEvent : public BaseEvent
{
public:
	TEvent(int evt_id, const T& data) : BaseEvent(evt_id), m_data(data){}
	virtual ~TEvent(void){}

	const T& GetData() const { return m_data; }

private:
	T m_data;
};


typedef MessageHandle<const BaseEvent&> EventHandle;