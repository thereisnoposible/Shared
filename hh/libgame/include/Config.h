/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		CONFIG.H
** @brief		�����ļ�����
**  
** Details	
**  
** @version	1.0
** @author		yuyi
** @date		2014/8/29 15:30
**  
** �޶�˵����	����汾 
** 
****************************************************************************/
#pragma once

#include "Singleton.h"
#include "inifile.h"


/**
* @brief		�����ļ�������
* @details		
* @author		yuyi
* @date			2014/8/29 15:30
* @note			�� 
*/ 
class XClass Config : public Singleton<Config>
{
public:
	Config(const xstring& filename);
	~Config(void);

public:
	/**
	* @brief		��ȡ���ͼ�ֵ
	* @details		
	* @param[in]	section : ��
	* @param[in]	key : ��
	* @param[in]	default : Ĭ��ֵ
	* @return		int32
	* @remarks		
	*/
	int32 GetIntValue(const xstring& section,const xstring& key,int32 defaultv=0);


	/**
	* @brief		��ȡ�����ͼ�ֵ
	* @details		
	* @param[in]	section : ��
	* @param[in]	key : ��
	* @param[in]	default : Ĭ��ֵ
	* @return		float
	* @remarks		
	*/
	float GetFloatValue(const xstring& section,const xstring& key,float defaultv=0.0f);


	/**
	* @brief		��ȡ�ַ�����ֵ
	* @details		
	* @param[in]	section : ��
	* @param[in]	key : ��
	* @param[in]	default : Ĭ��ֵ
	* @return		xstring
	* @remarks		
	*/
	xstring GetStringValue(const xstring& section,const xstring& key,const xstring& defaultv="");

	/**
	* @brief		��ȡURL·��
	* @details
	* @param[in]	section : ��
	* @param[in]	key : ��
	* @param[in]	default : Ĭ��ֵ
	* @return		xstring
	* @remarks
	*/
	xstring GetUrlPath(const xstring& section, const xstring& key, const xstring& defaultv = "");

private:
	CIniFileA m_IniFile;
};

