#pragma once
#include <vector>
#include <map>
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/wire_format.h"

#define xvector std::vector
#define xmap std::map

typedef unsigned int uint;
typedef std::string xstring;

#ifdef _MSC_VER
typedef __int8  int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#else
typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
#endif


namespace proto
{
    struct Data;
    typedef xvector<Data> proto_data;

    struct Data
    {
        Data();
        ~Data();
        proto_data* add_proto_data();
        proto_data* mutable_proto_data();

        int32 index;
        xstring type;
        xstring value;
        xvector<xstring> value_s;

        xstring proto_type;
        xvector<proto_data> proto_value_s;
    private:
        proto_data* proto_value;
        
    };

    int32 GetProtoSize(proto_data& ProtoData);
    void WriteMessageMaybeToArray(int32 index, proto_data& ProtoData, ::google::protobuf::io::CodedOutputStream& output);
    uint32 MakeTag(int32 field_number, ::google::protobuf::internal::WireFormatLite::WireType type);
    void WriteTag(int32 field_number, ::google::protobuf::internal::WireFormatLite::WireType type,
        ::google::protobuf::io::CodedOutputStream* output);

    uint8* SerializeWithCachedSizesToArray(proto_data& ProtoData, uint8* target);
    void SerializeWithCachedSizes(proto_data& ProtoData, ::google::protobuf::io::CodedOutputStream& output);

    bool MergePartialFromCodedStream(::google::protobuf::io::CodedInputStream* input,
        proto_data& proto, ::google::protobuf::UnknownFieldSet* _unknown_fields_);
    bool ParseFromArray(const void* data, int size, proto_data& proto);

    extern xmap<xstring, proto_data> _proto_data_mgr;
}