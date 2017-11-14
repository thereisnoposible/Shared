#ifndef _EVENT_H_
#define _EVENT_H_
#include "EventDefine.h"
//#include "EventArg.h"
//#include "EventHandler.h"
//#include "../Function/Function.h"
//#include "../Containers/TreeMap.h"
//#include "../Containers/CircularQueue.h"
//#include "../Containers/List.h"
#include "EventReturn.h"
#include <map>

namespace ZeroSpace
{

	class CEvent;

	class CEventProxy
	{
		friend class CEvent;
	public:
		CEventProxy(CEvent& eventx);
		template <typename... _Types >
		CEventRetuirnBase& Trigger(_Types... args);
	private:
		size_t		m_eventID;
		CEvent&		m_event;
	};

	class CEvent
	{
	public:
		typedef std::map<size_t, EventHandlerPtr> EventHandlerType;
		void AddHandler(EventHandlerPtr pHandler);
		void RemoveHandler(size_t id);

		CEvent():m_proxy(*this){}

		~CEvent();

		//CEventArgsBase* GetArgs(const byte* buffer, size_t len);
		// 触发事件 eventID：事件id pArgs：事件参数
		CEventRetuirnBase& Trigger(CEventArgsBase* pArgs);
		// 触发事件 eventID：事件id 
		CEventRetuirnBase& Trigger(const byte* buffer, size_t len);

		// 获取事件触发器代理
		CEventProxy& EventID(size_t id);

		// 获取事件
		EventHandlerPtr GetHandler(size_t id);

		// 注册事件
		template<typename _FuncType>
		void Regitster(size_t id, const _FuncType& func);

		// 注册事件
		template<typename _FuncType, typename _Object>
		void Regitster(size_t id, const _FuncType& func, _Object* pObject);

	private:
		CEventProxy			m_proxy;		//事件触发代理
		EventHandlerType	m_handlers;		//事件集合
	};

	template<typename _FuncType>
	inline void CEvent::Regitster(EventIDType id, const _FuncType& func)
	{
		EventHandlerPtr pHandler = GetHandler(id);
		CEventHandler< typename CGetEventHandlerType<_FuncType>::DataType >* pEventHandler = NULL;
		if (pHandler == NULL)
		{
			pEventHandler = new CEventHandler< typename CGetEventHandlerType<_FuncType>::DataType >();
			pHandler = pEventHandler;
			pHandler->SetEventID(id);
			AddHandler(pHandler);
		}
		else
		{
			pEventHandler = pHandler.DynamicCast< CEventHandler< typename CGetEventHandlerType<_FuncType>::DataType > >();
		}

		if (pEventHandler == NULL)
		{
			return;
		}
		pEventHandler->Regitster(func);
	}

	template<typename _FuncType, typename _Object>
	inline void CEvent::Regitster(EventIDType id, const _FuncType& func, _Object* pObject)
	{
		Regitster(id, CEventMethodX<_FuncType>::_Bind(func, pObject) );
	}

	inline CEventProxy::CEventProxy(CEvent& eventx) : m_event(eventx)
	{
	}

	template<typename ..._Types>
	inline CEventRetuirnBase & CEventProxy::Trigger(_Types ...args)
	{
		typedef CEventArgs< typename stEventArgDataType<_Types>::Type... > EventArgsType;
		EventArgsType eventArgs(m_eventID, args...);
		return m_event.Trigger(&eventArgs);
	}

}

#endif
