#include "pch.h"

#include "LinuxApp.h"

#include <signal.h>

//----------------------------------------------------------------------------------
LinuxApp::LinuxApp(xstring appname,HINSTANCE pInstance)
: BaseApp(appname),_bQuit(false)
{
    //ctor
}

//----------------------------------------------------------------------------------
LinuxApp::~LinuxApp()
{
    //dtor
}

//----------------------------------------------------------------------------------
void LinuxApp::Run()
{
    BaseApp::Run();

    signal(SIGINT,&LinuxApp::onSigInt);

    //循环等待程序退出
    while(!_bQuit)
    {
        //休眠
        Sleep(10);
    }

    _bQuit = false;
}

//----------------------------------------------------------------------------------
void LinuxApp::onSigInt(int sig)
{
    dynamic_cast<LinuxApp*>(GetSingletonPtr())->Quit();
}

//----------------------------------------------------------------------------------
void LinuxApp::PrintMessage(MessageLevel level, const char* message, ...)
{
    //直接使用printf输出
    if(level <_msgLevel)
    {
        return;
    }

    va_list argptr;
	va_start(argptr,message);
    vprintf(message,argptr);
    va_end(argptr);
}

//----------------------------------------------------------------------------------
void LinuxApp::SetStatus(const xstring message)
{

}

//----------------------------------------------------------------------------------
bool  LinuxApp::initialize()
{
    return true;
}

//----------------------------------------------------------------------------------
void LinuxApp::destroy()
{

}


void LinuxApp::Quit()
{
    _bQuit = true;
}
