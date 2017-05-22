#pragma once

#include "SharedMemory.h"
#include "DBConnect.h"



template<typename T>
class SharedMemSave
{
public:
	SharedMemSave(const xstring& name)
		: m_ModuleName(name),m_pMemPool(NULL),m_UpdateInterval(10)
	{
		m_LastTime = time(NULL);
	}

	~SharedMemSave(void)
	{
		SafeDelete(m_pMemPool);
	}

public:
	virtual bool SaveHandler()
	{
		//���������ڴ�
		if(NULL == m_pMemPool)
		{
			m_pMemPool = new MemoryPool(m_ModuleName);
		}

		if(NULL == m_pMemPool)
		{
			THROW("Memory Pool Open error!");
			return false;
		}

		//����pageҳ
		m_pMemPool->importPage();


		//��ʱ�������й����ڴ��
		time_t currTime = time(NULL);
		if(currTime-m_LastTime >= m_UpdateInterval)
		{
			m_pMemPool->foreach(boost::bind(&SharedMemSave::handleSharedObject,this,_1));
			m_LastTime = time(NULL);
		}
		
		return true;
	}


protected:
	virtual void create(T* pdata) {};

	virtual void update(T* pdata) {};

	virtual void destroy(T* pdata) {};

	virtual void handleCreate(T* pdata,DBCommandPtr pCmdRes)
	{
		//�жϳɹ�ʧ��
		if(pCmdRes->m_Result)
		{
			pdata->m_create = false;
		}
		else
		{
			pdata->m_updateTime = pdata->m_modifyTime - 1;	//��������create
		}

		pdata->m_handling = false;

	}

	virtual void handleUpdate(T* pdata,DBCommandPtr pCmdRes)
	{
		if(!pCmdRes->m_Result)
		{
			pdata->m_updateTime = pdata->m_modifyTime - 1;		//��������update
		}

		pdata->m_handling = false;
	}

	virtual void handleDestroy(T* pdata,DBCommandPtr pCmdRes)
	{
		if(pCmdRes->m_Result)
		{
			pdata->m_destroy = false;
			pdata->m_used = false;		//����ʹ��

		}

		pdata->m_handling = false;
	}


	void handleSharedObject(T* pdata)
	{
		//�������ڴ�ı���,�ֱ����create update destroy
		//if(!pdata->m_used) return;

		if(pdata->m_lock) return;

		if(pdata->m_handling) return;

		if(pdata->NeedHandle())
		{
			pdata->m_handling = true;
			pdata->m_updateTime = ToInt32(time(NULL));

			if(pdata->m_create)
			{
				create(pdata);
				return;
			}

			update(pdata);
			return;
		}

		if(pdata->m_destroy)
		{
			pdata->m_handling = true;
			destroy(pdata);
		}
		
	}

protected:
	typedef SharedMemory<T> MemoryPool;

	MemoryPool* m_pMemPool;

	xstring m_ModuleName;

	int32 m_UpdateInterval;

	time_t m_LastTime;
};

