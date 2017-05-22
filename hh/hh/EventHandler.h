#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_
#include "EventDefine.h"
#include "EventArg.h"
#include "../SmartPtr/IntrusivePtr.h"
#include "../Function/Function.h"
#include "../Function/Bind.h"
#include "../Containers/List.h"
#include "../Containers/Array.h"
#include "../ByteStream/ByteStream.h"
#include "EventReturn.h"
namespace ZeroSpace
{

	template<typename _Func>
	class ZS_API CEventMethodX
	{
	public:
		typedef CFunction<_Func> FuncType;
	};

	template<typename _Ret, typename _Object, typename... _Datas>
	class ZS_API CEventMethodX<_Ret (_Object::*)(_Datas...)>
	{
	public:
		typedef CEventMethodX<_Ret(_Object::*)(_Datas...)> SelfType;
		typedef _Ret RetType;
		typedef _Object ObjectType;
		typedef RetType FuncType(_Datas...);
		typedef CFunction<RetType(_Datas...)> FuncObjType;
		typedef RetType(ObjectType::*MethodType)(_Datas...);

		static FuncObjType _Bind(MethodType pMethod, ObjectType* pObj)
		{
			return SelfType(pObj, pMethod);
		}

		CEventMethodX(ObjectType* pObject, MethodType pMethod):m_pObject(pObject), m_pMethod(pMethod)
		{
		}

		RetType operator()(_Datas... args)
		{
			return Invoker(m_pMethod, m_pObject, args...);
		}
	private:
		ObjectType* m_pObject;
		MethodType m_pMethod;
	};

	template< typename _Type >
	class ZS_API CGetEventHandlerType
	{
	public:
		typedef _Type DataType;
	};

	template< typename _Type >
	class ZS_API CGetEventHandlerType< CFunction<_Type> >
	{
	public:
		typedef _Type DataType;
	};

	class ZS_API CEventHandlerBase
	{
		INTRUSIVE_REF_COUNT()
	public:
		virtual ~CEventHandlerBase(){}
		EventIDType GetEventID() { return m_eventID; }
		void SetEventID(EventIDType id) { m_eventID = id; }
		
		virtual CEventRetuirnBase& TriggerEvent(CEventArgsBase* pData) = 0;
		virtual CEventRetuirnBase& TriggerEvent(const byte* buffer, size_t len) = 0;
	protected:
		EventIDType m_eventID;
	};

	template< typename _Data >
	class ZS_API CEventHandler : public CEventHandlerBase
	{
	};

	template<typename... _Datas >
	class ZS_API CEventHandler<void(_Datas...)> : public CEventHandlerBase
	{
	public:
		typedef void RetType;
		typedef void EventArgsHandlerFunc(_Datas...);
		typedef CFunction<EventArgsHandlerFunc> HandlerFunction;
		typedef CArray<HandlerFunction> HandlerFunctionList;
		typedef CEventArgs< typename stEventArgDataType<_Datas>::Type... > EventArgsType;

		void Regitster(const HandlerFunction& func)
		{
			m_handerList.PushBack(func);
		}

		CEventRetuirnBase& TriggerEvent(const byte* buf, size_t s)
		{
			CByteStream stream(buf, s, s);
			EventArgsType args;
			args.ParseFrom(stream);
			return EventFuncCall(typename EventArgsType::GetArgIndexType(), args);
		}

		CEventRetuirnBase& TriggerEvent(CEventArgsBase* pArgs)
		{
			if (pArgs == NULL)
			{
				return m_return;
			}
			EventArgsType* pDataArgs = dynamic_cast<EventArgsType*>(pArgs);
			if (pDataArgs == NULL)
			{
				CByteStream stream;
				pArgs->Serialize(stream);
				EventArgsType args;
				args.ParseFrom(stream);
				return EventFuncCall(typename EventArgsType::GetArgIndexType(), args);
			}

			return EventFuncCall(typename EventArgsType::GetArgIndexType(), *pDataArgs);
		}
	private:

		template<size_t..._Indexs>
		CEventRetuirn<RetType>& EventFuncCall(const CArgIndex<_Indexs...>&, EventArgsType& args)
		{
			return Invoke(stEventArgsGetData<_Datas>::Invoke(args.GetData<_Indexs>())...);
		}

		CEventRetuirn<RetType>& Invoke(_Datas... args)
		{
			typename HandlerFunctionList::Iterator it = m_handerList.GetBegin();
			for (; it != m_handerList.GetEnd(); ++it)
			{
				(*it)(args...);
			}
			return m_return;
		}

		HandlerFunctionList m_handerList;
		CEventRetuirn<RetType> m_return;
	};

	template< typename... _Datas >
	class ZS_API CEventHandler<void(*)(_Datas...)> : public CEventHandlerBase
	{
	public:
		typedef void RetType;
		typedef void EventArgsHandlerFunc(_Datas...);
		typedef CFunction<EventArgsHandlerFunc> HandlerFunction;
		typedef CArray<HandlerFunction> HandlerFunctionList;
		typedef CEventArgs< typename stEventArgDataType<_Datas>::Type... > EventArgsType;

		void Regitster(const HandlerFunction& func)
		{
			m_handerList.PushBack(func);
		}

		CEventRetuirnBase& TriggerEvent(const byte* buf, size_t s)
		{
			CByteStream stream(buf, s, s);
			EventArgsType args;
			args.ParseFrom(stream);
			return EventFuncCall(typename EventArgsType::GetArgIndexType(), args);
		}

		CEventRetuirnBase& TriggerEvent(CEventArgsBase* pArgs)
		{
			if (pArgs == NULL)
			{
				return m_return;
			}
			EventArgsType* pDataArgs = dynamic_cast<EventArgsType*>(pArgs);
			if (pDataArgs == NULL)
			{
				CByteStream stream;
				pArgs->Serialize(stream);
				EventArgsType args;
				args.ParseFrom(stream);
				return EventFuncCall(typename EventArgsType::GetArgIndexType(), args);
			}

			return EventFuncCall(typename EventArgsType::GetArgIndexType(), *pDataArgs);
		}
	private:

		template<size_t..._Indexs>
		CEventRetuirn<RetType>& EventFuncCall(const CArgIndex<_Indexs...>&, EventArgsType& args)
		{
			return Invoke(stEventArgsGetData<_Datas>::Invoke(args.GetData<_Indexs>())...);
		}

		CEventRetuirn<RetType>& Invoke(_Datas... args)
		{
			typename HandlerFunctionList::Iterator it = handerList.GetBegin();
			for (; it != handerList.GetEnd(); ++it)
			{
				(*it)(args...);
			}
			return m_return;
		}

		HandlerFunctionList m_handerList;
		CEventRetuirn<RetType> m_return;
	};

	template< typename _Ret, typename... _Datas >
	class ZS_API CEventHandler<_Ret(_Datas...)> : public CEventHandlerBase
	{
	public:
		typedef _Ret RetType;
		typedef _Ret EventArgsHandlerFunc(_Datas...);
		typedef CFunction<EventArgsHandlerFunc> HandlerFunction;
		typedef CArray<HandlerFunction> HandlerFunctionList;
		typedef CEventArgs< typename stEventArgDataType<_Datas>::Type... > EventArgsType;

		void Regitster(const HandlerFunction& func)
		{
			m_handerList.PushBack(func);
		}

		CEventRetuirnBase& TriggerEvent(const byte* buf, size_t s)
		{
			CByteStream stream(buf, s, s);
			EventArgsType args;
			args.ParseFrom(stream);
			return EventFuncCall(typename EventArgsType::GetArgIndexType(), args);
		}

		CEventRetuirnBase& TriggerEvent(CEventArgsBase* pArgs)
		{
			if (pArgs == NULL)
			{
				return m_return;
			}
			EventArgsType* pDataArgs = dynamic_cast<EventArgsType*>(pArgs);
			if (pDataArgs == NULL)
			{
				CByteStream stream;
				pArgs->Serialize(stream);
				EventArgsType args;
				args.ParseFrom(stream);
				return EventFuncCall(typename EventArgsType::GetArgIndexType(), args);
			}

			return EventFuncCall(typename EventArgsType::GetArgIndexType(), *pDataArgs);
		}
	private:

		template<size_t..._Indexs>
		CEventRetuirn<RetType>& EventFuncCall(const CArgIndex<_Indexs...>&, EventArgsType& args)
		{
			return Invoke(stEventArgsGetData<_Datas>::Invoke(args.GetData<_Indexs>())...);
		}

		CEventRetuirn<RetType>& Invoke(_Datas... args)
		{
			typename HandlerFunctionList::Iterator it = handerList.GetBegin();
			for (; it != handerList.GetEnd(); ++it)
			{
				m_return.Push((*it)(args...));
			}
			return m_return;
		}

		HandlerFunctionList m_handerList;
		CEventRetuirn<RetType> m_return;
	};

	template< typename _Ret, typename... _Datas >
	class ZS_API CEventHandler<_Ret(*)(_Datas...)> : public CEventHandlerBase
	{
	public:
		typedef _Ret RetType;
		typedef _Ret EventArgsHandlerFunc(_Datas...);
		typedef CFunction<EventArgsHandlerFunc> HandlerFunction;
		typedef CArray<HandlerFunction> HandlerFunctionList;
		typedef CEventArgs< typename stEventArgDataType<_Datas>::Type... > EventArgsType;

		void Regitster(const HandlerFunction& func)
		{
			m_handerList.PushBack(func);
		}

		CEventRetuirnBase& TriggerEvent(const byte* buf, size_t s)
		{
			CByteStream stream(buf, s, s);
			EventArgsType args;
			args.ParseFrom(stream);
			return EventFuncCall(typename EventArgsType::GetArgIndexType(), args);
		}

		CEventRetuirnBase& TriggerEvent(CEventArgsBase* pArgs)
		{
			if (pArgs == NULL)
			{
				return m_return;
			}
			EventArgsType* pDataArgs = dynamic_cast<EventArgsType*>(pArgs);
			if (pDataArgs == NULL)
			{
				CByteStream stream;
				pArgs->Serialize(stream);
				EventArgsType args;
				args.ParseFrom(stream);
				return EventFuncCall(typename EventArgsType::GetArgIndexType(), args);
			}

			return EventFuncCall(typename EventArgsType::GetArgIndexType(), *pDataArgs);
		}
	private:

		template<size_t..._Indexs>
		CEventRetuirn<RetType>& EventFuncCall(const CArgIndex<_Indexs...>&, EventArgsType& args)
		{
			return Invoke(stEventArgsGetData<_Datas>::Invoke(args.GetData<_Indexs>())...);
		}

		CEventRetuirn<RetType>& Invoke(_Datas... args)
		{
			typename HandlerFunctionList::Iterator it = handerList.GetBegin();
			for (; it != handerList.GetEnd(); ++it)
			{
				m_return.Push((*it)(args...));
			}
			return m_return;
		}

		HandlerFunctionList m_handerList;
		CEventRetuirn<RetType> m_return;
	};


}


#endif // !_EVENT_HANDLER_H_

