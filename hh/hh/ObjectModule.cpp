#include "stdafx.h"
#include "TongheModule.h"
#include "Player.h"
#include "ObjectModule.h"
#include "Application.h"
#include "MapManager.h"

#include "table/type_prop.h"

//-------------------------------------------------------------------------------------------
ObjectModule::ObjectModule(std::string modulename, Player* pOwner) : BaseModule(modulename, pOwner)
{

}

//-------------------------------------------------------------------------------------------
ObjectModule::~ObjectModule()
{
	for (auto it = _props.begin(); it != _props.end(); ++it)
	{
		delete it->second;
	}

	_props.clear();
	_append_props.clear();
}

//-------------------------------------------------------------------------------------------
bool ObjectModule::Init()
{
	pm_object_data_db request;
	request.set_playerid(m_pOwner->GetPlayerID());
	sApp.GetDataBaseConnect().SendProtoBuf(GM_OBJECT_DATA_DB, request);
	return true;
}

//-------------------------------------------------------------------------------------------
void ObjectModule::OnDataResponse(pm_object_data_response_db& response)
{
	for (int32 i = 0; i < response.datas_size(); i++)
	{
		TABLE::type_prop* pType = s_type_propTable.Get1(response.datas(i).prop_id());
		if (!pType)
			continue;		

		objectItem* temp = new objectItem;
		temp->fromPBMessage(response.datas(i));
		_props[temp->dbid] = temp;

		if (pType->append)
		{
			_append_props[temp->prop_id] = temp;
		}
	}

	SetInitOK();
}

//-------------------------------------------------------------------------------------------
void ObjectModule::AddObject(int32 id, int32 num, int32 cause)
{
	TABLE::type_prop* pType = s_type_propTable.Get1(id);
	if (!pType)
		return;

	if (!pType->append)
	{
		objectItem* temp = new objectItem;
		temp->dbid = sApp.getDBID();
		temp->playerid = m_pOwner->GetPlayerID();
		temp->prop_id = id;
		temp->prop_num = num;

		Insert(*temp);
		_props[temp->dbid] = temp;

		return;
	}

	auto it = _append_props.find(id);
	if (it == _append_props.end())
	{
		objectItem* temp = new objectItem;
		temp->dbid = sApp.getDBID();
		temp->playerid = m_pOwner->GetPlayerID();
		temp->prop_id = id;
		temp->prop_num = 0;

		Insert(*temp);
		_props[temp->dbid] = temp;
		_append_props[temp->prop_id] = temp;
		it = _append_props.find(id);
	}

	it->second->prop_num += num;
	Update(*it->second);
}

//-------------------------------------------------------------------------------------------
void ObjectModule::DestroyObject(int64 dbid, int32 num, int32 cause)
{
	auto it = _props.find(dbid);
	if (it == _props.end())
		return;

	it->second->prop_num -= num;

	if (it->second->prop_num > 0)
	{
		Update(*it->second);
		return;
	}

	Delete(*it->second);

	_append_props.erase(it->second->prop_id);
	delete it->second;
	_props.erase(it);
}

//-------------------------------------------------------------------------------------------
int32 ObjectModule::GetCount(int64 dbid)
{
	std::unordered_map<int64, objectItem*>::iterator it = _props.find(dbid);
	if (it == _props.end())
		return 0;

	return it->second->prop_num;
}

//-------------------------------------------------------------------------------------------
void ObjectModule::Insert(objectItem& temp)
{
	pm_object notify;
	temp.toPBMessage(notify);

	sApp.GetDataBaseConnect().SendProtoBuf(GM_INSERT_OBJECT, notify);
}

//-------------------------------------------------------------------------------------------
void ObjectModule::Update(objectItem& temp)
{
	pm_object notify;
	temp.toPBMessage(notify);

	sApp.GetDataBaseConnect().SendProtoBuf(GM_UPDATE_OBJECT, notify);
}

//-------------------------------------------------------------------------------------------
void ObjectModule::Delete(objectItem& temp)
{
	pm_object notify;
	temp.toPBMessage(notify);

	sApp.GetDataBaseConnect().SendProtoBuf(GM_DELETE_OBJECT, notify);
}