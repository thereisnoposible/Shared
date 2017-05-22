#ifndef _GAME_EVENT_OBJECT_H_
#define _GAME_EVENT_OBJECT_H_

#include "../Public.h"
#include "../Object/Object.h"
#include "Event.h"
#include "../Containers/HashMap.h"
#include "../SmartPtr/IntrusivePtr.h"
#include "../Function/Bind.h"

namespace ZeroSpace
{
	template< typename _Object>
	class ZS_API CEventHandlerMethodBase
	{
	public:
		typedef _Object ObjectType;
		EventIDType GetEventID() { return m_eventID; }
		void SetEventID(EventIDType id) { m_eventID = id; }
		virtual CEventRetuirnBase& TriggerEvent(CEventArgsBase* pData) = 0;
		void SetObject(ObjectType* pObject);
	protected:
		EventIDType m_eventID;
		ObjectType*	m_pObject;
	};

	template<typename _Object>
	inline void CEventHandlerMethodBase<_Object>::SetObject(ObjectType* pObject)
	{
		m_pObject = pObject;
	}

	template< typename _FuncType >
	class ZS_API CEventHandlerMethod
	{

	};

	template< typename _Object, typename _Ret >
	class ZS_API CEventHandlerMethod<_Ret (_Object::*)()> : public CEventHandlerMethodBase<_Object>
	{
	public:
		typedef _Ret RetType;
		typedef _Object ObjectType;
		typedef RetType (ObjectType::*MethodPrt)();
		typedef CArray<MethodPrt> MethodList;

		CEventHandlerObject()
		{
		}

		void TriggerEvent(CEventArgsBase* pArgs)
		{
			if (m_pObject == NULL)
			{
				return m_return;
			}
			CEventArgs<_Ret()>* pDataArgs = dynamic_cast<CEventArgs<_Ret()>*>(pArgs);
			if (pDataArgs == NULL)
			{
				return m_return;
			}
			MethodPrt pMethod = NULL;
			MethodList::Iterator it = m_methodList.GetBegin();
			for (; it != m_methodList.GetEnd(); ++it)
			{
				pMethod = *it;
				m_return.Push((m_pObject->*pMethod)());
			}
			return m_return;
			return (m_pObject->*pMethod)()
		}
	private:
		
		MethodPrt  m_pMethod;
		CEventRetuirn<RetType> m_return;
	};

	class ZS_API CEventObjectMethodBase
	{
	};

	template< typename _MethodType >
	class ZS_API CEventObjectMethod : public CEventObjectMethodBase
	{
	};

	template< typename _Object, typename _Ret >
	class ZS_API CEventObjectMethod<_Ret (_Object::*)()> : public CEventObjectMethodBase
	{
	public:
		typedef _Ret RetType;
		typedef _Object ObjectType;
		typedef RetType(ObjectType::*MethodPrt)();
		CEventObjectMethod(MethodPrt pMethod):m_pMethod(pMethod)
		{
		}

		MethodPrt GetMethod() { return m_pMethod; }
	private:
		MethodPrt m_pMethod;
	};


	template< typename _Object >
	class ZS_API CEventObject
	{
	public:
		typedef _Object ObjectType;

		CEventObject(ObjectType& object);

		template<typename _FuncType>
		void Regitster(EventIDType id, const _FuncType& func);
		
		CEventRetuirnBase& Trigger(CEventArgsBase* pArgs);
	private:
		ObjectType& m_object;
	};

	template<typename _Object>
	inline CEventObject<_Object>::CEventObject(ObjectType& object) :m_object(object)
	{
	}

	template<typename _Object>
	inline CEventRetuirnBase& CEventObject<_Object>::Trigger(CEventArgsBase* pArgs)
	{
		return 
	}

	template<typename _Object>
	template<typename _FuncType>
	inline void CEventObject<_Object>::Regitster(EventIDType id, const _FuncType& func)
	{
		new CEventHandlerObject<_FuncType>(m_object);
	}
	
}
#endif
