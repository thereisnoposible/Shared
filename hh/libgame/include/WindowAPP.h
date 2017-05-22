#pragma  once


#include "BaseApp.h"


#define MAX_PRINT_LINE	3000
#define MAX_LINE_LEN	200


class XClass WindowAPP :public BaseApp
{
public:
	WindowAPP(xstring name,HINSTANCE instance);

	virtual ~WindowAPP();

	virtual void Run();

	virtual void PrintMessage(MessageLevel level, const char* format, ...);

	virtual void SetStatus(const xstring message);

	const HINSTANCE &getInstance() const
	{
		return m_hInstance;
	}

	/**
	* @brief		��ʾ��־�ӿ�
	*/
	void DisplayLog();

protected:
	/**
	* \fn			createWnd
	* @brief		����������
	* @param[in]	
	* @return		bool : true = �ɹ���false = ʧ��
	* @remarks		
	*/
	bool  createWnd(int width,int height);

	

protected:
	HINSTANCE	m_hInstance;		/** Ӧ�ó����� */

	HWND	m_hMainWnd;				/** ������ */ 

	HWND	m_hPrintWind;			/** ��ӡ��Ϣ���� */ 

	HWND	m_hStatusWind;			/** ��ӡ��ǰ��ǰ�������� */ 

	typedef xvector<xstring> PrintMessageCollect;
	PrintMessageCollect m_PrintMessages;			/** ��ӡ�����б� */

	char	m_csPrintMsg[MAX_PRINT_LINE*MAX_LINE_LEN];		/** ��ӡ��Ϣ���� */

	xstring m_strContent;

	boost::mutex			m_logMutex;			/** ��־���Ᵽ������ */

	bool _haveLog;
};