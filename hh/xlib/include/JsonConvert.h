#pragma once
#include "../public/proto/protoFiles/player.txt.pb.h"
#include "json/json.h"

template<typename _Type>
class JsonConvertImpl
{
public:
    static void SerializeProtoBuf(const _Type& val, Json::Value& json);
    static void DeserializeProtoBuf(Json::Value& json, _Type& val);
};

template<typename _Type>
void JsonConvertImpl<_Type>::DeserializeProtoBuf(Json::Value& json, _Type& val)
{
    const ::google::protobuf::Reflection* pReflection = NULL;
    const ::google::protobuf::Descriptor* pDescriptor = NULL;
    stack< ::google::protobuf::Message* > stackMessage;
    stack<Json::Value*> stackJson;
    int32 fieldCount = 0;
    ::google::protobuf::FieldDescriptor::Label label;
    const ::google::protobuf::FieldDescriptor* pField = NULL;
    ::google::protobuf::Message* pMessage = NULL;
    ::google::protobuf::Message* pTmpMessage = NULL;
    Json::Value* pJsonValue = NULL;
    stackJson.push(&json);
    stackMessage.push(&val);

    while (!stackJson.empty())
    {
        pMessage = stackMessage.top();
        pJsonValue = stackJson.top();
        stackMessage.pop();
        stackJson.pop();
        if (pMessage == NULL)
        {
            continue;
        }
        if (pJsonValue == NULL)
        {
            continue;
        }

        Json::Value& jsonValue = *pJsonValue;
        pReflection = pMessage->GetReflection();
        pDescriptor = pMessage->GetDescriptor();
        fieldCount = pDescriptor->field_count();

        int32 repeatedCount = 0;
        for (int32 i = 0; i < fieldCount; ++i)
        {
            pField = pDescriptor->field(i);
            label = pField->label();
            switch (label)
            {
            case ::google::protobuf::FieldDescriptor::LABEL_REPEATED:
                {
                    if (jsonValue.isObject() && jsonValue.isMember(pField->name()))
                    {
                        
                        Json::Value& val1 = jsonValue[pField->name()];
                        repeatedCount = val1.size();
                    
                        for (int32 j = 0; j < repeatedCount; ++j)
                        {
                            Json::Value& val = (val1[j]);
                            switch (pField->type())
                            {
                                case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                                {
                                    //::google::protobuf::Message& rMessage = const_cast<::google::protobuf::Message&>(pReflection->GetRepeatedMessage(*pMessage, pField, j));
                                    pTmpMessage = pReflection->AddMessage(pMessage, pField);
                                    stackMessage.push(pTmpMessage);
                                    stackJson.push(&val);
                                }break;
                                case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                                {
                                    if (val.isNumeric())
                                    {
                                        pReflection->AddDouble(pMessage, pField, val.asDouble());
                                    }
                                }break;
                                case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
                                {
                                    if (val.isNumeric())
                                    {
                                        pReflection->AddFloat(pMessage, pField, val.asFloat());
                                    }
                                }break;
                                case ::google::protobuf::FieldDescriptor::TYPE_INT32:
                                {
                                    if (val.isInt())
                                    {
                                        pReflection->AddInt32(pMessage, pField, val.asInt());
                                    }
                                }break;
                                case ::google::protobuf::FieldDescriptor::TYPE_INT64:
                                {
                                    if (val.isInt64())
                                    {
                                        pReflection->AddInt64(pMessage, pField, val.asInt64());
                                    }
                                }break;
                                case ::google::protobuf::FieldDescriptor::TYPE_UINT32:
                                {
                                    if (val.isUInt())
                                    {
                                        pReflection->AddUInt32(pMessage, pField, val.asUInt());
                                    }
                                }break;
                                case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
                                {
                                    if (val.isUInt64())
                                    {
                                        pReflection->AddUInt64(pMessage, pField, val.asUInt64());
                                    }
                                }break;
                                case ::google::protobuf::FieldDescriptor::TYPE_STRING:
                                {
                                    if (val.isString())
                                    {
                                        pReflection->AddString(pMessage, pField, val.asString());
                                    }
                                }break;
                                default:
                                    break;
                            }
                        }

                    }
                    
                }break;

            case ::google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
            case ::google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            {

                 if (jsonValue.isObject() && jsonValue.isMember(pField->name()))
                 {
                     Json::Value& val = jsonValue[pField->name()];
                     switch (pField->type())
                     {
                        case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                        {
                            pTmpMessage = pReflection->MutableMessage(pMessage, pField);
                            stackMessage.push(pTmpMessage);
                            stackJson.push(&val);
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                        {
                             if (val.isNumeric())
                             {
                                 pReflection->SetDouble(pMessage, pField, val.asDouble());
                             }
                         
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
                        {
                             if (val.isNumeric())
                             {
                                 pReflection->SetFloat(pMessage, pField, val.asFloat());
                             }
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_INT32:
                        {
                              if (val.isInt())
                              {
                                  pReflection->SetInt32(pMessage, pField, val.asInt());
                              }
                             
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_INT64:
                        {
                             if (val.isInt64())
                             {
                                 pReflection->SetInt64(pMessage, pField, val.asInt64());
                             }
                             
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_UINT32:
                        {
                             if (val.isUInt())
                             {
                                 pReflection->SetUInt32(pMessage, pField, val.asUInt());
                             }
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
                        {
                             if (val.isUInt64())
                             {
                                 pReflection->SetUInt64(pMessage, pField, val.asUInt64());
                             }
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_STRING:
                        {
                             if (val.isString())
                             {
                                 pReflection->SetString(pMessage, pField, val.asString());
                             }
                             
                        }break;
                        default:
                            break;
                     }
                 }
                 
            }break;
            default:
                break;
            }
        }
    }
}

template<typename _Type>
void JsonConvertImpl<_Type>::SerializeProtoBuf(const _Type& val, Json::Value& json)
{
    const ::google::protobuf::Reflection* pReflection = NULL;
    const ::google::protobuf::Descriptor* pDescriptor = NULL;
    stack< ::google::protobuf::Message* > stackMessage;
    stack<Json::Value*> stackJson;

    int32 fieldCount = 0;
    ::google::protobuf::FieldDescriptor::Label label;
    const ::google::protobuf::FieldDescriptor* pField = NULL;
    _Type& protoBuf = const_cast<_Type&>(val);
    ::google::protobuf::Message* pMessage = NULL;
    //const ::google::protobuf::Message* pMessage1 = NULL;
    Json::Value* pJsonValue = NULL;
    stackMessage.push(&protoBuf);
    stackJson.push(&json);

    int32 repeatedCount = 0;
    while (!stackMessage.empty())
    {
        pMessage = stackMessage.top();
        pJsonValue = stackJson.top();
        stackMessage.pop();
        stackJson.pop();
        if (pMessage == NULL)
        {
            continue;
        }
        if (pJsonValue == NULL)
        {
            continue;
        }

        Json::Value& jsonValue = *pJsonValue;
        pReflection = pMessage->GetReflection();
        pDescriptor = pMessage->GetDescriptor();
        fieldCount = pDescriptor->field_count();
        for (int32 i = 0; i < fieldCount; ++i)
        {
            pField = pDescriptor->field(i);
            label = pField->label();
            switch (label)
            {
            case ::google::protobuf::FieldDescriptor::LABEL_REPEATED:
                {
                    repeatedCount = pReflection->FieldSize(*pMessage, pField);
                    Json::Value& val1 = jsonValue[pField->name()];
                    for (int32 j = 0; j < repeatedCount; ++j)
                    {
                        Json::Value& val = (val1[j]);
                        switch (pField->type())
                        {
                        case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                        {
                            ::google::protobuf::Message& rMessage = const_cast<::google::protobuf::Message&>(pReflection->GetRepeatedMessage(*pMessage, pField, j));
                            stackMessage.push(&rMessage);
                            stackJson.push(&val);
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                        {
                            val = pReflection->GetRepeatedDouble(*pMessage, pField, j);
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
                        {
                            val = pReflection->GetRepeatedFloat(*pMessage, pField, j);
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_INT32:
                        {
                            val = pReflection->GetRepeatedInt32(*pMessage, pField, j);
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_INT64:
                        {
                            val = pReflection->GetRepeatedInt64(*pMessage, pField, j);
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_UINT32:
                        {
                            val = pReflection->GetRepeatedUInt32(*pMessage, pField, j);
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
                        {
                            val = pReflection->GetRepeatedUInt64(*pMessage, pField, j);
                        }break;
                        case ::google::protobuf::FieldDescriptor::TYPE_STRING:
                        {
                            val = pReflection->GetRepeatedString(*pMessage, pField, j);
                        }break;
                        default:
                            break;
                        }
                    }
                }break;

            case ::google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
            case ::google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            {
                if (pReflection->HasField(*pMessage, pField))
                {
                    Json::Value& val = jsonValue[pField->name()];
                    switch (pField->type())
                    {
                    case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                    {

                        ::google::protobuf::Message& rMessage = const_cast<::google::protobuf::Message&>(pReflection->GetMessage(*pMessage, pField));
                        stackMessage.push(&rMessage);
                        stackJson.push(&val);
                    }break;
                    case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                    {
                         val = pReflection->GetDouble(*pMessage, pField);
                    }break;
                    case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
                    {
                         val = pReflection->GetFloat(*pMessage, pField);
                    }break;
                    case ::google::protobuf::FieldDescriptor::TYPE_INT32:
                    {
                         val = pReflection->GetInt32(*pMessage, pField);
                    }break;
                    case ::google::protobuf::FieldDescriptor::TYPE_INT64:
                    {
                         val = pReflection->GetInt64(*pMessage, pField);
                    }break;
                    case ::google::protobuf::FieldDescriptor::TYPE_UINT32:
                    {
                         val = pReflection->GetUInt32(*pMessage, pField);
                    }break;
                    case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
                    {
                         val = pReflection->GetUInt64(*pMessage, pField);
                    }break;
                    case ::google::protobuf::FieldDescriptor::TYPE_STRING:
                    {
                         val = pReflection->GetString(*pMessage, pField);
                    }break;
                    default:
                        break;
                    }
                }
            }break;
            default:
                break;
            }

        }

    }
}

template<typename _Type>
class JsonConvertImpl<::google::protobuf::RepeatedField<_Type> >
{
public:
    static void SerializeProtoBuf(const ::google::protobuf::RepeatedField<_Type>& repeated, Json::Value& json)
    {
        int32 s = repeated.size();
        for (int32 i = 0; i < s; ++i)
        {
            JsonConvertImpl<_Type>::SerializeProtoBuf(repeated.Get(i), json[i]);
        }
    }

    static void DeserializeProtoBuf(Json::Value& json, ::google::protobuf::RepeatedField<_Type>& val)
    {
        int32 s = json.size();
        _Type* pVal = NULL;
        for (int32 i = 0; i < s; ++i)
        {
            pVal = val.Add();
            JsonConvertImpl<_Type>::DeserializeProtoBuf(json[i], *pVal);
        }
    }

};

template<typename _Type>
class JsonConvertImpl<::google::protobuf::RepeatedPtrField<_Type> >
{
public:
    static void SerializeProtoBuf(const ::google::protobuf::RepeatedPtrField<_Type>& repeated, Json::Value& json)
    {
        int32 s = repeated.size();
        for (int32 i = 0; i < s; ++i)
        {
            JsonConvertImpl<_Type>::SerializeProtoBuf(repeated.Get(i), json[i]);
        }
    }

    static void DeserializeProtoBuf(Json::Value& json, ::google::protobuf::RepeatedPtrField<_Type>& val)
    {
        int32 s = json.size();
        _Type* pVal = NULL;
        for (int32 i = 0; i < s; ++i)
        {
            pVal = val.Add();
            JsonConvertImpl<_Type>::DeserializeProtoBuf(json[i], *pVal);
        }
    }

};

template<>
class JsonConvertImpl<int32 >
{
public:
    static void SerializeProtoBuf(int32 val, Json::Value& json)
    {
        json = val;
    }

    static void DeserializeProtoBuf(Json::Value& json, int32& val)
    {
        if (!json.isInt())
        {
            return;
        }
        val = json.asInt();
    }
    
};

template<>
class JsonConvertImpl<int64 >
{
public:
    static void SerializeProtoBuf(int64 val, Json::Value& json)
    {
        json = val;
    }

    static void DeserializeProtoBuf(Json::Value& json, int64& val)
    {
        if (!json.isInt64())
        {
            return;
        }
        val = json.asInt64();
    }
};


template<>
class JsonConvertImpl<uint32 >
{
public:
    static void SerializeProtoBuf(uint32 val, Json::Value& json)
    {
        json = val;
    }

    static void DeserializeProtoBuf(Json::Value& json, uint32& val)
    {
        if (!json.isUInt())
        {
            return;
        }
        val = json.asUInt();
    }

};

template<>
class JsonConvertImpl<uint64 >
{
public:
    static void SerializeProtoBuf(uint64 val, Json::Value& json)
    {
        json = val;
    }

    static void DeserializeProtoBuf(Json::Value& json, uint64& val)
    {
        if (!json.isUInt64())
        {
            return;
        }
        val = json.asUInt64();
    }
};

template<>
class JsonConvertImpl<float >
{
public:
    static void SerializeProtoBuf(float val, Json::Value& json)
    {
        json = val;
    }

    static void DeserializeProtoBuf(Json::Value& json, float& val)
    {
        if (!json.isNumeric())
        {
            return;
        }
        val = json.asFloat();
    }
};

template<>
class JsonConvertImpl<double >
{
public:
    static void SerializeProtoBuf(double val, Json::Value& json)
    {
        json = val;
    }

    static void DeserializeProtoBuf(Json::Value& json, double& val)
    {
        if (!json.isNumeric())
        {
            return;
        }
        val = json.asDouble();
    }
};

template<>
class JsonConvertImpl<std::string >
{
public:
    static void SerializeProtoBuf(const std::string& val, Json::Value& json)
    {
        json = val;
    }

    static void DeserializeProtoBuf(Json::Value& json, std::string& val)
    {
        val = json.asString();
    }
};

class JsonConvert
{
public:
    //ProtoBuf转化到json
    template<typename _Type>
    static void ProtoBufToJson(const _Type& val, Json::Value& json)
    {
        JsonConvertImpl<_Type>::SerializeProtoBuf(val, json);
    }
    //json转化到ProtoBuf
    template<typename _Type>
    static void JsonToProtoBuf(Json::Value& json, _Type& val)
    {
        JsonConvertImpl<_Type>::DeserializeProtoBuf(json, val);
    }

    //ProtoBuf转化到json字符串
    template<typename _Type>
    static std::string ProtoBufToJsonString(const _Type& val)
    {
        Json::Value json;
        Json::FastWriter writer;
        JsonConvertImpl<_Type>::SerializeProtoBuf(val, json);

        if (json.empty())
        {
            return  "";
        }
        return writer.write(json);
    }

    //json字符串 转化到ProtoBuf
    template<typename _Type>
    static void JsonStringToProtoBuf(const std::string& str, _Type& val)
    {
        Json::Reader reader;
        Json::Value json;
        reader.parse(str, json, false);
        JsonConvertImpl<_Type>::DeserializeProtoBuf(json, val);
    }
};

