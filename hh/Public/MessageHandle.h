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

	typedef std::tr1::function< void(T&) > handle;		//����������

public:
	/**
	* @brief		ע����Ϣ
	* @param[in]	messageid[int32] : ��ע�����ϢID
	* @param[in]	fun[handle] : ������
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
	* @brief		ע����Ϣ
	* @param[in]	messageid[int32] : ��ע�����ϢID
	* @param[in]	fun[handle] : ������
	* @return		void
	* @remarks
	*/
	void UnregisterMessage(int messageid, handle fun)
	{//����boostδʵ��function�ıȽϣ����޷�ʵ��
		MessageHandleMap::iterator it = m_MsgHandleMap.find(messageid);
		if (it != m_MsgHandleMap.end())
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
	void RegisterMessageRange(int smessage, int emessage, handle fun)
	{
		//���������Ƿ���ע��
		MessageRangeHandle rangeHandle(smessage, emessage, fun);
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
	void UnregisterMessageRange(int smessage, int emessage, handle fun)
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
	bool FireMessage(int messageid, T& t)
	{
		bool bFired = false;
		MessageHandleMap::iterator itMsg = m_MsgHandleMap.find(messageid);
		if (itMsg != m_MsgHandleMap.end())
		{//�������еĴ�����
			HandleVector& lstHandle = itMsg->second;
			HandleVector::iterator it = lstHandle.begin();
			HandleVector::iterator itEnd = lstHandle.end();
			for (; it != itEnd; it++)
			{
				(*it)(t);
			}

			bFired = true;
		}

		//��������
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
	typedef vector<handle> HandleVector;					//����������
	typedef hash_map<int, HandleVector> MessageHandleMap;		//key=��ϢID,HandleVector=����������

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

		int smessage;			//��ʼ��Ϣ
		int emessage;			//������Ϣ
		handle fun;				//������
	}MessageRangeHandle;

	typedef vector<MessageRangeHandle> MessageRangeCollect;

	MessageRangeCollect m_MsgRangeCollect;				/** ������Ϣ������� */

	MessageHandleMap m_MsgHandleMap;						/** ��Ϣ����map */
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