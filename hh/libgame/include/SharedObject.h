#pragma once


class XClass SharedObject
{
public:
	/**
	* @brief		构造函数
	*/
	SharedObject(void);

	/**
	* @brief		析构函数	
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
	//共享内存块公共信息
	int8 m_flag;			/** 标记0x5A */
	bool m_lock;			/** 是否锁定 */
	bool m_used;			/** 是否正在使用 */
	bool m_create;			/** 是否创建 */
	bool m_destroy;			/** 是否删除 */
	bool m_handling;		/** 是否正在操作 */
	int32 m_modifyTime;		/** 纪录修改时间 */
	int32 m_updateTime;		/** 记录更新时间 */


private:
	static const char ObjectFlag;		/** 共享内存块标识符 */

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