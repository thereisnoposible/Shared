#ifndef _EVENT_ARG_DATA_H_
#define _EVENT_ARG_DATA_H_
#include "EventDefine.h"

//#include "../ByteStream/ByteStream.h"
//#include "../Templates/TypeTraits.h"
//#include "../SmartPtr/SharedPtr.h"
//#include "../SmartPtr/IntrusivePtr.h"
//#include "../Containers/Array.h"
//#include "../Containers/List.h"
//#include "../String/String.h"
#include <list>
#include <vector>
namespace ZeroSpace
{

	class CEventArgsBase
	{
	public:
		CEventArgsBase():m_eventID(0){}
		CEventArgsBase(size_t eventID) :m_eventID(eventID) {}

		virtual void ParseFrom(const CByteStream& stream)
		{
			stream >> m_eventID;
		}

		virtual void Serialize(CByteStream& stream) const
		{
			stream << m_eventID;
		}

		size_t GetEventID()
		{
			return m_eventID;
		}
		void SetEventID(size_t eventID)
		{
			m_eventID = eventID;
		}
	protected:
		size_t m_eventID;
	};

	////其他////////////////
	template< typename _Data >
	struct stEventArgDataType
	{
		typedef typename stTypeIf< stIsClass< _Data >::Value, _Data*, _Data >::Type Type;
	};

	template< typename _Data >
	struct stEventArgDataType< _Data& >
	{
		typedef typename stTypeIf< stIsClass< _Data >::Value, _Data*, _Data >::Type Type;
	};

	template< typename _Data >
	struct stEventArgDataType< const _Data& >
	{
		typedef typename stTypeIf< stIsClass< _Data >::Value, _Data*, _Data >::Type Type;
	};
	//指針////////////////
	template< typename _Data >
	struct stEventArgDataType< _Data* >
	{
		typedef _Data* Type;
	};

	template< typename _Data >
	struct stEventArgDataType< const _Data* >
	{
		typedef _Data* Type;
	};
	//智能指針//////////////
	template< typename _Data >
	struct stEventArgDataType< CSharedPtr< _Data > >
	{
		typedef CSharedPtr< _Data > Type;
	};

	template< typename _Data >
	struct stEventArgDataType< CIntrusivePtr< _Data > >
	{
		typedef CIntrusivePtr< _Data > Type;
	};
	//字符串/////////////////////
	template<>
	struct stEventArgDataType< ansichar* >
	{
		typedef AnsiString Type;
	};

	template<>
	struct stEventArgDataType< const ansichar* >
	{
		typedef AnsiString Type;
	};
	//兼容stl
	template< >
	struct stEventArgDataType<std::string>
	{
		typedef std::string Type;
	};
	//數組/////////////////
	template< typename _Data, size_t N>
	struct stEventArgDataType< _Data[N] >
	{
		typedef CArray<_Data, N > Type;
	};

	template< typename _Data>
	struct stEventArgDataType< CArray<_Data > >
	{
		typedef CArray<_Data > Type;
	};

	template< typename _Data, size_t N>
	struct stEventArgDataType< CArray<_Data, N > >
	{
		typedef CArray<_Data, N > Type;
	};
	//兼容stl
	template< typename _Data>
	struct stEventArgDataType< std::vector<_Data > >
	{
		typedef std::vector<_Data> Type;
	};
	//鏈錶/////////////////////////
	template< typename _Data>
	struct stEventArgDataType< CList<_Data> >
	{
		typedef CList<_Data> Type;
	};
	template< typename _Data>
	struct stEventArgDataType< std::list<_Data> >
	{
		typedef std::list<_Data> Type;
	};
	//Map//////////////////////////
	//字節流（Byte Stream）////////
	template< >
	struct stEventArgDataType< CByteStream >
	{
		typedef CByteStream& Type;
	};
	template< >
	struct stEventArgDataType< CByteStream& >
	{
		typedef CByteStream& Type;
	};

	/////////////////////////////////////////////////////////////////////
	template < typename _Data >
	struct stByteParseFrom
	{
		typedef _Data DataType;
		static void Invoke(const CByteStream& stream, DataType& data)
		{
			stream >> data;
		}
	};

	template < typename _Data >
	struct stByteParseFrom<_Data*>
	{
		typedef _Data* DataType;
		static void Invoke(const CByteStream& stream, DataType& data)
		{
			if (data == NULL)
			{
				data = new _Data();
			}
			stream >> (*data);
		}
	};

	template < typename _Data >
	struct stByteParseFrom< CSharedPtr<_Data> >
	{
		typedef CSharedPtr<_Data> DataType;
		static void Invoke(const CByteStream& stream, DataType& data)
		{
			if (data == NULL || data.GetUseCount() < 1)
			{
				data = new _Data();
			}

			stream >> (*data);
		}
	};

	template < typename _Data >
	struct stByteParseFrom<CIntrusivePtr<_Data> >
	{
		typedef const CIntrusivePtr<_Data> DataType;
		static void Invoke(const CByteStream& stream, DataType& data)
		{
			if (data == NULL)
			{
				stream >> _Data();
			}
			else
			{
				stream >> (*data);
			}
		}
	};

	template <>
	struct stByteParseFrom<CEventArgsBase>
	{
		typedef CEventArgsBase DataType;
		static void Invoke(const CByteStream& stream, DataType& data)
		{
			data.ParseFrom(stream);
		}
	};

	template <>
	struct stByteParseFrom<CEventArgsBase*>
	{
		typedef CEventArgsBase* DataType;
		static void Invoke(const CByteStream& stream, DataType data)
		{
			if (data == NULL)
			{
				return;
			}
			data->ParseFrom(stream);
		}
	};

	template < typename _Data >
	int32 ByteParseFrom(const CByteStream& stream, _Data& data)
	{
		stByteParseFrom<_Data>::Invoke(stream, data);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////


	template < typename _Data >
	struct stByteSerialize
	{
		typedef _Data DataType;
		static void Invoke(CByteStream& stream, DataType& data)
		{
			stream << data;
		}
	};

	template < typename _Data >
	struct stByteSerialize<_Data*>
	{
		typedef _Data* DataType;
		static void Invoke(CByteStream& stream, DataType& data)
		{
			if (data == NULL)
			{
				stream << _Data();
			}
			else
			{
				stream << (*data);
			}
		}
	};

	template < typename _Data >
	struct stByteSerialize<const _Data>
	{
		typedef const _Data DataType;
		static void Invoke(CByteStream& stream, DataType& data)
		{
			stream << data;
		}
	};

	template < typename _Data >
	struct stByteSerialize<const _Data*>
	{
		typedef const _Data* DataType;
		static void Invoke(CByteStream& stream, DataType& data)
		{
			if (data == NULL)
			{
				stream << _Data();
			}
			else
			{
				stream << (*data);
			}
		}
	};

	template < typename _Data >
	struct stByteSerialize< _Data* const>
	{
		typedef const _Data* DataType;
		static void Invoke(CByteStream& stream, DataType data)
		{
			if (data == NULL)
			{
				stream << _Data();
			}
			else
			{
				stream << (*data);
			}
		}
	};

	template <>
	struct stByteSerialize<CEventArgsBase>
	{
		typedef CEventArgsBase DataType;
		static void Invoke(CByteStream& stream, DataType& data)
		{
			data.Serialize(stream);
		}
	};

	template <>
	struct stByteSerialize<CEventArgsBase*>
	{
		typedef CEventArgsBase* DataType;
		static void Invoke(CByteStream& stream, DataType data)
		{
			if (data == NULL)
			{
				return;
			}
			data->Serialize(stream);
		}
	};

	template <>
	struct stByteSerialize<const CEventArgsBase>
	{
		typedef const CEventArgsBase DataType;
		static void Invoke(CByteStream& stream, DataType& data)
		{
			data.Serialize(stream);
		}
	};

	template <>
	struct stByteSerialize<const CEventArgsBase*>
	{
		typedef const CEventArgsBase* DataType;
		static void Invoke(CByteStream& stream, DataType data)
		{
			if (data == NULL)
			{
				return;
			}
			data->Serialize(stream);
		}
	};

	template <>
	struct stByteSerialize<CEventArgsBase* const>
	{
		typedef CEventArgsBase* const DataType;
		static void Invoke(CByteStream& stream, DataType data)
		{
			if (data == NULL)
			{
				return;
			}
			data->Serialize(stream);
		}
	};

	template < typename _Data >
	int32 ByteSerialize(CByteStream& stream, _Data& data)
	{
		stByteSerialize<_Data>::Invoke(stream, data);
		return 0;
	}


	//////////////////////////////////////////////////////////////////////////////
	//一般函数形参
	template< typename _RetData >
	struct stEventArgsGetData
	{
	public:
		typedef typename stRemoveReference< typename stRemoveConst<_RetData>::Type >::Type RetDataType;

		template< typename _Data >
		struct stGetData
		{
		public:
			typedef _Data DataType;
			static RetDataType& Invoke(DataType& data)
			{
				return (RetDataType&)data;
			}
		};

		template< typename _Data >
		struct stGetData<CSharedPtr<_Data> >
		{
		public:
			typedef _Data DataType;
			static RetDataType& Invoke(CSharedPtr<_Data>& pData)
			{
				return (RetDataType&)(*pData);
			}
		};

		template< typename _Data >
		struct stGetData<CIntrusivePtr<_Data> >
		{
		public:
			typedef _Data DataType;
			static RetDataType& Invoke(CIntrusivePtr<_Data>& pData)
			{
				return (RetDataType&)(*pData);
			}
		};

		template< typename _Data >
		struct stGetData<_Data* >
		{
		public:
			typedef _Data DataType;
			static RetDataType& Invoke(_Data* pData)
			{
				return (RetDataType&)(*pData);
			}
		};

		template < typename _DataX>
		static RetDataType& Invoke(_DataX& data)
		{
			return stGetData<_DataX>::Invoke(data);
		}
	};
	//指针
	template < typename _RetData>
	struct stEventArgsGetData<_RetData*>
	{
	public:
		typedef _RetData* RetDataType;

		template< typename _Data >
		struct stGetData
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(DataType& data)
			{
				return (RetDataType)(&data);
			}
		};

		template< typename _Data >
		struct stGetData<CSharedPtr<_Data> >
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(CSharedPtr<_Data>& pData)
			{
				return (RetDataType)(pData.GetPtr());
			}
		};

		template< typename _Data >
		struct stGetData<CIntrusivePtr<_Data> >
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(CIntrusivePtr<_Data>& pData)
			{
				return (RetDataType)(pData.GetPtr());
			}
		};

		template< typename _Data >
		struct stGetData<_Data* >
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(_Data* pData)
			{
				return (RetDataType)(pData);
			}
		};

		template < typename _DataX>
		static RetDataType Invoke(_DataX& data)
		{
			return stGetData<_DataX>::Invoke(data);
		}
	};
	//智能指针
	template < typename _RetData>
	struct stEventArgsGetData< CSharedPtr<_RetData> >
	{
	public:
	public:
		typedef  CSharedPtr<_RetData> RetDataType;

		template< typename _Data >
		struct stGetData
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(DataType& data)
			{
				CSharedPtr<_RetData> ptr = new _Data(data);
				return ptr;
			}
		};

		template< typename _Data >
		struct stGetData<CSharedPtr<_Data> >
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(CSharedPtr<_Data>& pData)
			{
				return pData;
			}
		};

		template< typename _Data >
		struct stGetData<CIntrusivePtr<_Data> >
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(CIntrusivePtr<_Data>& pData)
			{
				CSharedPtr<_RetData> ptr = new _Data(*pData);
				return ptr;
			}
		};

		template< typename _Data >
		struct stGetData<_Data* >
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(_Data* pData)
			{
				return pData;
			}
		};

		template < typename _DataX>
		static RetDataType Invoke(_DataX& data)
		{
			return stGetData<_DataX>::Invoke(data);
		}
	};

	template < typename _RetData>
	struct stEventArgsGetData< CIntrusivePtr<_RetData> >
	{
		typedef  CIntrusivePtr<_RetData> RetDataType;

		template< typename _Data >
		struct stGetData
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(DataType& data)
			{
				return new _Data(data);
			}
		};

		template< typename _Data >
		struct stGetData<CSharedPtr<_Data> >
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(CSharedPtr<_Data>& pData)
			{
				return pData;
			}
		};

		template< typename _Data >
		struct stGetData<CIntrusivePtr<_Data> >
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(CIntrusivePtr<_Data>& pData)
			{
				CSharedPtr<_RetData> ptr = new _Data(*pData);
				return ptr;
			}
		};

		template< typename _Data >
		struct stGetData<_Data* >
		{
		public:
			typedef _Data DataType;
			static RetDataType Invoke(_Data* pData)
			{
				return pData;
			}
		};

		template < typename _DataX>
		static RetDataType Invoke(_DataX& data)
		{
			return stGetData<_DataX>::Invoke(data);
		}


	};

	///////////////////////////////////////兼容stl////////////////
	//stl map
	//stl set
	//stl vector
	//stl string
	//stl 智能指针 c++11

	////////////////////////////////////////////////////////////////////


}
#endif
