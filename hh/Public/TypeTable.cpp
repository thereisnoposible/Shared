#include "TypeTable.h"

TypeTable* Singleton<TypeTable>::single = nullptr;
TypeTable::TypeTable()
{
	loadAllTable();
}

TypeTable::~TypeTable()
{

}

void TypeTable::loadAllTable()
{
	loadTypeProp();
	loadTypeWugong();
	loadTypeWugongLevelUp();
}

void TypeTable::loadTypeProp()
{
	PropType item;
	item.id = 1;
	item.type = CURRENCY;
	item.baccumulate = false;
	_propMap.insert(std::make_pair(item.id, item));
	for (int i = 1; i < 100; i++)
	{
		item.id = i + 1;
		item.type = EQUIPMENT;
		item.sub_type = (i+1) % (EquipmentSubTypeEnd);
		_propMap.insert(std::make_pair(item.id, item));
		item.baccumulate = false;
	}
}

const PropType* TypeTable::GetPropType(int id)
{
	PropType* pItem = nullptr;
	std::hash_map<int, PropType>::iterator it = _propMap.find(id);
	if (it != _propMap.end())
		pItem = &it->second;
	return pItem;
}

void TypeTable::loadTypeWugong()
{
	for (int i = 0; i < 4; i++)
	{
		WugongType item;
		item.id = i+1;
		item.type = WAIGONG;
		item.quality = i;
		_wugongMap.insert(std::make_pair(i, item));
	}
}

const WugongType* TypeTable::GetWugongType(int id)
{
	WugongType* pItem = nullptr;
	std::hash_map<int, WugongType>::iterator it = _wugongMap.find(id);
	if (it != _wugongMap.end())
		pItem = &it->second;
	return pItem;
}

void TypeTable::loadTypeWugongLevelUp()
{	
	for (int i = 0; i < 10; i++)
	{
		std::hash_map<int,WugongLevelUpType> itemMap;
		WugongLevelUpType item;
		item.quality = i;
		item.needxuedian = i * 100;
		for (int j = 0; j < 10; j++)
		{
			item.level = j;
			itemMap.insert(std::make_pair(item.level, item));
		}
		_wugongLevelUpMap.insert(std::make_pair(item.quality, itemMap));
	}
}
const WugongLevelUpType* TypeTable::GetWugongLevelUpType(int quality, int level)
{
	WugongLevelUpType* pItem = nullptr;
	auto it = _wugongLevelUpMap.find(quality);
	if (it != _wugongLevelUpMap.end())
	{
		auto levelit = it->second.find(level);
		pItem = &levelit->second;
	}
	return pItem;
}
