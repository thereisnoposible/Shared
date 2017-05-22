/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		MULTILANGUAGE.H
** @brief		多语言实现
**  
** Details	
**  
** @version	1.0
** @date		2014/8/6 14:56
**  
** 修订说明：	最初版本 
** 
****************************************************************************/

#pragma  once
#include "Singleton.h"

/**
* @brief		多语言包
* @details		
* @date			2014/8/6 14:57
* @note			无 
*/ 
class XClass MultiLanguage : public Singleton<MultiLanguage>
{

public:
	/**
	* @brief		构造函数	
	*/
	MultiLanguage();
	

	/**
	* @brief		析构函数	
	*/
	~MultiLanguage();


	/**
	* @brief		获取对应结果文本
	* @param[in]	csKey[xstring] : key文本
	* @return		xstring : value
	* @remarks		
	*/
	const xstring ToLanguage(const xstring & csKey);


private:
	/**
	* @brief		加载一个语言包
	* @param[in]	filename[xstring] : 语言包文件
	* @return		void
	* @remarks		
	*/
	void loadLanguageFile( const xstring& filename );

protected:
	typedef xmap <xstring,xstring> LanguageMap;
	LanguageMap		m_LanguagePack;						/** 语言包容器 */
};