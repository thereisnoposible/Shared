#pragma once

#include <xmemory0>

namespace xlib
{
	///////////////
	//单向链表节点
	//_Type 节点值
	//////////////
	template < typename _Type >
	struct stOneWayListNode
	{
		template < typename _Typex, typename _Alloc > friend class CCircularQueue;
		template < typename _Typex, typename _Alloc > friend class CAtomicQueue;
		const stOneWayListNode<_Type>* GetNext() const { return next; }
		stOneWayListNode<_Type>* GetNext(){ return next; }

		_Type data;//值
	private:
		stOneWayListNode<_Type>* next;//下一个节点
	};

	// 循环单链表队列
	// 支持一线程取一线程放
	template < typename _Type, typename _Alloc = std::allocator<_Type> >
	class CCircularQueue
	{
	public:
		typedef typename _Alloc::template rebind< stOneWayListNode< typename _Alloc::value_type > >::other AllocType;
		typedef	CCircularQueue<_Type, _Alloc>	SelfType; // 自己
		typedef stOneWayListNode<_Type>			NodeType;
		typedef _Type							ValType;

		CCircularQueue();
		CCircularQueue(const SelfType& q);
		~CCircularQueue();
		bool Offer(const ValType& val);
		bool Offers(const SelfType& q);
		ValType* Allot();
		ValType* Get();
		ValType Poll();

		size_t GetCount()const;
		size_t GetCountNode()const;

		bool IsEmpty() const{ return m_head == m_end; }
		void Clear();

		SelfType& operator =(const SelfType& q);
	private:
		//分配一个节点
		NodeType* CreateNode();
		//销毁节点
		void DestroyNode(NodeType* pNode);
		//增加一个节点
		bool AddNode();

		NodeType*				m_head;			//链表头
		NodeType*				m_end;			//链表尾
		AllocType				m_alloc;		//分配器
		volatile size_t			m_count;		//元素个数
		volatile size_t			m_countNode;	//节点个数
	};

	template < typename _Type, typename _Alloc >
	CCircularQueue<_Type, _Alloc>::CCircularQueue() :m_head(0), m_end(0), m_count(0), m_countNode(0)
	{
		m_end = m_head = CreateNode();
		m_head->next = m_head;
	}

	template < typename _Type, typename _Alloc >
	CCircularQueue<_Type, _Alloc>::CCircularQueue(const SelfType& q)
	{
		Clear();
		Offers(q);
	}
	template < typename _Type, typename _Alloc >
	typename CCircularQueue<_Type, _Alloc>::SelfType&
		CCircularQueue<_Type, _Alloc>::operator =(const SelfType& q)
	{
		Clear();
		Offers(q);
		return *this;
	}

	template < typename _Type, typename _Alloc >
	CCircularQueue<_Type, _Alloc>::~CCircularQueue()
	{
		Clear();
		NodeType* pNode = m_head;
		m_head = m_head->GetNext();
		DestroyNode(pNode);
		while (m_head != m_end)
		{
			pNode = m_head;
			m_head = m_head->GetNext();
			DestroyNode(pNode);
		}
	}

	//************************************
	// Method:    CreateNode
	// FullName:  CCircularQueue<_Type, _Alloc>::CreateNode
	// Access:    public
	// Returns:   stListNode<T>* 返回创建的节点
	// Qualifier: 创建一个节点
	//************************************
	template < typename _Type, typename _Alloc >
	typename CCircularQueue<_Type, _Alloc>::NodeType*
		CCircularQueue<_Type, _Alloc>::CreateNode()
	{
		++m_countNode;
		return m_alloc.allocate(1);
	}

	//************************************
	// Method:    DestroyNode
	// FullName:  CCircularQueue<_Type, _Alloc>::DestroyNode
	// Access:    public
	// Returns:   void
	// Qualifier: 销毁内存
	//************************************
	template < typename _Type, typename _Alloc >
	void CCircularQueue<_Type, _Alloc>::DestroyNode(NodeType* pNode)
	{
		if (pNode == 0)
		{
			return;
		}
		pNode->data.~_Type();
		--m_countNode;
		m_alloc.deallocate(pNode, 1);
	}
	template < typename _Type, typename _Alloc >
	bool CCircularQueue<_Type, _Alloc>::AddNode()
	{
		NodeType* pNode = CreateNode();
		if (pNode == 0)
		{
			return false;
		}
		m_end->next = pNode;
		pNode->next = m_head;
		return true;
	}

	template < typename _Type, typename _Alloc >
	bool CCircularQueue<_Type, _Alloc>::Offer(const ValType& val)
	{
		if (m_end->GetNext() == m_head)
		{
			if (AddNode() == false)
			{
				return false;
			}
		}
		m_end->data = val;
		m_end = m_end->GetNext();
		++m_count;
		return true;
	}

	template < typename _Type, typename _Alloc >
	bool CCircularQueue<_Type, _Alloc>::Offers(const SelfType& q)
	{
		if (q.IsEmpty())
		{
			return true;
		}
		for (const NodeType* it = q.m_head; it != q.m_end; it = it->GetNext())
		{
			if (!Offer(it->data))
			{
				return false;
			}
		}
		return true;
	}

	template < typename _Type, typename _Alloc >
	typename CCircularQueue<_Type, _Alloc>::ValType*
		CCircularQueue<_Type, _Alloc>::Allot()
	{
		if (m_end->GetNext() == m_head)
		{
			if (AddNode() == false)
			{
				return false;
			}
		}
		NodeType* pRetNode = m_end;
		m_end = m_end->GetNext();
		++m_count;
		return &(pRetNode->data);
	}

	template < typename _Type, typename _Alloc >
	typename CCircularQueue<_Type, _Alloc>::ValType*
		CCircularQueue<_Type, _Alloc>::Get()
	{
		if (m_head == m_end || m_count == 0)
		{
			return 0;
		}
		return &(m_head->data);
	}

	template < typename _Type, typename _Alloc >
	typename CCircularQueue<_Type, _Alloc>::ValType
		CCircularQueue<_Type, _Alloc>::Poll()
	{
		if (m_head == m_end || m_count == 0)
		{
			return ValType();
		}
		--m_count;
		ValType ret = m_head->data;
		m_head = m_head->GetNext();

		return ret;
	}

	template < typename _Type, typename _Alloc >
	size_t CCircularQueue<_Type, _Alloc>::GetCount()const
	{
		return m_count;
	}

	template < typename _Type, typename _Alloc >
	void CCircularQueue<_Type, _Alloc>::Clear()
	{
		m_count = 0;
		m_end = m_head;
	}
	template < typename _Type, typename _Alloc >
	size_t CCircularQueue<_Type, _Alloc>::GetCountNode()const
	{
		return m_countNode;
	}

}
