/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		CONFIG.H
** @brief		配置文件管理
**  
** Details	
**  
** @version	1.0
** @author		yuyi
** @date		2014/8/29 15:30
**  
** 修订说明：	最初版本 
** 
****************************************************************************/
#pragma once

#include "Singleton.h"
#include "inifile.h"


/**
* @brief		配置文件管理类
* @details		
* @author		yuyi
* @date			2014/8/29 15:30
* @note			无 
*/ 
class XClass Config : public Singleton<Config>
{
public:
	Config(const xstring& filename);
	~Config(void);

public:
	/**
	* @brief		获取整型键值
	* @details		
	* @param[in]	section : 节
	* @param[in]	key : 键
	* @param[in]	default : 默认值
	* @return		int32
	* @remarks		
	*/
	int32 GetIntValue(const xstring& section,const xstring& key,int32 defaultv=0);


	/**
	* @brief		获取浮点型键值
	* @details		
	* @param[in]	section : 节
	* @param[in]	key : 键
	* @param[in]	default : 默认值
	* @return		float
	* @remarks		
	*/
	float GetFloatValue(const xstring& section,const xstring& key,float defaultv=0.0f);


	/**
	* @brief		获取字符串键值
	* @details		
	* @param[in]	section : 节
	* @param[in]	key : 键
	* @param[in]	default : 默认值
	* @return		xstring
	* @remarks		
	*/
	xstring GetStringValue(const xstring& section,const xstring& key,const xstring& defaultv="");

	/**
	* @brief		获取URL路径
	* @details
	* @param[in]	section : 节
	* @param[in]	key : 键
	* @param[in]	default : 默认值
	* @return		xstring
	* @remarks
	*/
	xstring GetUrlPath(const xstring& section, const xstring& key, const xstring& defaultv = "");

private:
	CIniFileA m_IniFile;
};

