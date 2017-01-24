// DataBase.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Application.h"
#include "crashReport.h"
#include "crashReport.cpp"

#ifdef _DEBUG
#include "Visual Leak Detector/include/vld.h"
#endif // DEBUG

bool bExit = false;
void loop()
{
    while (!bExit)
    {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "exit")
        {
            bExit = true;
        }
    }
}

Application* getApplication()
{
    Application* p = new Application;
    return p;
}

void aaa(Application* app)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::chrono::steady_clock::time_point fLast = std::chrono::steady_clock::now();
    while (!bExit)
    {
        std::chrono::steady_clock::time_point fCurr = std::chrono::steady_clock::now();
        std::chrono::duration<double> usetime = std::chrono::duration_cast<std::chrono::duration<double>>(fCurr - fLast);
        //在这里控制帧率
        if (usetime.count() < 0.00000000001)
        {
            Sleep(1);
            continue;
        }

        __try
        {
            app->update(usetime.count());
        }
        __except (CrashHandler(GetExceptionInformation()))
        {

        }

        fLast = fCurr;
    }

    google::protobuf::ShutdownProtobufLibrary();
}

boost::thread* getThread()
{
    boost::thread* p = new boost::thread(&loop);
    return p;
}

int main()
{
    Application* app = nullptr;
    boost::thread* thr = nullptr;

    __try
    {
        app = getApplication();
        thr = getThread();
    }
    __except (CrashHandler(GetExceptionInformation()))
    {
        return 0;
    }

    __try
    {
        aaa(app);
    }
    __except (CrashHandler(GetExceptionInformation()))
    {

    }

    delete app;
    delete thr;

    return 0;
}