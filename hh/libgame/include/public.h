/****************************************************************************
**
** Copyright (C) 2014, All right reserved.
**
** @file		PCH.H
** @brief		公共定义-宏
**
** Details
**
** @version	1.0
** @author		yuyi
** @date		2014/7/17 13:58
**
** 修订说明：	最初版本
**
****************************************************************************/


#pragma  once

#include "LogService.h"

#ifndef assertNotNullExit
#define assertNotNullExit(e)  if (!(e)) { return;}
#endif

#ifndef assertNotNullResult
#define assertNotNullResult(e,ret) if (!(e)) {return ret;} 
#endif

#ifndef assertTrueResult
#define assertTrueResult(e,ret) if (!(e)) {return ret;} 
#endif

#ifndef assertTrueExit
#define assertTrueExit(e) if (!(e)) {return;}
#endif

#define LOG_ERR(...) {LogService::GetSingleton().LogMessage("ERR: %s %s,%d", __FUNCTION__, __FILE__,__LINE__);}
#define LOG_ERR_SIMPLE() do {LogService::GetSingleton().LogMessage("ERR: %s %s,%d", __FUNCTION__, __FILE__,__LINE__);} while (0);
#define LOG_ERR_IF_TRUE(b,...) if((b)) {LogService::GetSingleton().LogMessage("ERR: %s %s,%d", __FUNCTION__, __FILE__,__LINE__);LogService::GetSingleton().LogMessage(__VA_ARGS__);}

#define CHECKERROR(b) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);}
#define CHECKERRORANDRETURN(b) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);return;}
#define CHECKERRORANDRETURNRESULT(b) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);return b;}
#define CHECKERRORANDCONTINUE(b) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);continue;}
#define CHECKERRORANDRETURNVALUE(b,v) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);return v;}

#define CHECKERRORPARAM(b,...) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);LogService::GetSingleton().LogMessage(__VA_ARGS__);}
#define CHECKERRORPARAMANDRETURNRESULT(b,...) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);LogService::GetSingleton().LogMessage(__VA_ARGS__);return b;}
#define CHECKERRORPARAMANDRETURNVALUE(b,v,...) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);LogService::GetSingleton().LogMessage(__VA_ARGS__);return v;}
#define CHECKERRORPARAMANDCONTINUE(b,...) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);LogService::GetSingleton().LogMessage(__VA_ARGS__);continue;}
#define CHECKERRORPARAMANDRETURN(b,...) if(!(b)) {LogService::GetSingleton().LogMessage("error:%s,%d",__FILE__,__LINE__);LogService::GetSingleton().LogMessage(__VA_ARGS__);return;}

#define  SafeDelete(p) if(p!=NULL){delete p; p=NULL;}
#define	 SafeDeleteArray(p) if(p!=NULL){delete []p;p=NULL;}

#define THROW(str) {xApplication::GetSingleton().Terminate();throw(str);}

#define ToInt32(v) static_cast<int32>(v)
#define ToInt64(v) static_cast<int64>(v)
#define ToFloat(v) static_cast<float>(v)


#define LANGUAGE(str) MultiLanguage::GetSingleton().ToLanguage(str)

#define NetServiceRegister(msgid,func) NetService::GetSingleton().RegisterMessage(msgid,boost::bind(&func,this,_1))
#define NetServiceRegisterRange(s_id,e_id,func) NetService::GetSingleton().RegisterMessageRange(s_id,e_id,boost::bind(&func,this,_1))
#define NetServiceSendProtoBuf NetService::GetSingleton().SendProtoBuf

#define TIME_LENGTH		20
#define ACCOUNT_LENGTH	20
#define NAME_LENGTH		22
#define PASSWORD_LENGTH	40
#define RANDSTR_LENGTH	8
#define MAIL_TITLE_LEN  31
#define MAIL_CONTENT_LEN 255
#define MAIL_ATTACH_COUNT 5
#define _MAXPATH_L_ 1000

#define STRINGIZE_HELPER(x) #x
#define STRINGIZE(x) STRINGIZE_HELPER(x)
#define WARNING(desc) message(__FILE__ "(" STRINGIZE(__LINE__) ") : Warning: " #desc)

#define DEFFUNC(type,member,name)                            \
public:                                                        \
    inline void set_##name(type name){ member = name; }            \
    inline type name() const { return member; }                \
public:

#define DEFFUNCREF(type,member,name)                            \
public:                                                        \
    inline void set_##name(const type& name){ member = name; }            \
    inline type& name() { return member; }                \
public:

template<class T>
void ToRepeat32(T *rp, const xstring &s)
{
    std::vector<xstring> v;
    Helper::SplitString(s, ",", v);

    for (int32 i = 0; i < (int32)v.size(); i++)
    {
        int32 data = Helper::StringToInt32(v[i]);
        rp->Add(data);
    }
}

template<class T>
void ToRepeat64(T *rp, const xstring &s)
{
    std::vector<xstring> v;
    Helper::SplitString(s, ",", v);

    for (int32 i = 0; i < (int32)v.size(); i++)
    {
        int64 data = Helper::StringToInt64(v[i]);
        rp->Add(data);
    }
}

//--------------------------------------------
#ifdef _WIN32

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif
//--------------------------------------------
