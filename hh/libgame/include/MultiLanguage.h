/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		MULTILANGUAGE.H
** @brief		������ʵ��
**  
** Details	
**  
** @version	1.0
** @date		2014/8/6 14:56
**  
** �޶�˵����	����汾 
** 
****************************************************************************/

#pragma  once
#include "Singleton.h"

/**
* @brief		�����԰�
* @details		
* @date			2014/8/6 14:57
* @note			�� 
*/ 
class XClass MultiLanguage : public Singleton<MultiLanguage>
{

public:
	/**
	* @brief		���캯��	
	*/
	MultiLanguage();
	

	/**
	* @brief		��������	
	*/
	~MultiLanguage();


	/**
	* @brief		��ȡ��Ӧ����ı�
	* @param[in]	csKey[xstring] : key�ı�
	* @return		xstring : value
	* @remarks		
	*/
	const xstring ToLanguage(const xstring & csKey);


private:
	/**
	* @brief		����һ�����԰�
	* @param[in]	filename[xstring] : ���԰��ļ�
	* @return		void
	* @remarks		
	*/
	void loadLanguageFile( const xstring& filename );

protected:
	typedef xmap <xstring,xstring> LanguageMap;
	LanguageMap		m_LanguagePack;						/** ���԰����� */
};