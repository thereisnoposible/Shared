/****************************************************************************
**
** Copyright (C) 2014, All right reserved.
**
** @file		MESSAGEHANDLE.H
** @brief		消息处理实现
**
** Details
**
** @version	1.0
** @date		2014/7/23 9:28
**
** 修订说明：	最初版本
**
****************************************************************************/

#pragma once



/**
* @brief		消息处理类模板
* @details		模板类，实例化参数为消息参数
* @date			2014/7/23 9:28
* @note			无
*/
template<typename T> class MessageHandle
{
public:
	MessageHandle(void) {};
	~MessageHandle(void) {};

	typedef boost::function< void(T&) > handle;		//处理函数类型

public:
	/**
	* @brief		注册消息
	* @param[in]	messageid[int32] : 被注册的消息ID
	* @param[in]	fun[handle] : 处理函数
	* @return		void
	* @remarks
	*/
	void RegisterMessage(int32 messageid, handle fun)
	{
		__typename MessageHandleMap::iterator it = m_MsgHandleMap.find(messageid);
		if( it == m_MsgHandleMap.end() )
		{
			HandleVector lstHandle;
			m_MsgHandleMap.insert(std::make_pair(messageid,lstHandle));

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
	void UnregisterMessage(int32 messageid,handle fun)
	{//由于boost未实现function的比较，故无法实现
		__typename MessageHandleMap::iterator it = m_MsgHandleMap.find(messageid);
		if(it != m_MsgHandleMap.end())
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
	void RegisterMessageRange(int32 smessage,int32 emessage,handle fun)
	{
		//遍历查找是否已注册
		MessageRangeHandle rangeHandle(smessage,emessage,fun);
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
	void UnregisterMessageRange(int32 smessage,int32 emessage,handle fun)
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
	bool FireMessage(int32 messageid, T& t)
	{
		bool bFired = false;
		__typename MessageHandleMap::iterator itMsg = m_MsgHandleMap.find(messageid);
		if( itMsg != m_MsgHandleMap.end())
		{//遍历所有的处理函数
			HandleVector& lstHandle = itMsg->second;
			__typename HandleVector::iterator it=lstHandle.begin();
			__typename HandleVector::iterator itEnd = lstHandle.end();
			for(; it!=itEnd; it++)
			{
				(*it)(t);
			}

			bFired = true;
		}

		//遍历区间
		__typename MessageRangeCollect::iterator itRg = m_MsgRangeCollect.begin();
		__typename MessageRangeCollect::iterator itRgEnd = m_MsgRangeCollect.end();
		for(; itRg!=itRgEnd; itRg++)
		{
			if((*itRg).IsInRange(messageid))
			{
				bFired = true;

				(*itRg).fun(t);
			}
		}

		return bFired;
	}

	bool hasRegisterMessage(int32 id)
	{
		//return m_MsgHandleMap.size() == 0 ? false : true;
		MessageHandleMap::iterator it = m_MsgHandleMap.find(id);
		if (it == m_MsgHandleMap.end())
		{
			return false;
		}
		if (it->second.size() == 0)
		{
			return false;
		}
		return true;
	}

protected:
	typedef xvector<handle> HandleVector;					//处理函数队列
	typedef xmap<int32,HandleVector> MessageHandleMap;		//key=消息ID,HandleVector=处理函数队列

	typedef struct tagMessageRangeHandle
	{
		tagMessageRangeHandle(int32 p_smessage,int32 p_emessage,handle p_fun)
		{
			smessage = p_smessage;
			emessage = p_emessage;
			fun = p_fun;
		}


		bool IsInRange(int32 messageid)
		{
			if(messageid>=smessage && messageid<=emessage)
				return true;

			return false;
		}

		int32 smessage;			//开始消息
		int32 emessage;			//结束消息
		handle fun;				//处理函数
	}MessageRangeHandle;

	typedef xvector<MessageRangeHandle> MessageRangeCollect;

	MessageRangeCollect m_MsgRangeCollect;				/** 区域消息处理队列 */

	MessageHandleMap m_MsgHandleMap;						/** 消息函数map */
};

