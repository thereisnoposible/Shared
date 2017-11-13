#include "pch.h"
#include "Application.h"

#ifdef _DEBUG
#include "Visual Leak Detector/include/vld.h"
#endif // DEBUG


bool bExit = false;
bool bReload = false;
Application* getApplication()
{
	Application* p = &Application::GetInstance();
	return p;
}

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
		if (cmd == "rld")
		{
			bReload = true;
		}
	}
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
			if (bReload)
			{
				app->GetLuaEngine().Reload();
				bReload = false;
			}
			if (!app->Update())
				break;
		}
		__except (CrashHandler(GetExceptionInformation()))
		{

		}

		fLast = fCurr;
	}
}

boost::thread* getThread()
{
	boost::thread* p = new boost::thread(&loop);
	return p;
}

void Destroy(Application* p)
{
	Application::GetInstance().Destroy();
	Application::DestroyInstance();

	google::protobuf::ShutdownProtobufLibrary();
}


int main()
{
	Application* app = nullptr;
	boost::thread* thr = nullptr;

	__try
	{
		app = getApplication();
		app->Init();
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

	__try
	{
		Destroy(app);
		delete thr;
	}
	__except (CrashHandler(GetExceptionInformation()))
	{
		return 0;
	}

	return 0;
}
