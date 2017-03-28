#include "stdafx.h"
#include "TongheModule.h"
#include "Player.h"
#include "ObjectModule.h"
#include "Application.h"
#include "MapManager.h"

//-------------------------------------------------------------------------------------------
ObjectModule::ObjectModule(std::string modulename, Player* pOwner) : BaseModule(modulename, pOwner)
{
	registmessage();
}

//-------------------------------------------------------------------------------------------
ObjectModule::~ObjectModule()
{

}

//-------------------------------------------------------------------------------------------
void ObjectModule::registmessage()
{
	m_pOwner->RegisterNetMsgHandle(GM_REQUEST_USE, std::bind(&ObjectModule::processRequestUse, this, std::placeholders::_1));
}

//-------------------------------------------------------------------------------------------
void ObjectModule::unregistmessage()
{

}

//-------------------------------------------------------------------------------------------
bool ObjectModule::Init()
{
	return true;
}

//-------------------------------------------------------------------------------------------
void ObjectModule::processRequestUse(PackPtr& pPack)
{
	int64 dbid=0;
	int32 target=1;
	Object* pObj = GetObject(dbid);
	if (pObj == NULL)
		return;

	if (pObj->usage == obj_usage_medicine)
	{
		UseMedicine(pObj, target);
	}

	if (pObj->num <= 0)
	{
		
	}
}

//-------------------------------------------------------------------------------------------
void ObjectModule::UseMedicine(Object* pObj, int32 target)
{

}

//-------------------------------------------------------------------------------------------
void ObjectModule::AddObject(int32 id, int32 num)
{
    Object obj;
    obj.dbid = sApp.getDBID();
    obj.id = id;
    obj.kind = 1;
    obj.subkind = 1;
    obj.num = num;

    _props[obj.dbid] = obj;
}

//-------------------------------------------------------------------------------------------
void ObjectModule::DelObject(int64 dbid, int32 num)
{
    _props.erase(dbid);
}

//-------------------------------------------------------------------------------------------
Object* ObjectModule::GetObject(int64 dbid)
{
    Object* pItem = nullptr;
    std::unordered_map<int64, Object>::iterator it = _props.find(dbid);
    if (it != _props.end())
        pItem = &it->second;
    return pItem;
}