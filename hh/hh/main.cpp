#include "stdafx.h"
#include <iostream>
#include <memory>
#include "google/protobuf/stubs/common.h"

#include "Application.h"

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

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	Application app;
	app.run();
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
		app.update(usetime.count());
		fLast = fCurr;
	}

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}