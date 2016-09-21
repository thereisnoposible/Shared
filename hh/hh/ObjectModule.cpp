#include "stdafx.h"
#include "TongheModule.h"
#include "Player.h"
#include "ObjectModule.h"
#include "MapManager.h"

#include "../new/proto/protobuf/object.pb.h"

//-------------------------------------------------------------------------------------------
ObjectModule::ObjectModule(std::string modulename, Player* pOwner) : BaseModule(modulename, pOwner)
{
	registmessage();
	std::vector<PropItem> temp;
	temp.resize(EquipmentSubTypeEnd);
	_props.insert(std::make_pair(-1, temp));
}

//-------------------------------------------------------------------------------------------
ObjectModule::~ObjectModule()
{

}

//-------------------------------------------------------------------------------------------
void ObjectModule::registmessage()
{
	m_pOwner->RegisterNetMsgHandle(GM_REQUEST_USE, boost::bind(&ObjectModule::processUse, this, _1));
}

//-------------------------------------------------------------------------------------------
void ObjectModule::unregistmessage()
{

}

//-------------------------------------------------------------------------------------------
bool ObjectModule::Init()
{
	setInitOk();
	return true;
}

void ObjectModule::addPropItem(PropItem& item)
{
	const PropType* pItem = TypeTable::getInstance().GetPropType(item.id);
	CHECKERRORANDRETURN(pItem != nullptr);

	std::hash_map<int, std::vector<PropItem>>::iterator it = _props.find(item.id);
	if (it == _props.end())
	{
		_props.insert(std::make_pair(item.id, std::vector<PropItem>()));
		it = _props.find(item.id);
	}
	std::vector<PropItem>& temp = it->second;
	if (pItem->baccumulate == false)
	{
		temp.push_back(item);
	}
	else
	{
		if (temp.size() < 1)
		{

			temp.push_back(item);
		}
		else
		{
			temp[0].num += item.num;
		}
	}
}

//-------------------------------------------------------------------------------------------
void ObjectModule::processUse(PackPtr& pPack)
{
	pm_request_use request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	pm_use_response response;
	response.set_result(0);
	auto it = _props.begin();
	for (; it != _props.end(); it++)
	{
		if (it->first == -1)
			continue;

		std::vector<PropItem>& temp = it->second;
		bool bBreak = false;
		for (int i = 0; i < (int)temp.size(); i++)
		{
			if (temp[i].dbid == request.dbid())
			{
				if (temp[i].num <= 0)
					continue;

				bBreak = true;

				const PropType* pItem = TypeTable::getInstance().GetPropType(temp[i].id);
				CHECKERRORANDCONTINUE(pItem != nullptr);
				if (pItem->type == EQUIPMENT)
				{
					auto propit = _props.find(-1);
					PropItem item = propit->second[pItem->sub_type];
					propit->second[pItem->sub_type] = temp[i];
					temp[i] = item;

				}
				if (pItem->type == MEDICINE)
				{
					m_pOwner->UseMedicine(temp[i]);
					temp[i].num -= 1;
				}
				break;
			}
		}
		if (bBreak)
			break;
	}

	m_pOwner->SendProtoBuf(GM_USE_RESPONSE, response);
}