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
	* @brief		显示日志接口
	*/
	void DisplayLog();

protected:
	/**
	* \fn			createWnd
	* @brief		创建出窗口
	* @param[in]	
	* @return		bool : true = 成功，false = 失败
	* @remarks		
	*/
	bool  createWnd(int width,int height);

	

protected:
	HINSTANCE	m_hInstance;		/** 应用程序句柄 */

	HWND	m_hMainWnd;				/** 主窗口 */ 

	HWND	m_hPrintWind;			/** 打印信息窗口 */ 

	HWND	m_hStatusWind;			/** 打印当前在前人数窗口 */ 

	typedef xvector<xstring> PrintMessageCollect;
	PrintMessageCollect m_PrintMessages;			/** 打印内容列表 */

	char	m_csPrintMsg[MAX_PRINT_LINE*MAX_LINE_LEN];		/** 打印消息缓存 */

	xstring m_strContent;

	boost::mutex			m_logMutex;			/** 日志互斥保护对象 */

	bool _haveLog;
};