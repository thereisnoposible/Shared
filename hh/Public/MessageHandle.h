#pragma once
#include <vector>
#include <functional>
#include <unordered_map>
using std::vector;
using std::unordered_map;

template<typename T>
class MessageHandle
{
public:
    MessageHandle(void) :incre(0){};
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
	long long RegisterMessage(int messageid, handle fun)
	{
		MessageHandleMap::iterator it = m_MsgHandleMap.find(messageid);
		if (it == m_MsgHandleMap.end())
		{
            HandleMap lstHandle;
			m_MsgHandleMap.insert(std::make_pair(messageid, lstHandle));

			it = m_MsgHandleMap.find(messageid);
		}
        incre++;

        HandleMap& lstHandle = it->second;
        long long ret = messageid;
        ret = ret << 32;
        ret += incre;

        lstHandle[ret] = fun;
        return ret;
	}


	/**
	* @brief		ע����Ϣ
	* @param[in]	messageid[int32] : ��ע�����ϢID
	* @param[in]	fun[handle] : ������
	* @return		void
	* @remarks
	*/
	void UnregisterMessage(long long id, handle fun)
	{//����boostδʵ��function�ıȽϣ����޷�ʵ��
        int messageid = id >> 32;
		MessageHandleMap::iterator it = m_MsgHandleMap.find(messageid);

		if (it != m_MsgHandleMap.end())
		{
            it->second.erase(id);
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
    long long RegisterMessageRange(int smessage, int emessage, handle fun)
	{
		//���������Ƿ���ע��
        incre++;

		MessageRangeHandle rangeHandle(smessage, emessage, fun);
        m_MsgRangeCollect[incre] = rangeHandle;

        return incre;
	}


	/**
	* @brief		ע����Ϣ��Χ
	* @param[in]	smessage[int32] : ��ʼ��ϢID
	* @param[in]	emessage[int32] : ������ϢID
	* @param[in]	fun[handle] : ������
	* @return		void
	* @remarks
	*/
	void UnregisterMessageRange(long long id, handle fun)
	{//����boostδʵ��function�ıȽϣ����޷�ʵ��
        m_MsgRangeCollect.erase(id);
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
            HandleMap& lstHandle = itMsg->second;
            HandleMap::iterator it = lstHandle.begin();
            HandleMap::iterator itEnd = lstHandle.end();
			for (; it != itEnd; it++)
			{
                handle& hd = it->second;
                (hd)(t);
			}

			bFired = true;
		}

		//��������
		MessageRangeCollect::iterator itRg = m_MsgRangeCollect.begin();
		MessageRangeCollect::iterator itRgEnd = m_MsgRangeCollect.end();
		for (; itRg != itRgEnd; itRg++)
		{
			if ((itRg->second).IsInRange(messageid))
			{
				bFired = true;

                (itRg->second).fun(t);
			}
		}

		return bFired;
	}

protected:
    typedef unordered_map<long long, handle> HandleMap;					//����������
    typedef unordered_map<int, HandleMap> MessageHandleMap;		//key=��ϢID,HandleVector=����������
    int incre;

	typedef struct tagMessageRangeHandle
	{
        tagMessageRangeHandle(){}
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

    typedef unordered_map<long long, MessageRangeHandle> MessageRangeCollect;

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