#include <boost/asio.hpp>
#include "Client.h"
#include <thread>
#include "Helper.h"
#include "Helper.cpp"
#include <boost/format.hpp>

std::string chartohex(unsigned char p)
{
	char a[3] = { 0 };
	sprintf_s(a, "%X", p);
	return a;
}

bool bRun = true;
Client ios(std::string("ios"));
Client android(std::string("android"));
void func()
{
	while (1)
	{
		std::string cmd;
		std::string param;
		std::getline(std::cin, cmd);

		int pos = cmd.find(" ");
		if (pos != std::string::npos)
		{
			param = cmd.substr(pos + 1);
			cmd.erase(cmd.begin() + pos, cmd.end());
		}

		if (cmd == "as_login")
		{
			ios.fire(cmd, param);
			android.fire(cmd, param);
			continue;
		}

		std::string sub_param;
		pos = param.find(" ");
		if (pos != std::string::npos)
		{
			sub_param = param.substr(pos + 1);
			param.erase(param.begin() + pos, param.end());
		}

		if (cmd == "ios")
			ios.fire(param, sub_param);
		
		if (cmd == "android")
			android.fire(param, sub_param);
		if (cmd == "exit")
		{
			bRun = false;
			break;
		}
	}

}

int main()
{	
	std::thread pthread(func);

	std::chrono::steady_clock::time_point fLast = std::chrono::steady_clock::now();

	while (bRun)
	{
		std::chrono::steady_clock::time_point fCurr = std::chrono::steady_clock::now();
		std::chrono::duration<double> usetime = std::chrono::duration_cast<std::chrono::duration<double>>(fCurr - fLast);
		//在这里控制帧率
		if (usetime.count() < 0.00000000001)
		{
			Sleep(1);
			continue;
		}
		ios.run(usetime.count());
		android.run(usetime.count());
		fLast = fCurr;
	}
		
	pthread.join();
	return 0;
}