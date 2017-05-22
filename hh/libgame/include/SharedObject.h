#pragma once


class XClass SharedObject
{
public:
	/**
	* @brief		���캯��
	*/
	SharedObject(void);

	/**
	* @brief		��������	
	*/
	~SharedObject(void);

public:
	inline void Lock();

	inline void Unlock();

	inline void Release();

	inline void Destroy();

	bool IsHandling();

	bool NeedHandle();

public:
	//�����ڴ�鹫����Ϣ
	int8 m_flag;			/** ���0x5A */
	bool m_lock;			/** �Ƿ����� */
	bool m_used;			/** �Ƿ�����ʹ�� */
	bool m_create;			/** �Ƿ񴴽� */
	bool m_destroy;			/** �Ƿ�ɾ�� */
	bool m_handling;		/** �Ƿ����ڲ��� */
	int32 m_modifyTime;		/** ��¼�޸�ʱ�� */
	int32 m_updateTime;		/** ��¼����ʱ�� */


private:
	static const char ObjectFlag;		/** �����ڴ���ʶ�� */

};



class XClass SharedLock
{
public:
	SharedLock(SharedObject* pOjbect)
	{
		m_pOjbect = pOjbect;
		m_pOjbect->Lock();
	}


	~SharedLock()
	{
		m_pOjbect->Unlock();
	}

	void Unlock()
	{
		m_pOjbect->Unlock();
	}


protected:

	SharedObject*  m_pOjbect;
};