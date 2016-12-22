#include "stdafx.h"
#include "PlayerMgr.h"
#include "Player.h"
#include "ModuleManager.h"
#include "MapManager.h"
#include "Application.h"
#include "../new/proto/protobuf/login.pb.h"
#include "../new/proto/protobuf/player.pb.h"

PlayerManager* Singleton<PlayerManager>::single = nullptr;
//-------------------------------------------------------------------------------------------
PlayerManager::PlayerManager(DBService* p) :m_pDBService(p),m_LoginGameIncreament(0)
{
	registmessage();
}

//-------------------------------------------------------------------------------------------
PlayerManager::~PlayerManager()
{
	std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator it = m_AccPlayer.begin();
	for (; it != m_AccPlayer.end(); ++it)
	{
		std::unordered_map<unsigned int, Player*>::iterator playerit = it->second.begin();
		for (; playerit != it->second.end(); ++playerit)
		{
			if (playerit->second->GetPlayerState() == Player::PlayerState::psOnline)
			{
				playerit->second->OnPlyaerLogout();
			}
			delete playerit->second;
		}
	}
}

//-------------------------------------------------------------------------------------------
void PlayerManager::registmessage()
{
	NetService::getInstance().RegisterMessage(GM_CREATE_ACCOUNT, boost::bind(&PlayerManager::CreateAccount, this, _1));
	NetService::getInstance().RegisterMessage(GM_CREATE_ACCOUNT_RESPONSE, boost::bind(&PlayerManager::CreateAccountResponse, this, _1));
	NetService::getInstance().RegisterMessage(GM_ACCOUNT_CHECK, boost::bind(&PlayerManager::AccountCheck, this, _1));
	NetService::getInstance().RegisterMessage(GM_ACCOUNT_CHECK_RESPONSE, boost::bind(&PlayerManager::AccountCheckResponse, this, _1));
	NetService::getInstance().RegisterMessage(GM_LOGIN, boost::bind(&PlayerManager::LoginGame, this, _1));
	NetService::getInstance().RegisterMessage(GM_CREATE_PLAYER, boost::bind(&PlayerManager::CreatePlayer, this, _1));
	NetService::getInstance().RegisterMessageRange(PLAYERMESSAGEBEGIN, PLAYERMESSAGEEND, boost::bind(&PlayerManager::onPlayerMessage, this, _1));
	
	NetService::getInstance().RegisterMessage(GM_HEARTBEAT_FROM_CLIENT, boost::bind(&PlayerManager::HeartBeat, this, _1));
}

//-------------------------------------------------------------------------------------------
void PlayerManager::unregistmessage()
{

}

//-------------------------------------------------------------------------------------------
void PlayerManager::init()
{

}

//-------------------------------------------------------------------------------------------
void PlayerManager::loadPlayer()
{
	_unique_player = 0;
	std::string sql = "select * from player";
	DBResult result;
	m_pDBService->syncQuery(std::move(sql), result);

	while (!result.eof())
	{
		PlayerData pData;
		pData.acc_id = result.getStringField("acc_id");

		pData.id = result.getIntField("id");
		pData.jinbi = result.getIntField("jinbi");

		pData.hp = result.getIntField("hp");
		pData.maxhp = result.getIntField("max_hp");

		pData.cellid = result.getIntField("cellid");
		pData.name = result.getStringField("name");

		if (pData.id > _unique_player)
			_unique_player = pData.id;

		std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator it = m_AccPlayer.find(pData.acc_id);
		if (it == m_AccPlayer.end())
		{
			m_AccPlayer.insert(std::make_pair(pData.acc_id, std::unordered_map<unsigned int, Player*>()));
			it = m_AccPlayer.find(pData.acc_id);
		}
		Player* p = new Player(pData);
		ModuleManager::getInstance().CreateRoleModule(p);
		p->SetPlayerState(Player::PlayerState::psOffline);
		it->second.insert(std::make_pair(pData.id, p));

		result.nextRow();
	}
	_unique_player += 1;
}

//-------------------------------------------------------------------------------------------
void PlayerManager::OnDisConnect(ConnectPtr& playerid)
{
	std::unordered_map<ConnectPtr, Session>::iterator sit = m_AllPlayer.find(playerid);
	if (sit != m_AllPlayer.end())
	{
		std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator accit = m_AccPlayer.find(sit->second.acc_id);
		if (accit != m_AccPlayer.end())
		{
			std::unordered_map<unsigned int, Player*>::iterator playerit = accit->second.find(sit->second.playerid);
			if (playerit != accit->second.end() && playerit->second->GetPlayerState() == Player::PlayerState::psOnline)
			{
				playerit->second->OnPlyaerLogout();
			}
		}
		m_AllPlayer.erase(sit);
	}	
}

//-------------------------------------------------------------------------------------------
void PlayerManager::onPlayerMessage(PackPtr& pPack)
{
	std::unordered_map<ConnectPtr, Session>::iterator it = m_AllPlayer.find(pPack->m_Connect);
	if (it != m_AllPlayer.end())
	{
		std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator accit = m_AccPlayer.find(it->second.acc_id);
		if (accit != m_AccPlayer.end())
		{
			std::unordered_map<unsigned int, Player*>::iterator playerit = accit->second.find(it->second.playerid);
			playerit->second->FireNetMsg(pPack->getMessageId(), pPack);
		}	
	}
}

//-------------------------------------------------------------------------------------------
void PlayerManager::CreateAccount(PackPtr& pPack)
{
	pm_createaccount request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	std::unordered_map<std::string, ConnectPtr>::iterator it = m_Creating.find(request.username());
	if (it != m_Creating.end())
	{
		return;
	}

	m_Creating.insert(std::make_pair(request.username(), pPack->m_Connect));
	Application::getInstance().GetAccountConnect().SendProtoBuf(pPack->getMessageId(), request);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::CreateAccountResponse(PackPtr& pPack)
{
	if (pPack->getAddr() != Application::getInstance().GetAccountConnect().getAddress())
		return;

	pm_createaccount_response response;
	CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	
	std::unordered_map<std::string, ConnectPtr>::iterator it = m_Creating.find(response.username());
	CHECKERRORANDRETURN(it != m_Creating.end());

	it->second->SendBuffer(pPack->getMessageId(), response, 0);
	m_Creating.erase(it);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::AccountCheck(PackPtr& pPack)
{
	pm_account_check request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	request.set_increament(m_LoginGameIncreament);
	m_pLogin.insert(std::make_pair(m_LoginGameIncreament++, pPack->m_Connect));

	Application::getInstance().GetAccountConnect().SendProtoBuf(pPack->getMessageId(), request);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::AccountCheckResponse(PackPtr& pPack)
{
	if (pPack->getAddr() != Application::getInstance().GetAccountConnect().getAddress())
		return;

	pm_account_check_response response;
	CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	std::unordered_map<unsigned int, ConnectPtr>::iterator it = m_pLogin.find(response.increament());
	CHECKERRORANDRETURN(it != m_pLogin.end());

	if (response.result() == 0)
	{
		std::unordered_map<ConnectPtr, Session>::iterator sit = m_AllPlayer.find(it->second);
		if (sit != m_AllPlayer.end())
		{
			std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator accit = m_AccPlayer.find(sit->second.acc_id);
			if (accit != m_AccPlayer.end())
			{
				std::unordered_map<unsigned int, Player*>::iterator playerit = accit->second.find(sit->second.playerid);
				if (playerit != accit->second.end() && playerit->second->GetPlayerState() == Player::PlayerState::psOnline)
				{
					playerit->second->OnPlyaerLogout();
				}
			}
		}

		Session player;
		player.acc_id = response.username();
		player.playerid = 0;
		m_AllPlayer[it->second] = player;

		std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator accit = m_AccPlayer.find(response.username());
		if (accit != m_AccPlayer.end())
		{
			std::unordered_map<unsigned int, Player*>::iterator playerit = accit->second.begin();
			for (; playerit != accit->second.end(); ++playerit)
			{
				pm_player_sub_data* pData = response.add_playerdata();
				pData->set_id(playerit->second->GetPlayerData().id);
				pData->set_name(playerit->second->GetPlayerData().name);
			}
		}
		else
		{
			m_AccPlayer.insert(std::make_pair(response.username(), std::unordered_map<unsigned int, Player*>()));
		}
	}

	it->second->SendBuffer(pPack->getMessageId(), response, 0);
	m_pLogin.erase(it);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::LoginGame(PackPtr& pPack)
{
	pm_login_game request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	pm_login_game_response response;
	response.set_result(0);
	std::unordered_map<ConnectPtr, Session>::iterator sit = m_AllPlayer.find(pPack->m_Connect);
	if (sit == m_AllPlayer.end())	
		return;
	std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator accit = m_AccPlayer.find(sit->second.acc_id);
	if (accit == m_AccPlayer.end())	
		return;
	std::unordered_map<unsigned int, Player*>::iterator playerit = accit->second.find(sit->second.playerid);
	if (playerit != accit->second.end())
	{
		if (playerit->second->GetPlayerState() == Player::PlayerState::psOnline)
		{
			playerit->second->OnPlyaerLogout();
		}
	}
	playerit = accit->second.find(request.id());
	if (playerit == accit->second.end())
		return;
	playerit->second->OnPlyaerLogin(pPack->m_Connect);
	sit->second.playerid = request.id();
				
	pm_playerdata* pData = new pm_playerdata;
	playerit->second->GetPlayerData().toPBMessage(*pData);
	response.set_allocated_data(pData);

	pPack->m_Connect->SendBuffer(GM_LOGIN_RESPONSE, response, 0);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::CreatePlayer(PackPtr& pPack)
{
	pm_create_player request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	std::unordered_map<ConnectPtr, Session>::iterator sit = m_AllPlayer.find(pPack->m_Connect);
	if (sit == m_AllPlayer.end())	
		return;
	std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator accit = m_AccPlayer.find(sit->second.acc_id);
	if (accit == m_AccPlayer.end())		
		return;

	pm_create_player_response response;
	
	response.set_result(0);
	PlayerData pData;
	pData.id = ++_unique_player;
	pData.name = request.player_name();
	pData.acc_id = sit->second.acc_id;
	InitPlayer(pData);	

    InsertPlayer(pData);

	Player* p = new Player(pData);
	ModuleManager::getInstance().CreateRoleModule(p);
	p->SetPlayerState(Player::PlayerState::psOffline);
	accit->second.insert(std::make_pair(pData.id, p));

	pm_playerdata* pItem = new pm_playerdata;
	pData.toPBMessage(*pItem);
	response.set_allocated_data(pItem);

	pPack->m_Connect->SendBuffer(GM_CREATE_PLAYER_RESPONSE, response, pData.id);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::InitPlayer(PlayerData& player)
{
	player.hp = 150;
	player.maxhp = 150;
}

//-------------------------------------------------------------------------------------------
void PlayerManager::HeartBeat(PackPtr& pPack)
{
	pm_client_heart_beat request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	pm_client_heart_beat_response response;

	response.set_currtime((int)time(NULL));
	pPack->m_Connect->SendBuffer(GM_CLIENT_HEARTBEAT_RESPONSE, response, 0);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::InsertPlayer(PlayerData& data)
{
    std::stringstream ss;
    ss << "insert into player(accid,id,jinbi,hp,maxhp,cellid,name) values(";
    ss << "'" << data.acc_id << "',";
    ss << data.id << ",";
    ss << data.jinbi << ",";
    ss << data.hp << ",";
    ss << data.maxhp << ",";
    ss << data.cellid << ",";
    ss << "'" << data.name << "')";

    m_pDBService->asynExcute(data.id, ss.str(), std::function<void(DBResult&)>());
}