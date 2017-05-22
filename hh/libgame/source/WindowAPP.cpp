#pragma once

#include "pch.h"
#include "helper.h"
//#include "xLogManager.h"


#ifdef WIN32
#include "WindowAPP.h"


LRESULT CALLBACK WndProc(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:

		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_USER + 100:
		{
			WindowAPP* pThis = reinterpret_cast<WindowAPP*>(wParam);
			pThis->DisplayLog();
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}







//-----------------------------------------------------------------------------------
WindowAPP::WindowAPP( xstring name,HINSTANCE instance)
	: BaseApp(name),m_hInstance(instance),m_hMainWnd(NULL),m_hPrintWind(NULL),m_hStatusWind(NULL)
{
	_haveLog = false;
}



//-----------------------------------------------------------------------------------
WindowAPP::~WindowAPP()
{

}


//-----------------------------------------------------------------------------------
void WindowAPP::Run()
{
	//启动帧驱动线程，并初始化定时器管理对象
	createWnd(640,800);

	BaseApp::Run();

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);

		DispatchMessage(&msg);
	}

	//调用quit,等待update线程退出
	quit();
}


//-----------------------------------------------------------------------------------
bool  WindowAPP::createWnd(int width,int height)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= _TEXT("gamewindow");
	wcex.hIconSm		= 0;

	RegisterClassEx(&wcex);

	m_hMainWnd = CreateWindow(wcex.lpszClassName, m_Name.c_str(), WS_OVERLAPPEDWINDOW,
		100, 100, 1018, 768,NULL, NULL, m_hInstance, NULL);

	if (!m_hMainWnd)
	{
		return false;
	}


	//创建一个多行消息框
	m_hPrintWind=CreateWindow( _TEXT("edit"), _TEXT(""),
		WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|WS_HSCROLL|
		ES_MULTILINE|ES_READONLY,
		0, 0, 1000, 700, m_hMainWnd, (HMENU)NULL, m_hInstance, NULL);

	m_hStatusWind=CreateWindow( _TEXT("edit"), _TEXT(""),
		WS_VISIBLE|WS_CHILD|WS_BORDER|ES_MULTILINE|ES_READONLY,
		0, 700, 1000, 50, m_hMainWnd, (HMENU)NULL, m_hInstance, NULL);


	ShowWindow(m_hStatusWind,1); 
	ShowWindow(m_hPrintWind,1);
	ShowWindow(m_hMainWnd, 1);
	UpdateWindow(m_hMainWnd);

	return true;
}

//-----------------------------------------------------------------------------------
void WindowAPP::PrintMessage(MessageLevel level, const char* format, ...)
{
	if (level < _msgLevel)
	{//低等级的日志不显示
		return;
	}

	xstring message = format;

	char szMsg[1024]={0};
	va_list argptr;
	va_start(argptr,format);
	vsnprintf_s(szMsg,1023,1023,format,argptr);
	message = szMsg;

	//加锁,设置标识
	boost::mutex::scoped_lock oLock(m_logMutex);

	xstring tmpMsg = Helper::Timet2String(time(NULL)) + "   " + message + "\r\n";
	m_PrintMessages.push_back(tmpMsg);
	int32 nPrintLine = (int32)m_PrintMessages.size();
	

	m_strContent = "";			//打印内容
	if(nPrintLine>MAX_PRINT_LINE)
	{
		//截断 1/3
		int32 nDelete = MAX_PRINT_LINE / 3;
		PrintMessageCollect tmpMessages;
		for(int32 i=nDelete; i<MAX_PRINT_LINE; i++)
		{//放在一个新的数组中
			tmpMessages.push_back(m_PrintMessages[i]);
		}

		//反向打印出tmpMessages
		for(int32 i=(int32)tmpMessages.size()-1; i>=0; i--)
		{
			m_strContent += tmpMessages[i];
		}

		m_PrintMessages = tmpMessages;
	}
	else
	{
		GetWindowText(m_hPrintWind,m_csPrintMsg,MAX_PRINT_LINE*MAX_LINE_LEN-1);
		m_strContent = m_csPrintMsg;
		m_strContent = tmpMsg + m_strContent;
	}

	SetWindowText(m_hPrintWind, m_strContent.c_str());

	oLock.unlock();
	
	//boost::mutex::scoped_lock oLock(m_logMutex);
	//if (!_haveLog)
	//{//之前没有log再输出
	//	::PostMessage(m_hMainWnd, WM_USER + 100, WPARAM(this), LPARAM(0));
	//}

	//_haveLog = true;

	//oLock.unlock();
}


//-----------------------------------------------------------------------------------
void WindowAPP::SetStatus( const xstring message )
{
	SetWindowText(m_hStatusWind,message.c_str());
}

//-------------------------------------------------------------------------------------------
void WindowAPP::DisplayLog()
{
	boost::mutex::scoped_lock oLock(m_logMutex);
	if (_haveLog)
	{
		_haveLog = false;

		SetWindowText(m_hPrintWind, m_strContent.c_str());
	}
}

#endif
