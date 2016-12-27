#include "pch.h"
#include "proto_data.h"
#include "Helper.h"

#include <fstream>

//#include "google/protobuf/io/zero_copy_stream_impl.cc"
//#include "google/protobuf/wire_format_lite.cc"
//#include "google/protobuf/wire_format.cc"

namespace proto
{
    Data::Data() :proto_value(NULL)
    {

    }
    Data::~Data()
    {
        if (proto_value != nullptr)
        {
            delete proto_value;
            proto_value = nullptr;
        }
    }
    proto_data* Data::add_proto_data()
    {
        proto_data value;
        xmap<xstring, proto_data>::iterator it = _proto_data_mgr.find(proto_type);
        if (it != _proto_data_mgr.end())
        {
            value = it->second;
        }
        proto_value_s.push_back(value);

        return &proto_value_s[proto_value_s.size() - 1];
    }

    proto_data* Data::mutable_proto_data()
    {
        if (proto_value == nullptr)
        {
            xmap<xstring, proto_data>::iterator it = _proto_data_mgr.find(proto_type);
            if (it != _proto_data_mgr.end())
            {
                proto_value = new proto_data;
                *proto_value = it->second;
            }
        }
        return proto_value;
    }

    int32 GetProtoSize(proto_data& ProtoData)
    {
        int32 total_size = 0;
        for (int32 i = 0; i < ProtoData.size(); i++)
        {
            Data& temp = ProtoData[i];
            int32 off = i / 16 + 1;
            if (temp.type == "int32")
            {
                total_size += off + ::google::protobuf::internal::WireFormatLite::Int32Size(
                    Helper::StringToInt32(temp.value));
            }

            if (temp.type == "string")
            {
                total_size += off + ::google::protobuf::internal::WireFormatLite::StringSize(
                    temp.value);
            }

            if (temp.type == "int64")
            {
                total_size += off + ::google::protobuf::internal::WireFormatLite::Int64Size(
                    Helper::StringToInt64(temp.value));
            }

            if (temp.type == "bool")
            {
                total_size += off + 1;
            }

            if (temp.type == "int32_s")
            {
                total_size += off * temp.value_s.size();

                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    total_size += ::google::protobuf::internal::WireFormatLite::Int64Size(
                        Helper::StringToInt32(temp.value_s[zed]));
                }
            }

            if (temp.type == "string_s")
            {
                total_size += off * temp.value_s.size();

                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
                        temp.value_s[zed]);
                }
            }

            if (temp.type == "int64_s")
            {
                total_size += off * temp.value_s.size();

                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    total_size += ::google::protobuf::internal::WireFormatLite::Int64Size(
                        Helper::StringToInt64(temp.value));
                }
            }

            if (temp.type == "bool_s")
            {
                total_size += off * temp.value_s.size();

                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    total_size += 1;
                }
            }

            if (temp.type == "proto")
            {
                total_size += 1;

                total_size += GetProtoSize(*temp.mutable_proto_data());
            }

            if (temp.type == "proto_s")
            {
                total_size += temp.proto_value_s.size();

                for (int32 zed = 0; zed < temp.proto_value_s.size(); zed)
                {
                    total_size += GetProtoSize(temp.proto_value_s[zed]);
                }

            }
        }

        return total_size;
    }

    void WriteMessageMaybeToArray(int32 index, proto_data& ProtoData, ::google::protobuf::io::CodedOutputStream& output)
    {
        WriteTag(index, ::google::protobuf::internal::WireFormatLite::WireType::WIRETYPE_LENGTH_DELIMITED, &output);
        const int size = GetProtoSize(ProtoData);
        output.WriteVarint32(size);
        uint8* target = output.GetDirectBufferForNBytesAndAdvance(size);
        if (target != NULL) {
            uint8* end = SerializeWithCachedSizesToArray(ProtoData, target);
            GOOGLE_DCHECK_EQ(end - target, size);
        }
        else {
            SerializeWithCachedSizes(ProtoData, output);
        }
    }

    uint32 MakeTag(int32 field_number, ::google::protobuf::internal::WireFormatLite::WireType type) {
        return GOOGLE_PROTOBUF_WIRE_FORMAT_MAKE_TAG(field_number, type);
    }

    void WriteTag(int32 field_number, ::google::protobuf::internal::WireFormatLite::WireType type,
        ::google::protobuf::io::CodedOutputStream* output) {
        output->WriteTag(MakeTag(field_number, type));
    }

    uint8* SerializeWithCachedSizesToArray(proto_data& ProtoData, uint8* target)
    {
        for (int32 i = 0; i < ProtoData.size(); i++)
        {
            Data& temp = ProtoData[i];
            if (temp.type == "int32")
            {
                target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(temp.index, Helper::StringToInt32(temp.value), target);
            }

            if (temp.type == "int64")
            {
                target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(temp.index, Helper::StringToInt64(temp.value), target);
            }

            if (temp.type == "string")
            {
                target = ::google::protobuf::internal::WireFormatLite::WriteStringToArray(temp.index, temp.value, target);
            }

            if (temp.type == "proto")
            {
                target = SerializeWithCachedSizesToArray(*temp.mutable_proto_data(), target);
            }

            if (temp.type == "int32_s")
            {
                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(
                        temp.index, Helper::StringToInt32(temp.value_s[zed]), target);
                }
            }

            if (temp.type == "int64_s")
            {
                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(
                        temp.index, Helper::StringToInt32(temp.value_s[zed]), target);
                }
            }

            if (temp.type == "string_s")
            {
                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    target = ::google::protobuf::internal::WireFormatLite::WriteStringToArray(temp.index, temp.value_s[zed], target);
                }
            }

            if (temp.type == "proto_s")
            {
                for (int32 zed = 0; zed < temp.proto_value_s.size(); zed++)
                {
                    target = SerializeWithCachedSizesToArray(temp.proto_value_s[zed], target);
                }
            }
        }

        return target;
    }

    void SerializeWithCachedSizes(proto_data& ProtoData, ::google::protobuf::io::CodedOutputStream& output)
    {
        for (int32 i = 0; i < ProtoData.size(); i++)
        {
            Data& temp = ProtoData[i];
            if (temp.type == "int32")
            {
                ::google::protobuf::internal::WireFormatLite::WriteInt32(temp.index, Helper::StringToInt32(temp.value), &output);
            }

            if (temp.type == "int64")
            {
                ::google::protobuf::internal::WireFormatLite::WriteInt32(temp.index, Helper::StringToInt64(temp.value), &output);
            }

            if (temp.type == "string")
            {
                ::google::protobuf::internal::WireFormatLite::WriteString(temp.index, temp.value, &output);
            }


            if (temp.type == "proto")
            {
                WriteMessageMaybeToArray(temp.index, *temp.mutable_proto_data(), output);
            }

            if (temp.type == "int32_s")
            {
                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    ::google::protobuf::internal::WireFormatLite::WriteInt32(
                        temp.index, Helper::StringToInt32(temp.value_s[zed]), &output);
                }
            }

            if (temp.type == "int64_s")
            {
                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    ::google::protobuf::internal::WireFormatLite::WriteInt64(
                        temp.index, Helper::StringToInt32(temp.value_s[zed]), &output);
                }
            }

            if (temp.type == "string_s")
            {
                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    ::google::protobuf::internal::WireFormatLite::WriteString(temp.index, temp.value_s[zed], &output);
                }
            }

            if (temp.type == "proto_s")
            {
                for (int32 zed = 0; zed < temp.value_s.size(); zed++)
                {
                    WriteMessageMaybeToArray(temp.index, temp.proto_value_s[zed], output);
                }
            }
        }
    }

    bool MergePartialFromCodedStream(::google::protobuf::io::CodedInputStream* input, 
        proto_data& proto, ::google::protobuf::UnknownFieldSet* _unknown_fields_)
    {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
        ::google::protobuf::uint32 tag;
        while ((tag = input->ReadTag()) != 0)
        {
            int32 index = ::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag);
            for (int32 i = 0; i < proto.size(); i++)
            {
                Data& temp = proto[i];
                if (temp.index != index)
                    continue;

                if (temp.type == "int32")
                {
                    int32 id_;
                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                            ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                            input, &id_)));
                        temp.value = Helper::Int32ToString(id_);
                    }
                    else {
                        goto handle_uninterpreted;
                    }

                    break;
                }

                if (temp.type == "int64")
                {
                    int64 id_;
                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                            ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                            input, &id_)));
                        temp.value = Helper::Int64ToString(id_);
                    }
                    else {
                        goto handle_uninterpreted;
                    }

                    break;
                }

                if (temp.type == "string")
                {
                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
                        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                            input, &temp.value));
                        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
                            temp.value.data(), temp.value.length(),
                            ::google::protobuf::internal::WireFormat::PARSE);
                    }
                    else {
                        goto handle_uninterpreted;
                    }

                    break;
                }

                if (temp.type == "bool")
                {
                    bool is_look_leave_message_;
                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                            bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                            input, &is_look_leave_message_)));
                        temp.value = is_look_leave_message_ ? "1" : "0";
                    }
                    else {
                        goto handle_uninterpreted;
                    }
                    break;
                }

                if (temp.type == "proto")
                {
                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
                        MergePartialFromCodedStream(input, *temp.mutable_proto_data(), _unknown_fields_);
                    }
                    else {
                        goto handle_uninterpreted;
                    }
                    if (input->ExpectAtEnd()) return true;
                    break;
                }

                if (temp.type == "int32_s")
                {
                    int32 id_;
                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                            ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                            input, &id_)));
                        temp.value_s.push_back(Helper::Int32ToString(id_));
                    }
                    else {
                        goto handle_uninterpreted;
                    }

                    break;
                }

                if (temp.type == "int64_s")
                {
                    int64 id_;
                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                            ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                            input, &id_)));
                        temp.value_s.push_back(Helper::Int64ToString(id_));
                    }
                    else {
                        goto handle_uninterpreted;
                    }

                    break;
                }

                if (temp.type == "string_s")
                {
                    xstring value_;
                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
                        
                        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                            input, &value_));
                        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
                            value_.data(), value_.length(),
                            ::google::protobuf::internal::WireFormat::PARSE);
                        temp.value_s.push_back(value_);
                    }
                    else {
                        goto handle_uninterpreted;
                    }

                    break;
                }

                if (temp.type == "bool_s")
                {
                    bool is_look_leave_message_;
                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
                        DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                            bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                            input, &is_look_leave_message_)));
                        temp.value_s.push_back(is_look_leave_message_ ? "1" : "0");
                    }
                    else {
                        goto handle_uninterpreted;
                    }
                    break;
                }

                if (temp.type == "proto_s")
                {
                    // repeated .pm_activity_item info = 3;

                    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                        ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
                        uint32 length;
                        if (!input->ReadVarint32(&length)) return false;
                        if (!input->IncrementRecursionDepth()) return false;
                        ::google::protobuf::io::CodedInputStream::Limit limit = input->PushLimit(length);
                        if (!MergePartialFromCodedStream(input, *temp.add_proto_data(), _unknown_fields_))
                            return false;
                        // Make sure that parsing stopped when the limit was hit, not at an endgroup
                        // tag.
                        if (!input->ConsumedEntireMessage()) return false;
                        input->PopLimit(limit);
                        input->DecrementRecursionDepth();
                    }
                    else {
                        goto handle_uninterpreted;
                    }
                    break;
                }

            handle_uninterpreted:
                if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
                    ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
                    return true;
                }
                DO_(::google::protobuf::internal::WireFormat::SkipField(
                    input, tag, _unknown_fields_));
                break;
            }
        }

        return true;
    }

    bool ParseFromArray(const void* data, int size, proto_data& proto)
    {
        ::google::protobuf::io::CodedInputStream input(reinterpret_cast<const uint8*>(data), size);
        ::google::protobuf::UnknownFieldSet _unknown_fields_;
        return MergePartialFromCodedStream(&input, proto, &_unknown_fields_) &&
            input.ConsumedEntireMessage();
    }

    xmap<xstring, proto_data> _proto_data_mgr;

    void auto_load_proto(string path, xstring Wild, vector<string>& files)
    {
#ifdef _WINDOWS_
        //文件句柄 
        long  hFile = 0;
        //文件信息 
        struct _finddata_t fileinfo;
        string p;

        if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
        {
            do
            {
                if ((fileinfo.attrib & _A_SUBDIR))
                {
                    if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    {
                        auto_load_proto(p.assign(path).append("\\").append(fileinfo.name), Wild, files);
                    }
                }
            } while (_findnext(hFile, &fileinfo) == 0);
            _findclose(hFile);
        }

        if ((hFile = _findfirst(p.assign(path).append("\\").append(Wild).c_str(), &fileinfo)) != -1)
        {
            do
            {
                if (!(fileinfo.attrib & _A_SUBDIR))
                {
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                }
            } while (_findnext(hFile, &fileinfo) == 0);
            _findclose(hFile);
        }
#endif // _WINDOWS_

    }

    void SplitString(const std::string&str, const std::string&spl, std::vector<std::string>&sAstr)
    {
        if (str.empty())
            return;
        int pos = 0;
        pos = str.find(spl, pos);
        if (pos != std::string::npos)
        {
            if (pos != 0)
                sAstr.push_back(std::string().assign(str.begin(), str.begin() + pos));
            SplitString(str.substr(pos + spl.size()), spl, sAstr);
        }
        else
            sAstr.push_back(str);
    }

    void SplitString(const std::string&str, const std::vector<std::string>&spl, std::vector<std::string>&sAstr)
    {
        if (str.empty())
            return;
        int minpos = std::string::npos;
        int pos = 0;
        int vecpos = 0;
        for (int i = 0; i < (int)spl.size(); i++)
        {
            pos = str.find(spl[i], 0);
            if (pos != std::string::npos)
            {
                if (minpos == std::string::npos)
                {
                    minpos = pos;
                    vecpos = i;
                }
                else
                {
                    if (minpos > pos)
                    {
                        minpos = pos;
                        vecpos = i;
                    }
                }
            }
        }
        if (minpos != std::string::npos)
        {
            if (minpos != 0)
                sAstr.push_back(std::string().assign(str.begin(), str.begin() + minpos));
            SplitString(str.c_str() + minpos + spl[vecpos].length(), spl, sAstr);
        }
        else
            sAstr.push_back(str);
    }

    void erase_something(xstring& src, xstring erase)
    {
        if (erase.empty())
            return;

        int32 pos = src.find(erase);
        while (pos != xstring::npos)
        {
            src.erase(pos, erase.size());
            pos = src.find(erase);
        }
    }

    void load_proto(xstring& filename)
    {
        ifstream file;
        file.open(filename.c_str());
        xstring str;
        if (file && !file.eof())
        {
            file.seekg(0, ios::end);
            int32 num = file.tellg();
            file.seekg(0, ios::beg);
            str.resize(num);
            if (num != 0)
                file.read(&*str.begin(), num);
        }
        file.close();

        int32 notepos = str.find("//");
        while (notepos != xstring::npos)
        {
            int32 npos = str.find("\n", notepos);
            if (npos != xstring::npos)
            {
                str.erase(str.begin() + notepos, str.begin() + npos + 1);
            }
            else
                str.erase(str.begin() + notepos, str.end());

            notepos = str.find("//");
        }

        int32 pos = str.find("message");
        if (pos != xstring::npos)
        {
            str.erase(0, pos);
        }

        xvector<xstring> message;
        SplitString(str, "}", message);

        for (int32 i = 0; i < message.size(); i++)
        {
            int32 messpos = message[i].find("message ");
            if (messpos == xstring::npos)
                continue;
            message[i].erase(0, messpos + sizeof("message ") - 1);
            xvector<xstring> split;
            split.push_back("{");
            xvector<xstring> mess_proto;
            SplitString(message[i], split, mess_proto);

            xstring& proto_name = mess_proto[0];
            xstring& proto_field = mess_proto[1];

            erase_something(proto_name, "\n");
            erase_something(proto_name, " ");
            erase_something(proto_name, "\t");

            erase_something(proto_field, "\t");
            erase_something(proto_field, "\n");

            xvector<xstring> vec_filed;
            SplitString(proto_field, ";", vec_filed);

            proto_data data;

            for (int32 zed = 0; zed < vec_filed.size(); zed++)
            {
                xvector<xstring> vec_row;
                SplitString(vec_filed[zed], "=", vec_row);
                if (vec_row.size() != 2)
                    continue;;

                xstring& left_row = vec_row[0];
                xstring& right_row = vec_row[1];
                erase_something(right_row, " ");

                Data temp;
                temp.index = Helper::StringToInt32(right_row);

                xvector<xstring> left_data;
                Helper::SplitString(left_row, " ", left_data);
                temp.type = left_data[1];
                if (temp.type != "int32" && temp.type != "int64" && temp.type != "string" && temp.type != "bool")
                {
                    temp.proto_type = temp.type;
                    temp.type = "proto";
                }

                if (left_data[0] == "repeated")
                    temp.type += "_s";
               
                data.push_back(temp);
            }

            _proto_data_mgr[proto_name] = data;
        }
    }

    struct auto_load
    {
        auto_load()
        {
            xvector<xstring> file;
            auto_load_proto("proto", "*.txt", file);

            for (int32 i = 0; i < file.size(); i++)
            {
                load_proto(file[i]);
            }
        }
    } auto_load_;
}