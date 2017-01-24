#include "stdafx.h"
#include "PlayerMgr.h"
#include "Player.h"
#include "ModuleManager.h"
#include "MapManager.h"
#include "Application.h"
#include "MysqlStmt.cpp"
#include "../new/proto/protobuf/login.pb.h"
#include "../new/proto/protobuf/player.pb.h"

PlayerManager* Singleton<PlayerManager>::single = nullptr;

void cbbb(StmtExData* pData)
{
    if (pData->result.bResult == false)
        return;

    while (!pData->eof())
    {
        StmtData* pItem = pData->result.GetData("hp");
        std::cout << "hp = " << *(int*)pItem->buffer << "\n";
        pData->nextRow();
    }
}

//-------------------------------------------------------------------------------------------
PlayerManager::PlayerManager(DBService* p) :m_pDBService(p)
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
	sNetService.RegisterMessage(GM_CREATE_ACCOUNT, boost::bind(&PlayerManager::CreateAccount, this, _1));
	sNetService.RegisterMessage(GM_ACCOUNT_CHECK, boost::bind(&PlayerManager::AccountCheck, this, _1));
	sNetService.RegisterMessage(GM_LOGIN, boost::bind(&PlayerManager::LoginGame, this, _1));
	sNetService.RegisterMessage(GM_CREATE_PLAYER, boost::bind(&PlayerManager::CreatePlayer, this, _1));
	sNetService.RegisterMessageRange(PLAYERMESSAGEBEGIN, PLAYERMESSAGEEND, boost::bind(&PlayerManager::onPlayerMessage, this, _1));
	sNetService.RegisterMessage(GM_HEARTBEAT_FROM_CLIENT, boost::bind(&PlayerManager::HeartBeat, this, _1));
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
void PlayerManager::loadPlayer(pm_playerdata_db_response& response)
{
    for (int32 i = 0; i < response.datas_size(); i++)
    {
        PlayerData pData;
        pData.fromPBMessage(response.datas(i));

        if (pData.id > (int)_unique_player)
            _unique_player = pData.id;

        std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator it = m_AccPlayer.find(pData.account);
        if (it == m_AccPlayer.end())
        {
            m_AccPlayer.insert(std::make_pair(pData.account, std::unordered_map<unsigned int, Player*>()));
            it = m_AccPlayer.find(pData.account);
        }
        Player* p = new Player(pData);
        ModuleManager::getInstance().CreateRoleModule(p);
        p->SetPlayerState(Player::PlayerState::psOffline);
        it->second.insert(std::make_pair(pData.id, p));
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
std::unordered_map<ConnectPtr, PlayerManager::Session>& PlayerManager::GetPlayerMap()
{
    return m_AllPlayer;
}

//-------------------------------------------------------------------------------------------
std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>& PlayerManager::GetAccPlayerMap()
{
    return m_AccPlayer;
}

//-------------------------------------------------------------------------------------------
void PlayerManager::onPlayerMessage(PackPtr& pPack)
{
	std::unordered_map<ConnectPtr, Session>::iterator it = m_AllPlayer.find(pPack->m_Connect);
	if (it == m_AllPlayer.end())
        return;

    std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator accit = m_AccPlayer.find(it->second.acc_id);
    if (accit == m_AccPlayer.end())
        return;


    std::unordered_map<unsigned int, Player*>::iterator playerit = accit->second.find(it->second.playerid);
    if (playerit->second->GetPlayerState() != Player::PlayerState::psOnline)
        return;

    playerit->second->FireNetMsg(pPack->getMessageId(), pPack);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::CreateAccount(PackPtr& pPack)
{
	pm_createaccount request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

    Application::getInstance().GetAccountConnect().CreateAccount(pPack->m_Connect, request);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::AccountCheck(PackPtr& pPack)
{
	pm_account_check request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

    Application::getInstance().GetAccountConnect().AccountCheck(pPack->m_Connect, request);
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
	pData.id = _unique_player++;
    pData.jinbi = 0;
	pData.name = request.player_name();
    pData.account = sit->second.acc_id;
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
    pm_playerdata notify;
    data.toPBMessage(notify);

    sApp.GetDataBaseConnect().SendProtoBuf(GM_INSERT_PLAYER, notify);
}