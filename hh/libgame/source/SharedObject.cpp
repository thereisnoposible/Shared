#include "pch.h"

#include "SharedObject.h"


const int8 SharedObject::ObjectFlag = (char)0x5A;


/*
���ڴ�������У�handleTime > updateTime
����Ҫ���£�   updateTime > modifyTime
*/

//------------------------------------------------------------
SharedObject::SharedObject(void)
{
	m_flag = ObjectFlag;		/** ���0x5A */
	m_lock = false;				/** �Ƿ����� */
	m_used = false;				/** �Ƿ�����ʹ�� */
	m_create = false;			/** �Ƿ񴴽� */
	m_destroy = false;			/** �Ƿ�ɾ�� */
	m_handling = false;
	m_modifyTime = 0;			/** ��¼�޸�ʱ�� */
	m_updateTime = 0;	/** ��¼����ʱ�� */
}


//------------------------------------------------------------
SharedObject::~SharedObject(void)
{
}


//------------------------------------------------------------
void SharedObject::Lock()
{
	m_lock = true;
}


//------------------------------------------------------------
void SharedObject::Unlock()
{
	m_modifyTime = ToInt32(time(NULL));
	m_lock = false;
}


//------------------------------------------------------------
void SharedObject::Release()
{
	m_used = false;
}


//------------------------------------------------------------
void SharedObject::Destroy()
{
	m_used = false;
	m_destroy = true;
}


//------------------------------------------------------------
bool SharedObject::IsHandling()
{
	return m_handling;
}


//------------------------------------------------------------
bool SharedObject::NeedHandle()
{
	if(m_modifyTime > m_updateTime)
		return true;

	return false;
}