#include "stdafx.h"
#include "Player.h"
#include <stdlib.h>
#include "sourceMap.h"
#include "PlayerMgr.h"
#include "MapManager.h"

Player::Player(PlayerData& p) : _UniqueCount(0), speed(10), AOI::Entity(p.id, AOI::EntityTypePlayer)
{
	starttime = std::chrono::steady_clock::now();
	_playerdata = p;
	status = PlayerStatus::psFree;


	RegisterNetMsgHandle(GM_REQUEST_MOVE, std::bind(&Player::processRequestMove, this, std::placeholders::_1));

}
Player::~Player()
{
	if (state == PlayerState::psOnline)
	{
		OnPlyaerLogout();
	}

	std::hash_map<std::string, BaseModule*>::iterator it = m_pModuleMap.begin();
	for (; it != m_pModuleMap.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
}

void Player::SetPlayerState(PlayerState _state)
{
	state = _state;
}

Player::PlayerState Player::GetPlayerState()
{
	return state;
}

void Player::SetPlayerStatus(PlayerStatus _status)
{
	status = _status;
}

Player::PlayerStatus Player::GetPlayerStatus()
{
	return status;
}

//void Player::SetConnect(boost::shared_ptr<NetConnect>& conn)
//{
	
//}

void Player::OnPlyaerLogin(ConnectPtr& conn)
{
	if (state == PlayerState::psOffline)
	{
		MapManager::getInstance().AddEntity(GetCellID(), this);
	}
	m_pConnect = conn;
	state = PlayerState::psOnline;

	auto it = m_pModuleMap.begin();
	for (; it != m_pModuleMap.end(); it++)
		it->second->OnLogin();
}

void Player::OnPlyaerLogout()
{
	MapManager::getInstance().DeleteEntity(GetCellID(), this);
	m_pConnect = ConnectPtr();
	state = PlayerState::psOffline;

	auto it = m_pModuleMap.begin();
	for (; it != m_pModuleMap.end(); it++)
		it->second->OnLogout();
}

void Player::AddProp(PropItem& item)
{

}

//---------------------------------------------------------------------------
void Player::RegisterEventHandle(int evt, EventHandle::handle fun)
{
	m_EventPump.RegisterMessage(evt, fun);
}

//---------------------------------------------------------------------------
void Player::RegisterEventRange(int evt_s, int evt_e, EventHandle::handle fun)
{
	m_EventPump.RegisterMessageRange(evt_s, evt_e, fun);
}

//---------------------------------------------------------------------------
void Player::TriggerEvent(const BaseEvent& edata)
{
	m_EventPump.FireMessage(edata.GetEvtID(), edata);
}

//-------------------------------------------------------------------------------------------
void Player::RegisterNetMsgHandle(int msgid, NetMsgHandle::handle fun)
{
	m_NetMsgPump.RegisterMessage(msgid, fun);
}
//-------------------------------------------------------------------------------------------
void Player::FireNetMsg(int msgid, PackPtr& pPack)
{
	m_NetMsgPump.FireMessage(msgid, pPack);
}

void Player::AddModule(BaseModule* pModule)
{
	m_pModuleMap.insert(std::make_pair(pModule->GetModuleName(), pModule));
}

BaseModule* Player::GetModule(std::string modulename)
{
	BaseModule* pModule = nullptr;
	std::hash_map<std::string, BaseModule*>::iterator it = m_pModuleMap.find(modulename);
	if (it != m_pModuleMap.end())
		pModule = it->second;
	return pModule;
}
int Player::GetPlayerID()
{
	return _playerdata.id;
}

void Player::SetPlayerID(int id)
{
	_playerdata.id = id;
}
PlayerData& Player::GetPlayerData()
{
	return _playerdata;
}

std::string Player::GetPlayerAddress()
{
	return m_pConnect.get() != nullptr ? m_pConnect->GetAddress() : "";
}
int Player::GetCellID()
{
	return _playerdata.cellid;
}
void Player::SetCellID(int id)
{
	_playerdata.cellid = id;
}

int32 Player::GetGengu()
{
	return _playerdata.gengu;
}

void Player::AddHP(int32 num)
{
	_playerdata.hp += num;
	if (_playerdata.hp > _playerdata.maxhp)
		_playerdata.hp = _playerdata.maxhp;
}

//-------------------------------------------------------------------------------------------
void Player::processRequestMove(PackPtr& pPack)
{
	Math::Vector3 pos;
	pos.x = 2;
	pos.y = 2;
	pos.z = 0;
	position(position() + pos);
	pEntityCoordinateNode()->update();

	std::list<int>& Witnesses = GetWitnesses();
	for (auto it = Witnesses.begin(); it != Witnesses.end(); ++it)
	{
		Entity* pEntity = MapManager::getInstance().FindEntity(*it);
		pEntity->onOtherEntityMove(this);
	}	
}

void Player::onOtherEntityMove(Entity* entity)
{
    std::cout << _playerdata.id << "currpos "<<position().x<<": player " << entity->id() << " move " << "curr pos:" << entity->position().x << "\n";
}

//-------------------------------------------------------------------------------------------
void Player::SendProtoBuf(int messageid, ::google::protobuf::Message &proto)
{
	m_pConnect->SendBuffer(messageid, proto, GetPlayerID());
}