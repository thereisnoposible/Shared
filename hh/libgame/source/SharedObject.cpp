#include "pch.h"

#include "SharedObject.h"


const int8 SharedObject::ObjectFlag = (char)0x5A;


/*
还在处理过程中：handleTime > updateTime
不需要更新：   updateTime > modifyTime
*/

//------------------------------------------------------------
SharedObject::SharedObject(void)
{
	m_flag = ObjectFlag;		/** 标记0x5A */
	m_lock = false;				/** 是否锁定 */
	m_used = false;				/** 是否正在使用 */
	m_create = false;			/** 是否创建 */
	m_destroy = false;			/** 是否删除 */
	m_handling = false;
	m_modifyTime = 0;			/** 纪录修改时间 */
	m_updateTime = 0;	/** 记录更新时间 */
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