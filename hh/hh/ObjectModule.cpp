#include "stdafx.h"
#include "Player.h"
#include "ObjectModule.h"
#include "Application.h"
#include "MapManager.h"
#include "CoroutineManager.h"

#include "table/type_prop.h"

//-------------------------------------------------------------------------------------------
ObjectModule::ObjectModule(std::string modulename, Player* pOwner) : BaseModule(modulename, pOwner)
{

}

//-------------------------------------------------------------------------------------------
ObjectModule::~ObjectModule()
{

}

void ObjectModule::InitFunc()
{
	pm_object_data_db request;
	request.set_playerid(m_pOwner->GetPlayerID());
	sApp.GetDataBaseConnect().SendProtoBuf(GM_OBJECT_DATA_DB, request);
}

void ObjectModule::registmessage()
{

}

//-------------------------------------------------------------------------------------------
void ObjectModule::OnDataResponse(pm_object_data_response_db& response)
{
	if (IsInitOK())
		return;

	for (int32 i = 0; i < response.datas_size(); i++)
	{
		TABLE::type_prop* pType = s_type_propTable.Get1(response.datas(i).prop_id());
		if (!pType)
			continue;		

		objectItem* temp = new objectItem;
		temp->fromPBMessage(response.datas(i));
		(_props)[temp->dbid] = temp;

		if (pType->append)
		{
			_append_props[temp->prop_id] = temp;
		}
	}

	SetInitOK();
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