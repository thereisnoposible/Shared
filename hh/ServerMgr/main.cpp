
#include "stdafx.h"

#include "Application.h"
#include "google/protobuf/stubs/common.h"

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	Application app;
	app.run();
	std::chrono::steady_clock::time_point fLast = std::chrono::steady_clock::now();
	while (1)
	{
		double p = Helper::GetRandom(0.1, 0.2);
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