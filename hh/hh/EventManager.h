#ifndef _EVENT_MANAGER_H_
#define _EVENT_MANAGER_H_

#include "../Public.h"
#include "../Function/Function.h"
#include "Event.h"
#include "../Containers/CircularQueue.h"

namespace ZeroSpace
{
	typedef CEventArgs<void (ObjectIDType, CEventArgsBase*) > ObjectEventArg;
	class ZS_API CEventManager
	{
	public:
		typedef CEventArgsBase EventArgsType;
		typedef CCircularQueue<EventArgsType*> EventArgsQueueType;
		typedef CCircularQueue<EventArgsQueueType*> EventArgsQueuesType;
		typedef CList<EventArgsQueueType*> EventArgsQueueList;
		typedef CTreeMap<ObjectIDType, CEvent*> EventMap;

		CEventManager();
		void BindQueue(EventArgsQueueType* pQueue);
		void AddEvent(ObjectIDType id, CEvent* pEvent);
		CEvent* GetEvent(ObjectIDType id);
		CEvent& GetEvent();
		void TriggerEvent(ObjectIDType id, CEventArgsBase* pArgs);
		void Update();
	private:
		EventMap			m_eventMap;
		CEvent				m_event;
		EventArgsQueueList	m_argsQueueList;
		EventArgsQueuesType	m_argsQueues;
	};
}

#endif
