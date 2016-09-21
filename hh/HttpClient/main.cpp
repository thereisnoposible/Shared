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

void func(Client* client)
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
		client->fire(cmd, param);
		if (cmd == "exit")
		{
			bRun = false;
			break;
		}
	}

}

int main()
{
	Client client;
	std::thread pthread(func, &client);

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
		client.run(usetime.count());
		fLast = fCurr;
	}
		
	pthread.join();
	return 0;
}