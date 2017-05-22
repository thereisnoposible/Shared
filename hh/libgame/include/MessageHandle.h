/****************************************************************************
**
** Copyright (C) 2014, All right reserved.
**
** @file		MESSAGEHANDLE.H
** @brief		��Ϣ����ʵ��
**
** Details
**
** @version	1.0
** @date		2014/7/23 9:28
**
** �޶�˵����	����汾
**
****************************************************************************/

#pragma once



/**
* @brief		��Ϣ������ģ��
* @details		ģ���࣬ʵ��������Ϊ��Ϣ����
* @date			2014/7/23 9:28
* @note			��
*/
template<typename T> class MessageHandle
{
public:
	MessageHandle(void) {};
	~MessageHandle(void) {};

	typedef boost::function< void(T&) > handle;		//����������

public:
	/**
	* @brief		ע����Ϣ
	* @param[in]	messageid[int32] : ��ע�����ϢID
	* @param[in]	fun[handle] : ������
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
	* @brief		ע����Ϣ
	* @param[in]	messageid[int32] : ��ע�����ϢID
	* @param[in]	fun[handle] : ������
	* @return		void
	* @remarks
	*/
	void UnregisterMessage(int32 messageid,handle fun)
	{//����boostδʵ��function�ıȽϣ����޷�ʵ��
		__typename MessageHandleMap::iterator it = m_MsgHandleMap.find(messageid);
		if(it != m_MsgHandleMap.end())
		{
			m_MsgHandleMap.erase(it);
		}
	}


	/**
	* @brief		ע����Ϣ��Χ
	* @param[in]	smessage[int32] : ��ʼ��ϢID
	* @param[in]	emessage[int32] : ������ϢID
	* @param[in]	fun[handle] : ������
	* @return		void
	* @remarks
	*/
	void RegisterMessageRange(int32 smessage,int32 emessage,handle fun)
	{
		//���������Ƿ���ע��
		MessageRangeHandle rangeHandle(smessage,emessage,fun);
		m_MsgRangeCollect.push_back(rangeHandle);
	}


	/**
	* @brief		ע����Ϣ��Χ
	* @param[in]	smessage[int32] : ��ʼ��ϢID
	* @param[in]	emessage[int32] : ������ϢID
	* @param[in]	fun[handle] : ������
	* @return		void
	* @remarks
	*/
	void UnregisterMessageRange(int32 smessage,int32 emessage,handle fun)
	{//����boostδʵ��function�ıȽϣ����޷�ʵ��

	}


	/**
	* @brief		������Ϣ
	* @details
	* @param[in]	messageid[int32] : ��ϢID
	* @param[in]	t[T] : ��Ϣ����
	* @return		bool : true=�Ѵ��� false=δ����
	* @remarks
	*/
	bool FireMessage(int32 messageid, T& t)
	{
		bool bFired = false;
		__typename MessageHandleMap::iterator itMsg = m_MsgHandleMap.find(messageid);
		if( itMsg != m_MsgHandleMap.end())
		{//�������еĴ�����
			HandleVector& lstHandle = itMsg->second;
			__typename HandleVector::iterator it=lstHandle.begin();
			__typename HandleVector::iterator itEnd = lstHandle.end();
			for(; it!=itEnd; it++)
			{
				(*it)(t);
			}

			bFired = true;
		}

		//��������
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
	typedef xvector<handle> HandleVector;					//����������
	typedef xmap<int32,HandleVector> MessageHandleMap;		//key=��ϢID,HandleVector=����������

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

		int32 smessage;			//��ʼ��Ϣ
		int32 emessage;			//������Ϣ
		handle fun;				//������
	}MessageRangeHandle;

	typedef xvector<MessageRangeHandle> MessageRangeCollect;

	MessageRangeCollect m_MsgRangeCollect;				/** ������Ϣ������� */

	MessageHandleMap m_MsgHandleMap;						/** ��Ϣ����map */
};

