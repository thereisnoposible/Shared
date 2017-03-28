#include "stdafx.h"
#include "Player.h"
#include <stdlib.h>
#include "sourceMap.h"
#include "PlayerMgr.h"
#include "MapManager.h"

#include <boost/any.hpp>

bool cmpp(int32 a)
{
	if (a <= 0)
	{
		return true;
	}

	return false;
}

Player::Player(PlayerData& p) : _UniqueCount(0), speed(10), AOI::Entity(p.id, AOI::EntityTypePlayer)
{
	starttime = std::chrono::steady_clock::now();
	_playerdata = p;
	status = PlayerStatus::psFree;

	m_RootComponent = INode::Create();

	INode* head = INode::Create(com_type_head);
	head->AddIComponent(power_type_attack, 1, com_property_self);
	head->AddIComponent(power_type_equip, 1, com_property_self);
	head->AddIComponent(power_type_hp, 10, com_property_self);
	head->SetCallBack(power_type_hp, std::bind(cmpp, std::placeholders::_1), 
		std::bind(&Player::OnPlayerDead, this, std::placeholders::_1));
	m_RootComponent->AddINode(head);

	INode* body = INode::Create(com_type_body);
	body->AddIComponent(power_type_equip, 1, com_property_self);
	body->AddIComponent(power_type_hp, 10, com_property_self);
	m_RootComponent->AddINode(body);

	INode* left_hand = INode::Create(com_type_hand);
	left_hand->AddIComponent(power_type_attack, 1, com_property_self);
	left_hand->AddIComponent(power_type_equip, 1, com_property_self);
	left_hand->AddIComponent(power_type_hp, 10, com_property_self);
	m_RootComponent->AddINode(left_hand);

	INode* sub_left_hand1 = INode::Create(com_type_hand);
	sub_left_hand1->AddIComponent(power_type_attack, 1, com_property_self);
	sub_left_hand1->AddIComponent(power_type_equip, 1, com_property_self);
	sub_left_hand1->AddIComponent(power_type_hp, 10, com_property_self);
	left_hand->AddINode(sub_left_hand1);

	INode* sub_left_hand2 = INode::Create(com_type_hand);
	sub_left_hand2->AddIComponent(power_type_attack, 1, com_property_self);
	sub_left_hand2->AddIComponent(power_type_equip, 1, com_property_self);
	sub_left_hand2->AddIComponent(power_type_hp, 10, com_property_self);
	left_hand->AddINode(sub_left_hand2);

	INode* right_hand = INode::Create(com_type_hand);
	right_hand->AddIComponent(power_type_attack, 1, com_property_self);
	right_hand->AddIComponent(power_type_equip, 1, com_property_self);
	right_hand->AddIComponent(power_type_hp, 10, com_property_self);
	m_RootComponent->AddINode(right_hand);

	INode* left_foot = INode::Create(com_type_foot);
	left_foot->AddIComponent(power_type_attack, 1, com_property_self);
	left_foot->AddIComponent(power_type_equip, 1, com_property_self);
	left_foot->AddIComponent(power_type_run, 1, com_property_self);
	left_foot->AddIComponent(power_type_walk, 1, com_property_self);
	left_foot->AddIComponent(power_type_hp, 10, com_property_self);
	m_RootComponent->AddINode(left_foot);

	INode* right_foot = INode::Create(com_type_foot);
	right_foot->AddIComponent(power_type_attack, 1, com_property_self);
	right_foot->AddIComponent(power_type_equip, 1, com_property_self);
	right_foot->AddIComponent(power_type_run, 1, com_property_self);
	right_foot->AddIComponent(power_type_walk, 1, com_property_self);
	right_foot->AddIComponent(power_type_hp, 10, com_property_self);
	m_RootComponent->AddINode(right_foot);

	RegisterNetMsgHandle(GM_REQUEST_MOVE, std::bind(&Player::processRequestMove, this, std::placeholders::_1));
	RegisterNetMsgHandle(GM_REQUEST_ROOM_INFO, std::bind(&Player::processRequestRoomInfo, this, std::placeholders::_1));

	RegisterNetMsgHandle(GM_REQUEST_FIGHT, std::bind(&Player::processRequestFight, this, std::placeholders::_1));

	RegisterNetMsgHandle(GM_REQUEST_OTHER_INFO, std::bind(&Player::processRequestOtherInfo, this, std::placeholders::_1));

	RegisterNetMsgHandle(GM_SHANGHUI, std::bind(&Player::processShanghui, this, std::placeholders::_1));

	RegisterNetMsgHandle(GM_DUIHUA, std::bind(&Player::processDuihua, this, std::placeholders::_1));
	RegisterNetMsgHandle(GM_XUANXIANG, std::bind(&Player::processXuanxiang, this, std::placeholders::_1));

	RegisterNetMsgHandle(GM_GOUMAI, std::bind(&Player::processGoumai, this, std::placeholders::_1));
}

void Player::OnPlayerDead(INode* pNode)
{
	std::cout << "dead\n";
	pNode->bActive = false;

}

void Player::toIcomponentData(pm_icomponent_data& data)
{
	m_RootComponent->toPMessage(data);
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
		MapManager::getInstance().AddEntity(0, this);
	}
	m_pConnect = conn;
	state = PlayerState::psOnline;

	auto it = m_pModuleMap.begin();
	for (; it != m_pModuleMap.end(); it++)
		it->second->OnLogin();
}

void Player::OnPlyaerLogout()
{
	MapManager::getInstance().DeleteEntity(0, this);
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

void Player::processRequestRoomInfo(PackPtr& pPack)
{
	pm_room_info_response response;
	response.set_result(0);

	std::unordered_map<ConnectPtr, PlayerManager::Session>& AllPlayer = PlayerManager::getInstance().GetPlayerMap();

	for (auto it = AllPlayer.begin(); it != AllPlayer.end(); it++)
	{
		if (it->second.player == nullptr)
			continue;

		if (it->second.player->GetPlayerID() == GetPlayerID())
			continue;

		pm_other_info* pItem = response.add_info();
		pItem->set_id(it->second.player->GetPlayerID());
		pItem->set_name(it->second.player->GetPlayerData().name);
	}

	SendProtoBuf(GM_ROOM_INFO_RESPONSE, response);
}

void Player::processRequestFight(PackPtr& pPack)
{
	pm_request_fight request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	pm_fight_response response;

	int32 result = 0;

	for (int32 k = 0; k < 1; k++)
	{
		auto mmap = PlayerManager::getInstance().GetPlayerIDMap();
		auto mmapit = mmap.find(request.playerid());
		if (mmapit == mmap.end())
		{
			result = 1;
			break;
		}

		Player* other = mmapit->second;
		INode* pNode = other->m_RootComponent->GetChildNode(request.node());
		if (pNode == nullptr)
		{
			result = 2;
			break;
		}

		pNode->AddIComponent(power_type_hp, -1, com_property_self);

		pm_notify_playerdata_change notify;
		notify.set_result(0);
		pm_playerdata* pData = notify.mutable_data();
		other->GetPlayerData().toPBMessage(*pData);

		pm_icomponent_data* pidata = notify.mutable_idata();
		other->toIcomponentData(*pidata);
		other->SendProtoBuf(GM_NOTIFY_PLAYERDATA_CHANGE, notify);
	}

	response.set_result(result);

	SendProtoBuf(GM_FIGHT_RESPONSE, response);
}

void Player::processRequestOtherInfo(PackPtr& pPack)
{
	pm_request_other_info request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	pm_other_info_response response;
	response.set_playerid(request.playerid());
	int32 result = 0;

	for (int32 k = 0; k < 1; k++)
	{
		auto mmap = PlayerManager::getInstance().GetPlayerIDMap();
		auto it = mmap.find(request.playerid());
		if (it == mmap.end())
		{
			result = 1;
			break;
		}

		Player* player = it->second;

		pm_icomponent_data* pItem = response.mutable_idata();
		player->toIcomponentData(*pItem);
	}

	response.set_result(result);
	SendProtoBuf(GM_OTHER_INFO_RESPONSE, response);

}

class Action
{
	int32 id;
	int32 content;
	int32 type; 				//1.交谈 2物品 3 任务 4 行为

	boost::any type_content;
	//string type_1;					//交谈内容
	//vector<shangren_prop> type_2; 	//商人物品
	//vector<int32> type_3; 			//任务ID
	//vector<xuanxiang> type_4;		//行为ID Action

	int32 server_action;				//1取消任务 2传送
	boost::any server_action_param;
	//std::string server_action_param_1;	//任务类型
	//std::string server_action_param_2;	//传送坐标
};

class NPC
{
	int32 id;
	int32 first;
	std::unordered_map<int32, Action> action;
};

std::unordered_map<int32, NPC> npc;
void init()
{

}

void Player::processShanghui(PackPtr& pPack)
{
	pm_shanghui_response response;

	response.add_npcid(1);
	response.add_npcname(Helper::Gb2312ToUTF8("武器商"));

	response.add_npcid(2);
	response.add_npcname(Helper::Gb2312ToUTF8("装备商"));

	response.add_npcid(3);
	response.add_npcname(Helper::Gb2312ToUTF8("药品商"));

	SendProtoBuf(GM_SHANGHUI_RESPONSE, response);
}

void Player::processDuihua(PackPtr& pPack)
{
	pm_duihua request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	pm_duihua_response response;

	response.set_result(0);
	response.set_npcid(request.npcid());
	
	if (request.npcid() == 1)
	{
		response.add_id(1);
		response.add_content(Helper::Gb2312ToUTF8("购买武器"));
	}

	if (request.npcid() == 2)
	{
		response.add_id(1);
		response.add_content(Helper::Gb2312ToUTF8("购买装备"));
	}

	if (request.npcid() == 3)
	{
		response.add_id(1);
		response.add_content(Helper::Gb2312ToUTF8("购买药品"));
	}

	SendProtoBuf(GM_DUIHUA_RESPONSE, response);
}

void Player::processXuanxiang(PackPtr& pPack)
{
	pm_xuanxiang request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	pm_xuanxiang_response response;

	response.set_result(0);

	if (request.npcid() == 1 && request.id() == 1)
	{
		response.set_type(1);
		response.set_content(Helper::Gb2312ToUTF8("武器卖光啦！ 请明天再来"));
	}

	if (request.npcid() == 2 && request.id() == 1)
	{
		response.set_type(2);
		Json::Value value;
		
		Json::Value sub;
		sub["id"] = Helper::Gb2312ToUTF8("衣服1");
		sub["price"] = 1;
		sub["num"] = 1000;

		value.append(sub);
		sub["id"] = Helper::Gb2312ToUTF8("衣服2");
		sub["price"] = 2;
		value.append(sub);
		sub["id"] = Helper::Gb2312ToUTF8("衣服3");
		sub["price"] = 3;
		value.append(sub);

		sub["id"] = Helper::Gb2312ToUTF8("衣服4");
		sub["price"] = 4;
		value.append(sub);


		Json::FastWriter write;
		write.write(value);
		response.set_content(write.write(value));
	}

	if (request.npcid() == 3 && request.id() == 1)
	{
		response.set_type(1);
		response.set_content(Helper::Gb2312ToUTF8("药品卖光啦！ 请明天再来"));
	}

	SendProtoBuf(GM_XUANXIANG_RESPONSE, response);
}

void Player::processGoumai(PackPtr& pPack)
{
	pm_goumai request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	pm_goumai_response response;

	response.set_result(0);

}

void Player::onOtherEntityMove(Entity* entity)
{
    std::cout << _playerdata.id << "currpos "<<position().x<<": player " << entity->id() << " move " << "curr pos:" << entity->position().x << "\n";
}

//-------------------------------------------------------------------------------------------
void Player::SendProtoBuf(int messageid, const ::google::protobuf::Message &proto)
{
	m_pConnect->SendBuffer(messageid, proto, GetPlayerID());
}