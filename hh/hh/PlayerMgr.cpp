#include "stdafx.h"
#include "PlayerMgr.h"
#include "Player.h"
#include "ModuleManager.h"
#include "MapManager.h"
#include "ObjectModule.h"
#include "Application.h"
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

void SendBuffer(ConnectPtr& conn, int32 messageid, const  ::google::protobuf::Message & proto,int32 id)
{
	std::string buf;
	proto.SerializePartialToString(&buf);

	conn->Send(messageid, buf.c_str(), (int32)buf.length(), id);
}

//-------------------------------------------------------------------------------------------
PlayerManager::PlayerManager(DBService* p) :m_pDBService(p)
{
	registmessage();
}

//-------------------------------------------------------------------------------------------
PlayerManager::~PlayerManager()
{
	std::unordered_map<std::string, std::unordered_map<int, Player*>>::iterator it = m_AccPlayer.begin();
	for (; it != m_AccPlayer.end(); ++it)
	{
		std::unordered_map<int, Player*>::iterator playerit = it->second.begin();
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
void PlayerManager::Broadcast(int id, const ::google::protobuf::Message& mess, int except)
{
	auto sit = m_AllPlayer.begin();
	for (; sit != m_AllPlayer.end();sit++)
	{
		if (!sit->second.player)
			continue;
		if (sit->second.player->GetPlayerState() != Player::PlayerState::psOnline)		
			continue;
		
		if (sit->second.player->GetPlayerID() == except)
			continue;
		
		sit->second.player->SendProtoBuf(id, mess);
	}
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

		std::unordered_map<std::string, std::unordered_map<int, Player*>>::iterator it = m_AccPlayer.find(pData.account);
        if (it == m_AccPlayer.end())
        {
			m_AccPlayer.insert(std::make_pair(pData.account, std::unordered_map<int, Player*>()));
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
	auto sit = m_AllPlayer.find(playerid);
	if (sit != m_AllPlayer.end())
	{
		if (sit->second.player && sit->second.player->GetPlayerState() == Player::PlayerState::psOnline)
		{
			sit->second.player->OnPlyaerLogout();

			pm_other_info notify;
			notify.set_id(sit->second.player->GetPlayerID());
			notify.set_name(sit->second.player->GetPlayerData().name);
			Broadcast(GM_NOTIFY_OTHER_LOGOUT, notify, sit->second.player->GetPlayerID());

			m_AllPlayerID.erase(sit->second.player->GetPlayerID());
		}
		
		m_AllPlayer.erase(sit);
	}	
}

//-------------------------------------------------------------------------------------------
std::hash_map<ConnectPtr, PlayerManager::Session>& PlayerManager::GetPlayerMap()
{
    return m_AllPlayer;
}

//-------------------------------------------------------------------------------------------
std::unordered_map<int32, Player*>& PlayerManager::GetPlayerIDMap()
{
	return m_AllPlayerID;
}

//-------------------------------------------------------------------------------------------
Player* PlayerManager::GetPlayer(int32 playerid)
{
	auto it = m_AllPlayerID.find(playerid);
	if (it == m_AllPlayerID.end())
		return nullptr;

	return it->second;
}

//-------------------------------------------------------------------------------------------
std::unordered_map<std::string, std::unordered_map<int, Player*>>& PlayerManager::GetAccPlayerMap()
{
    return m_AccPlayer;
}

//-------------------------------------------------------------------------------------------
void PlayerManager::onPlayerMessage(PackPtr& pPack)
{
	auto it = m_AllPlayer.find(pPack->GetConnect());
	if (it == m_AllPlayer.end())
        return;

	if (!it->second.player)
		return;

    if (it->second.player->GetPlayerState() != Player::PlayerState::psOnline)
        return;

	if (it->second.player->GetConnect() != pPack->GetConnect())
	{
		return;
	}

	it->second.player->FireNetMsg(pPack->getMessageId(), pPack);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::CreateAccount(PackPtr& pPack)
{
	pm_createaccount request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer(), pPack->getBufferSize()));

	Application::getInstance().GetAccountConnect().CreateAccount(pPack->GetConnect(), request);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::AccountCheck(PackPtr& pPack)
{
	pm_account_check request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer(), pPack->getBufferSize()));

	Application::getInstance().GetAccountConnect().AccountCheck(pPack->GetConnect(), request);
}

void ttttt(boost::coroutines::symmetric_coroutine<void*>::yield_type& yield)
{

}

//-------------------------------------------------------------------------------------------
void PlayerManager::LoginGame(PackPtr& pPack)
{
	int64 dbid = CoroutineManager::getInstance().Alloc();

	CoroutineManager::getInstance().RegistFunc(dbid, [&](boost::coroutines::symmetric_coroutine<void*>::yield_type& yield){
		boost::shared_ptr<NetConnect> conn = pPack->GetConnect();
		int64 temp_dbid = dbid;
		pm_login_game request;
		CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer(), pPack->getBufferSize()));
		pm_login_game_response response;
		response.set_result(0);
		auto sit = m_AllPlayer.find(pPack->GetConnect());
		if (sit == m_AllPlayer.end())
			return;

		std::unordered_map<std::string, std::unordered_map<int, Player*>>::iterator accit = m_AccPlayer.find(sit->second.accid);
		if (accit == m_AccPlayer.end())
			return;

		std::unordered_map<int, Player*>::iterator playerit = accit->second.find(request.id());
		if (playerit == accit->second.end())
			return;

		Player* &player = sit->second.player;

		if (player && player->GetConnect() != pPack->GetConnect())
		{
			sit = m_AllPlayer.find(player->GetConnect());
			if (sit != m_AllPlayer.end())
			{
				sit->second.player = nullptr;
			}
		}

		if (player && player->GetPlayerState() == Player::PlayerState::psOnline)
		{
			player->OnPlyaerLogout();
		}


		playerit->second->OnPlyaerLogin(pPack->GetConnect());
		pm_other_info notify;
		notify.set_id(playerit->second->GetPlayerID());
		notify.set_name(playerit->second->GetPlayerData().name);
		Broadcast(GM_NOTIFY_OTHER_LOGIN, notify, playerit->second->GetPlayerID());

		player = playerit->second;

		player->InitModule();

		m_AllPlayerID[playerit->second->GetPlayerID()] = player;

		pm_playerdata* pData = response.mutable_data();
		playerit->second->GetPlayerData().toPBMessage(*pData);

		pm_icomponent_data* pidata = response.mutable_idata();
		playerit->second->toIcomponentData(*pidata);

		SendBuffer(conn, GM_LOGIN_RESPONSE, response, 0);

		CoroutineManager::getInstance().Free(temp_dbid);
	});

	auto iii = CoroutineManager::getInstance().GetCoroutine(dbid);
	if (iii)
	{
		iii->operator()(nullptr);
	}

	//boost::coroutines::symmetric_coroutine<void*>::call_type* coro_b = new boost::coroutines::symmetric_coroutine<void*>::call_type;
	//new(coro_b)boost::coroutines::symmetric_coroutine<void*>::call_type(fn);
	//coro_b->operator()("hehe");
}

//-------------------------------------------------------------------------------------------
void PlayerManager::CreatePlayer(PackPtr& pPack)
{
	pm_create_player request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer(), pPack->getBufferSize()));
	auto sit = m_AllPlayer.find(pPack->GetConnect());
	if (sit == m_AllPlayer.end())	
		return;
	std::unordered_map<std::string, std::unordered_map<int, Player*>>::iterator accit = m_AccPlayer.find(sit->second.accid);
	if (accit == m_AccPlayer.end())		
		return;

	pm_create_player_response response;
	
	response.set_result(0);
	PlayerData pData;
	pData.id = _unique_player++;
    pData.jinbi = 0;
	pData.name = request.player_name();
    pData.account = sit->second.accid;
	InitPlayer(pData);	

    InsertPlayer(pData);

	Player* p = new Player(pData);
	ModuleManager::getInstance().CreateRoleModule(p);
	p->SetPlayerState(Player::PlayerState::psOffline);
	accit->second.insert(std::make_pair(pData.id, p));

	pm_playerdata* pItem = response.mutable_data();
	pData.toPBMessage(*pItem);

	SendBuffer(pPack->GetConnect(), GM_CREATE_PLAYER_RESPONSE, response, 0);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::InitPlayer(PlayerData& player)
{

}

//-------------------------------------------------------------------------------------------
void PlayerManager::HeartBeat(PackPtr& pPack)
{
	pm_client_heart_beat request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer(), pPack->getBufferSize()));

	pm_client_heart_beat_response response;

	response.set_currtime((int)time(NULL));

	SendBuffer(pPack->GetConnect(), GM_CLIENT_HEARTBEAT_RESPONSE, response, 0);
}

//-------------------------------------------------------------------------------------------
void PlayerManager::InsertPlayer(PlayerData& data)
{
    pm_playerdata notify;
    data.toPBMessage(notify);

    sApp.GetDataBaseConnect().SendProtoBuf(GM_INSERT_PLAYER, notify);
}