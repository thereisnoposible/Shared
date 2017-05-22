#include "Event.h"

namespace ZeroSpace
{
	CEvent::~CEvent()
	{
		m_handlers.Clear();
	}

	void CEvent::AddHandler(EventHandlerPtr pHandler)
	{
		m_handlers.Insert(pHandler->GetEventID(), pHandler);
	}

	void CEvent::RemoveHandler(EventIDType id)
	{
		m_handlers.Erase(id);
	}

	CEventRetuirnBase& CEvent::Trigger(CEventArgsBase* pArgs)
	{
		static CEventRetuirn<void> errorReturn;
		if (pArgs == NULL)
		{
			return errorReturn;
		}
		EventHandlerType::Iterator it = m_handlers.Find(pArgs->GetEventID());
		if (it == m_handlers.GetEnd())
		{
			return errorReturn;
		}
		return it->GetValue()->TriggerEvent(pArgs);
	}

	CEventRetuirnBase& CEvent::Trigger(const byte* buffer, size_t len)
	{
		static CEventRetuirn<void> errorReturn;
		EventIDType eventID = *((EventIDType*)buffer);
		EventHandlerType::Iterator it = m_handlers.Find(eventID);
		if (it == m_handlers.GetEnd())
		{
			return errorReturn;
		}
		return it->GetValue()->TriggerEvent(buffer, len);
	}

	CEventProxy& CEvent::EventID(EventIDType id)
	{
		m_proxy.m_eventID = id;
		return m_proxy;
	}

	EventHandlerPtr CEvent::GetHandler(EventIDType id)
	{
		EventHandlerType::Iterator it = m_handlers.Find(id);
		if (it == m_handlers.GetEnd())
		{
			return NULL;
		}
		return it->GetValue();
	}
	
}
