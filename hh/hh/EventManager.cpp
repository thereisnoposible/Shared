#include "EventManager.h"
#include "../Function/Bind.h"
namespace ZeroSpace
{
	CEventManager::CEventManager()
	{
	}

	void CEventManager::BindQueue(EventArgsQueueType* pQueue)
	{
		m_argsQueues.Offer(pQueue);
	}

	void CEventManager::AddEvent(ObjectIDType id, CEvent* pEvent)
	{
		m_eventMap.Insert(id, pEvent);
	}

	CEvent* CEventManager::GetEvent(ObjectIDType id)
	{
		EventMap::Iterator it = m_eventMap.Find(id);
		if (m_eventMap.GetEnd() == it)
		{
			return NULL;
		}
		return it->GetValue();
	}

	CEvent& CEventManager::GetEvent()
	{
		return m_event;
	}

	void CEventManager::TriggerEvent(ObjectIDType id, CEventArgsBase* pArgs)
	{
		if (id == 0)
		{
			//m_event.Trigger();
		}
		CEvent* pEvent = GetEvent(id);
		if (pEvent == NULL)
		{
			return;
		}
		//pEvent->Trigger(pArgs);
	}

	void CEventManager::Update()
	{
		EventArgsQueueType* pQueue = NULL;
		while (!m_argsQueues.IsEmpty())
		{
			pQueue = m_argsQueues.Poll();
			m_argsQueueList.PushBack(pQueue);
		}

		EventArgsType* pArgs = NULL;
		EventArgsQueueList::Iterator it = m_argsQueueList.GetBegin();
		for (; it != m_argsQueueList.GetEnd(); ++it)
		{
			if ( (*it) == NULL )
			{
				continue;
			}

			while ( !(*it)->IsEmpty())
			{
				pArgs = (*it)->Poll();
				//m_event.Trigger(pArgs);
			} // while (pArgs = (*it)->Poll())
		} // for (; it != m_argsQueueList.GetEnd(); ++it)
	}


}
