#ifndef _EVENT_ARG_H_
#define _EVENT_ARG_H_
#include "EventDefine.h"
//#include "../ByteStream/ByteStream.h"
//#include "../Templates/TypeTraits.h"
//#include "../SmartPtr/SharedPtr.h"
#include "EventArgData.h"
//#include "../Templates/Tuple.h"
namespace ZeroSpace
{
	template <typename... _Types >
	class CEventArgs : public CEventArgsBase
	{
		template<typename _Type>friend class CEventHandler;
	public:
		typedef CEventArgs<_Types...> SeifType;
		typedef CEventArgsBase SuperType;
		typedef std::tuple< typename stEventArgDataType<_Types>::Type... > DataTableType;
		typedef typename CArgIndexList<sizeof...(_Types)>::IndexType GetArgIndexType;

		CEventArgs()
		{}

		CEventArgs(size_t eventID, const _Types&... datas):CEventArgsBase(eventID), m_datas(datas...)
		{}

		template <size_t _Index>
		typename stTupleElement<_Index, DataTableType>::ValType& GetData()
		{
			return m_datas.Get<_Index>();
		}

		template <size_t _Index>
		const typename stTupleElement<_Index, DataTableType>::ValType& GetData() const
		{
			return m_datas.Get<_Index>();
		}

		void ParseFrom(const CByteStream& stream)
		{
			SuperType::ParseFrom(stream);
			ParseFromX(GetArgIndexType(), stream);
		}

		void Serialize(CByteStream& stream) const
		{
			SuperType::Serialize(stream);
			SerializeX(GetArgIndexType(), stream);
		}
	private:
		template<size_t ..._Indexs>
		void ParseFromX(CArgIndex<_Indexs...>& , const CByteStream& stream)
		{
			int32 arr[] = { 0, ByteParseFrom(stream, m_datas.Get<_Indexs>())..., 0};
		}

		template<size_t ..._Indexs>
		void SerializeX(CArgIndex<_Indexs...>&, CByteStream& stream) const
		{
			int32 arr[] = { 0, ByteSerialize(stream, m_datas.Get<_Indexs>())..., 0 };
		}

		DataTableType m_datas;
	};

	
}

#endif
