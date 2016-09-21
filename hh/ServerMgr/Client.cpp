#include "stdafx.h"
#include "Client.h"
#include "../new/proto/protobuf/hello.pb.h"
#include "../new/proto/protobuf/tonghe.pb.h"
#include "../new/proto/protobuf/object.pb.h"
#include "Application.h"

Client::Client()
{
//	CreateAccount();
}

Client::~Client()
{

}

void Client::update()
{
	while (1)
	{
		NetService::getInstance().update();
		std::string cmd;
		std::getline(std::cin, cmd);
		if (!cmd.empty())
		{
			std::vector<std::string> sAstr;
			Helper::SplitString(cmd, " ", sAstr);
			cmd = sAstr[0];

			std::unordered_map<std::string, std::function<void(std::vector<std::string>&)>>::iterator it = player._funcMap.find(cmd);
			if (it != player._funcMap.end())
			{
				it->second(sAstr);
			}
		}
	}
}

