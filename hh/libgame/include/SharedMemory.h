/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		SHAREDMEMORY.H
** @brief		�����ڴ�ʵ��
**  
** Details	
**  
** @version	1.0
** @date		2014/7/24 14:26
**  
** �޶�˵����	����汾 
** 
****************************************************************************/


#pragma once


#include "SharedObject.h"




/**
* @brief		�����ڴ��װ
* @details		�ṩ�����ڴ�����
* @date			2014/7/24 14:27
* @note			�� 
*/ 

template<typename T>
class SharedMemory
{
protected:
	typedef boost::function< void(T*) > handle;

	struct SharedMemorySpace
	{
		int32 counter;		/** ���򿪼��� */
		int32 pagesize;		/** ҳ��С */
		T	FirstObject;		/** �����ڴ���� */
	};

	struct SharedMemoryPage
	{
		SharedMemorySpace* m_pdata;							/** ָ�������ڴ��ַ */
		boost::interprocess::mapped_region*   m_pregion;
		shared_memory_object*  m_shm;			/** �����ڴ���� */
	};

public:
	SharedMemory(const xstring name,int32 pagesize) 
		: m_ModuleName(name),m_PageSize(pagesize),m_PageCount(0),m_ObjectCount(0)
	{
		if(!importPage())
		{
			createPage();
		}
	}


	/**
	* @brief		���캯��
	* @details		��ȡ�����ڴ����
	* @param[in]	name[xstring] : ģ������
	* @remarks		
	*/
	SharedMemory(const xstring name)
		: m_ModuleName(name),m_PageCount(0),m_ObjectCount(0),m_PageSize(0)
	{
		importPage();
	}


	/**
	* @brief		��������
	* @details		
	* @remarks		
	*/
	~SharedMemory(void)
	{
		//�ͷ����й����ڴ�
		for(int32 i=0; i<(int32)m_SharedPageCollect.size(); i++)
		{
			if(m_SharedPageCollect[i].m_pdata->counter<=1)
			{
				//�����־
#ifndef _DEBUG
				while(m_SharedPageCollect[i].m_pdata->counter>=2)
				{//�ȴ������ڴ��������
					Sleep(1);
				}
#endif
			}

			SafeDelete(m_SharedPageCollect[i].m_pregion);
			SafeDelete(m_SharedPageCollect[i].m_shm);
		}
	}

public:
	/**
	* @brief		��ȡ�����ڴ��
	* @details		
	* @param[in]	bCreate[bool] : �Ƿ񴴽���¼��true����insert����
	* @return		T* : �����ڴ��
	* @remarks		
	*/
	T* NewObject(bool bCreate=true)
	{
		if( m_FreeObjectCollect.size()==0 )
		{
			//��������û�п��еĿ飬�ӵ�m_FreeObjectCollect��
			clearDirtyBlock();
		}

		if( m_FreeObjectCollect.size()==0 )
		{
			createPage();
		}

		CHECKERRORANDRETURNVALUE(m_FreeObjectCollect.size()>0,NULL);

		T* pTmp = *m_FreeObjectCollect.begin();
		m_FreeObjectCollect.erase(m_FreeObjectCollect.begin());

		new (pTmp)(T);		//����������

		//����Ϊʹ��״̬
		pTmp->m_used = true;

		if(bCreate)
		{
			pTmp->m_create = true;
		}

		m_UsedObjectCollect.push_back(pTmp);

		return pTmp;
	}


	/**
	* @brief		�ͷ�һ�������ڴ��
	* @details		
	* @param[in]	pObject[T*] : �����ڴ��ָ��
	* @return		bool : true=�ͷųɹ� false=ʧ��
	* @remarks		
	*/
	bool ReleaseObject(T* pObject)
	{
		SharedObjectSet::iterator it = m_UsedObjectCollect.find(pObject);
		if( it!= m_UsedObjectCollect.end())
		{
			T* pTmp = *it;
			m_UsedObjectCollect.erase(it);
			m_FreeObjectCollect.push_back(pTmp);
		}

		return true;
	}


	/**
	* @brief		��ȡ����object�ĸ���
	* @return		int32 : ���п����
	* @remarks		
	*/
	int32 GetSize()
	{
		return m_ObjectCount;
	}


	void foreach(handle fun)
	{
		SharedObjectCollect::iterator it = m_AllObjectCollect.begin();
		SharedObjectCollect::iterator itEnd = m_AllObjectCollect.end();
		for(; it!=itEnd; it++)
		{
			fun(*it);
		}
	}


	/**
	* @brief		�������й����ڴ�ҳ
	* @return		bool
	* @remarks		
	*/
	bool importPage()
	{
		bool bImport = false;

		//һֱ����
		while(true)
		{
			xstring pagename = m_ModuleName + "_" + Helper::Int32ToString(m_PageCount);
			try
			{
				SharedMemoryPage page;
				
				page.m_shm = new shared_memory_object(boost::interprocess::open_only,pagename.c_str(),boost::interprocess::read_write);
				
				page.m_pregion = new boost::interprocess::mapped_region(*page.m_shm,boost::interprocess::read_write);

				page.m_pdata = static_cast<SharedMemorySpace*>(page.m_pregion->get_address());
				
				m_PageSize = page.m_pdata->pagesize;

				page.m_pdata->counter = 2;			//����Ϊ���˶�����

				initPage(page);
				
				bImport = true;
			}
			catch(...)
			{
				break;
			}
		}
		
		return bImport;
	}


protected:
	/**
	* @brief		������һ�������ڴ�ҳ
	* @return		bool
	* @remarks		
	*/
	bool createPage()
	{
		xstring pagename = m_ModuleName + "_" + Helper::Int32ToString(m_PageCount);
		int32 size = m_PageSize*sizeof(T) + sizeof(int32)*2;
		SharedMemoryPage page;

		page.m_shm = new shared_memory_object(boost::interprocess::create_only,pagename.c_str(),boost::interprocess::read_write,size);
		page.m_pregion = new boost::interprocess::mapped_region(*page.m_shm,boost::interprocess::read_write);
		page.m_pdata = static_cast<SharedMemorySpace*>(page.m_pregion->get_address());

		//��ʼ�������ڴ�
		page.m_pdata->pagesize = m_PageSize;

		//�������п飬���ҵ��ó�ʼ��
		for(int32 i=0; i<m_PageSize; i++)
		{
			T* pTmp = &(page.m_pdata->FirstObject) + i;
			new(pTmp)(T);
		}

		initPage(page);

		return true;
	}


	/**
	* @brief		��ʼ��һҳ
	* @details		
	* @param[in]	
	* @return		
	* @remarks		
	*/
	void initPage(SharedMemoryPage& page)
	{
		m_PageCount++;
		m_ObjectCount += m_PageSize;

		//������ʼ��
		for(int32 i=0; i<m_PageSize; i++)
		{
			T* pTem = &(page.m_pdata->FirstObject) + i;

			m_AllObjectCollect.push_back(pTem);

			if(pTem->m_used)
			{
				m_UsedObjectCollect.push_back(pTem);
			}
			else
			{
				m_FreeObjectCollect.push_back(pTem);
			}
		}

		m_SharedPageCollect.push_back(page);
	}


	/**
	* @brief		����ʧЧ�����ݿ�
	*/
	void clearDirtyBlock()
	{
		SharedObjectCollect::iterator it = m_UsedObjectCollect.begin();
		SharedObjectCollect::iterator itEnd = m_UsedObjectCollect.end();
		for( ; it!=itEnd; it++ )
		{
			T* pSharedObject = *it;
			if(!pSharedObject->NeedHandle() && !pSharedObject->IsHandling() && !pSharedObject->m_used)
			{//���ڴ����У�����Ҫ�������ͷ�
				m_FreeObjectCollect.push_back(pSharedObject);
				it = m_UsedObjectCollect.erase(it);
			}
		}
	}


private:
	typedef xvector<SharedMemoryPage> SharedMemoryPageCollect;
	SharedMemoryPageCollect m_SharedPageCollect;		/** �����ڴ�ҳ */

	int32 m_PageSize;			/** ҳ��С */

	int32 m_PageCount;			/** ҳ�� */

	int32 m_ObjectCount;		/** ����� */

	xstring m_ModuleName;		/** �����ڴ����� */

	typedef xset<T*> SharedObjectSet;
	typedef xlist<T*> SharedObjectCollect;

	SharedObjectCollect m_FreeObjectCollect;		/** δ��ʹ�õ�Object���� */
	SharedObjectCollect m_UsedObjectCollect;		/** ��ռ�õ�Object���� */
	SharedObjectCollect m_AllObjectCollect;				/** ���п飬�����ڴ��ȡ��ʹ�� */
};

