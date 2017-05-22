/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		SHAREDMEMORY.H
** @brief		共享内存实现
**  
** Details	
**  
** @version	1.0
** @date		2014/7/24 14:26
**  
** 修订说明：	最初版本 
** 
****************************************************************************/


#pragma once


#include "SharedObject.h"




/**
* @brief		共享内存封装
* @details		提供共享内存块对象
* @date			2014/7/24 14:27
* @note			无 
*/ 

template<typename T>
class SharedMemory
{
protected:
	typedef boost::function< void(T*) > handle;

	struct SharedMemorySpace
	{
		int32 counter;		/** 被打开计数 */
		int32 pagesize;		/** 页大小 */
		T	FirstObject;		/** 共享内存对象 */
	};

	struct SharedMemoryPage
	{
		SharedMemorySpace* m_pdata;							/** 指定共享内存地址 */
		boost::interprocess::mapped_region*   m_pregion;
		shared_memory_object*  m_shm;			/** 共享内存对象 */
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
	* @brief		构造函数
	* @details		读取共享内存调用
	* @param[in]	name[xstring] : 模块名称
	* @remarks		
	*/
	SharedMemory(const xstring name)
		: m_ModuleName(name),m_PageCount(0),m_ObjectCount(0),m_PageSize(0)
	{
		importPage();
	}


	/**
	* @brief		析构函数
	* @details		
	* @remarks		
	*/
	~SharedMemory(void)
	{
		//释放所有共享内存
		for(int32 i=0; i<(int32)m_SharedPageCollect.size(); i++)
		{
			if(m_SharedPageCollect[i].m_pdata->counter<=1)
			{
				//输出日志
#ifndef _DEBUG
				while(m_SharedPageCollect[i].m_pdata->counter>=2)
				{//等待共享内存计数增加
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
	* @brief		获取共享内存块
	* @details		
	* @param[in]	bCreate[bool] : 是否创建纪录，true触发insert操作
	* @return		T* : 共享内存块
	* @remarks		
	*/
	T* NewObject(bool bCreate=true)
	{
		if( m_FreeObjectCollect.size()==0 )
		{
			//整理，看有没有空闲的块，加到m_FreeObjectCollect中
			clearDirtyBlock();
		}

		if( m_FreeObjectCollect.size()==0 )
		{
			createPage();
		}

		CHECKERRORANDRETURNVALUE(m_FreeObjectCollect.size()>0,NULL);

		T* pTmp = *m_FreeObjectCollect.begin();
		m_FreeObjectCollect.erase(m_FreeObjectCollect.begin());

		new (pTmp)(T);		//清理脏数据

		//设置为使用状态
		pTmp->m_used = true;

		if(bCreate)
		{
			pTmp->m_create = true;
		}

		m_UsedObjectCollect.push_back(pTmp);

		return pTmp;
	}


	/**
	* @brief		释放一个共享内存块
	* @details		
	* @param[in]	pObject[T*] : 共享内存块指针
	* @return		bool : true=释放成功 false=失败
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
	* @brief		获取所有object的个数
	* @return		int32 : 所有块个数
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
	* @brief		导入所有共享内存页
	* @return		bool
	* @remarks		
	*/
	bool importPage()
	{
		bool bImport = false;

		//一直遍历
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

				page.m_pdata->counter = 2;			//设置为两端都持有

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
	* @brief		创建出一个共享内存页
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

		//初始化所有内存
		page.m_pdata->pagesize = m_PageSize;

		//遍历所有块，并且调用初始化
		for(int32 i=0; i<m_PageSize; i++)
		{
			T* pTmp = &(page.m_pdata->FirstObject) + i;
			new(pTmp)(T);
		}

		initPage(page);

		return true;
	}


	/**
	* @brief		初始化一页
	* @details		
	* @param[in]	
	* @return		
	* @remarks		
	*/
	void initPage(SharedMemoryPage& page)
	{
		m_PageCount++;
		m_ObjectCount += m_PageSize;

		//遍历初始化
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
	* @brief		清理失效的数据块
	*/
	void clearDirtyBlock()
	{
		SharedObjectCollect::iterator it = m_UsedObjectCollect.begin();
		SharedObjectCollect::iterator itEnd = m_UsedObjectCollect.end();
		for( ; it!=itEnd; it++ )
		{
			T* pSharedObject = *it;
			if(!pSharedObject->NeedHandle() && !pSharedObject->IsHandling() && !pSharedObject->m_used)
			{//不在处理中，不需要处理，已释放
				m_FreeObjectCollect.push_back(pSharedObject);
				it = m_UsedObjectCollect.erase(it);
			}
		}
	}


private:
	typedef xvector<SharedMemoryPage> SharedMemoryPageCollect;
	SharedMemoryPageCollect m_SharedPageCollect;		/** 共享内存页 */

	int32 m_PageSize;			/** 页大小 */

	int32 m_PageCount;			/** 页数 */

	int32 m_ObjectCount;		/** 块个数 */

	xstring m_ModuleName;		/** 共享内存名称 */

	typedef xset<T*> SharedObjectSet;
	typedef xlist<T*> SharedObjectCollect;

	SharedObjectCollect m_FreeObjectCollect;		/** 未被使用的Object集合 */
	SharedObjectCollect m_UsedObjectCollect;		/** 已占用的Object集合 */
	SharedObjectCollect m_AllObjectCollect;				/** 所有块，共享内存读取端使用 */
};

