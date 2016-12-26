#include "stdafx.h"
#include <iostream>
#include <memory>
#include "google/protobuf/stubs/common.h"

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

void aaa()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Application app;

    std::chrono::steady_clock::time_point fLast = std::chrono::steady_clock::now();
    boost::thread thread(&loop);
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
            app.update(usetime.count());
        }
        __except (CrashHandler(GetExceptionInformation()))
        {

        }
       
        fLast = fCurr;
    }

    google::protobuf::ShutdownProtobufLibrary();
}

int main()
{
    __try
    {
        aaa();
    }
    __except (CrashHandler(GetExceptionInformation()))
    {
       
    }

	return 0;
}