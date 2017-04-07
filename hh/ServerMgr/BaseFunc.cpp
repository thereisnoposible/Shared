#include "stdafx.h"
#include "BaseFunc.h"
#include "Application.h"

#include "../new/proto/protobuf/login.pb.h"
#include "../new/proto/protobuf/player.pb.h"

Player::Player()
{
	registmessge();
	registfunc();
}

void Player::registmessge()
{
	Application::getInstance().GetServer().RegisterMessage(GM_CREATE_ACCOUNT_RESPONSE, boost::bind(&Player::CreateAccountResponse, this, _1));
    Application::getInstance().GetServer().RegisterMessage(GM_ACCOUNT_CHECK_RESPONSE, boost::bind(&Player::AccountCheckResponse, this, _1));
    Application::getInstance().GetServer().RegisterMessage(GM_CREATE_PLAYER_RESPONSE, boost::bind(&Player::CreatePlayerResponse, this, _1));
    Application::getInstance().GetServer().RegisterMessage(GM_LOGIN_RESPONSE, boost::bind(&Player::LoginGameResponse, this, _1));
}

//-------------------------------------------------------------------------------------------
void Player::AAAA(int,int,int)
{

}

//-------------------------------------------------------------------------------------------
void Player::registfunc()
{
	_funcMap.insert(std::make_pair("CreateAccount", boost::bind(&Player::CreateAccount, this, _1)));
	_funcMap.insert(std::make_pair("AccountCheck", boost::bind(&Player::AccountCheck, this, _1)));
	_funcMap.insert(std::make_pair("CreatePlayer", boost::bind(&Player::CreatePlayer, this, _1)));
	_funcMap.insert(std::make_pair("LoginGame", boost::bind(&Player::LoginGame, this, _1)));
	_funcMap.insert(std::make_pair("Move", boost::bind(&Player::Move, this, _1)));
}

//-------------------------------------------------------------------------------------------
void Player::CreateAccount(std::vector<std::string>& param)
{
	pm_createaccount request;
	request.set_username(param[1]);
	request.set_pwd(param[2]);
	Application::getInstance().GetServer().SendProtoBuf(GM_CREATE_ACCOUNT, request);
}

//-------------------------------------------------------------------------------------------
void Player::CreateAccountResponse(PackPtr& pPack)
{

}

//-------------------------------------------------------------------------------------------
void Player::AccountCheck(std::vector<std::string>& param)
{
	pm_account_check request;
	request.set_increament(0);
	request.set_username(param[1]);
	request.set_pwd(param[2]);
	Application::getInstance().GetServer().SendProtoBuf(GM_ACCOUNT_CHECK, request);	
}

//-------------------------------------------------------------------------------------------
void Player::AccountCheckResponse(PackPtr& pPack)
{
	pm_account_check_response response;
	CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	system("cls");
	if (response.result() == 0)
	{
		std::cout << "AccountCheckSuccess\n";
	}
	for (int i = 0; i < response.playerdata_size(); i++)
	{
		std::cout << response.playerdata(i).id() << "\n";
		std::cout << response.playerdata(i).name() << "\n";
	}
}

//-------------------------------------------------------------------------------------------
void Player::CreatePlayer(std::vector<std::string>& param)
{
	pm_create_player request;
	request.set_player_name(param[1]);
	Application::getInstance().GetServer().SendProtoBuf(GM_CREATE_PLAYER, request);
}

//-------------------------------------------------------------------------------------------
void Player::CreatePlayerResponse(PackPtr& pPack)
{
	pm_create_player_response response;
	CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	if (response.has_data())
	{
		std::cout << response.data().id() << "\n";
	}
}

//-------------------------------------------------------------------------------------------
void Player::LoginGame(std::vector<std::string>& param)
{
	pm_login_game request;
	request.set_id(Helper::StringToInt32(param[1]));
	Application::getInstance().GetServer().SendProtoBuf(GM_LOGIN, request);
}

//-------------------------------------------------------------------------------------------
void Player::LoginGameResponse(PackPtr& pPack)
{
	pm_login_game_response response;
	CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	if (response.has_data())
	{
		std::cout << response.data().id() << "\n";
	}

	if (response.has_idata())
	{
		std::cout << "dbid :" << response.idata().dbid() << "\n";
		std::cout << "type :" << response.idata().type() << "\n";
		std::cout << "bactive :" << response.idata().bactive() << "\n";
	}
}

//-------------------------------------------------------------------------------------------
void Player::Move(std::vector<std::string>& param)
{
	pm_login_game request;
	request.set_id(Helper::StringToInt32(param[1]));
	Application::getInstance().GetServer().SendProtoBuf(GM_REQUEST_MOVE, request);
}