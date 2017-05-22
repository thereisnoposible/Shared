#ifndef _EVENT_RETURN_H_
#define _EVENT_RETURN_H_
//#include "../Public.h"
//#include "../ByteStream/ByteStream.h"
//#include "../Templates/TypeTraits.h"
namespace ZeroSpace
{
	class CEventRetuirnBase
	{
	public:
		virtual ~CEventRetuirnBase() {}
		virtual size_t GetCount() { return 0; }
		virtual void ParseFrom(const CByteStream& buffer)
		{
		}
		virtual void Serialize(CByteStream& buffer) const
		{
		}

	private:
		//CArray<>
	};
	template < typename _Ret>
	class CEventRetuirn : public CEventRetuirnBase
	{
	public:
		typedef _Ret RetType;
		void Clear()
		{
			m_arr.Clear();
		}

		RetType& At(size_t index)
		{
			return m_arr.At(index);
		}

		void Push(const RetType& val)
		{
			m_arr.PushBack(val);
		}

		size_t GetCount() { return m_arr.GetCount(); }
	private:
		CArray<RetType> m_arr;
	};

	template <>
	class CEventRetuirn<void> : public CEventRetuirnBase
	{
	public:
		void Clear(){}

		void At(size_t index) {}

		void Push(void) {}
	};
}

#endif 
