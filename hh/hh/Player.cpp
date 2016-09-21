#include "stdafx.h"
#include "Player.h"
#include <stdlib.h>
#include "sourceMap.h"
#include "PlayerMgr.h"
#include "ObjectModule.h"
#include "MapManager.h"

Player::Player(PlayerData& p) : _UniqueCount(0), speed(10)
{
	starttime = std::chrono::steady_clock::now();
	_playerdata = p;
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

//void Player::SetConnect(boost::shared_ptr<NetConnect>& conn)
//{
	
//}

void Player::OnPlyaerLogin(ConnectPtr& conn)
{
	if (state == PlayerState::psOffline)
	{
		MapManager::getInstance().AddPlayer(this);
	}
	m_pConnect = conn;
	state = PlayerState::psOnline;
}

void Player::OnPlyaerLogout()
{
	MapManager::getInstance().DeletePlayer(this);
	m_pConnect = ConnectPtr();
	state = PlayerState::psOffline;
}

void Player::AddProp(PropItem& item)
{
	ObjectModule* pModule = dynamic_cast<ObjectModule*>(GetModule(OBJECT));

	pModule->addPropItem(item);
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

//void Player::Init()
//{
//	auto it = m_pModuleMap.begin();
//	for (; it != m_pModuleMap.end(); it++)
//		it->second->Init();
//}
//
//void Player::OnModuleInitOk()
//{
//	bool bOk = true;
//	auto it = m_pModuleMap.begin();
//	for (; it != m_pModuleMap.end(); it++)
//	{
//		if (!it->second->IsInitOk())
//		{
//			bOk = false;
//			break;
//		}
//	}
//	if (bOk)
//	{
//		PlayerManager::getInstance().PlayerLogin(this);
//	}
//}
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

void Player::UseMedicine(PropItem& temp)
{
	const PropType* pItem = TypeTable::getInstance().GetPropType(temp.id);
	CHECKERRORANDRETURN(pItem != nullptr);

	if (pItem->type != MEDICINE)
		return;

	_playerdata.hp += temp.yaowu.addhp;
	_playerdata.mp += temp.yaowu.addmp;

	if (_playerdata.hp > _playerdata.maxhp)
		_playerdata.hp = _playerdata.maxhp;
	if (_playerdata.mp > _playerdata.maxmp)
		_playerdata.mp = _playerdata.maxmp;
}

std::string Player::GetPlayerAddress()
{
	return m_pConnect.get() != nullptr ? m_pConnect->GetAddress() : "";
}
int Player::GetXuedian()
{
	return _playerdata.xuedian;
}
int Player::GetCellID()
{
	return _playerdata.cellid;
}
void Player::SetCellID(int id)
{
	_playerdata.cellid = id;
}
void Player::Addxuedian(int i)
{
	_playerdata.xuedian += i;
}

//-------------------------------------------------------------------------------------------
void Player::OnPlayerMove(void* param)
{
	Player* player = (Player*)param;

	Json::FastWriter write;
	Json::Value value;
	value["playerid"] = player->GetPlayerID();
	value["currpos_x"] = player->currpos.x;
	value["currpos_y"] = player->currpos.y;
	value["dstpos_x"] = player->dstpos.x;
	value["dstpos_y"] = player->dstpos.y;

	std::string str;
	str = write.write(value);

	m_pConnect->Send(GM_NOTIFY_MOVEING, str.c_str(),str.size(), GetPlayerID());
}

//-------------------------------------------------------------------------------------------
void Player::SendProtoBuf(int messageid, ::google::protobuf::Message &proto)
{
	m_pConnect->SendBuffer(messageid, proto, GetPlayerID());
}