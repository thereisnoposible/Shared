#include "TypeTable.h"

#include "DBService.h"
#include <iostream>
TypeTable* Singleton<TypeTable>::single = nullptr;
TypeTable::TypeTable()
{
	loadAllTable();
	

	m_pDBService = new DBService(1);
	if (!m_pDBService->Open("192.168.0.222", "guest", "123456", "smh_type_yf"))
	{
		std::cout << "open smh_type_yf failed\n";
	}

	FindAllTypeTable();
}

TypeTable::~TypeTable()
{
	if (m_pDBService != nullptr)
		delete m_pDBService;
	
}

void TypeTable::FindAllTypeTable()
{
	DBResult result;
	m_pDBService->syncQuery("SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'smh_type_yf'", result);
	//while (!result.eof())
	{
		DataBucket temp;

	}
}

void TypeTable::loadAllTable()
{
	loadTypeXinfa();
}

void TypeTable::loadTypeXinfa()
{
	TypeXinfa item;
	item.id = 1;
	item.add_ratio = 1;
	item.add_count = 1;
	item.recovery_mp_ratio = 1;
	item.recovery_mp_count = 1;
	item.damage_ratio = 1;
	item.damage_count = 1;
	item.recovery_hp_ratio = 1;
	item.recovery_hp_count = 1;

	_typeXinfa[item.id] = item;
}

const TypeXinfa* TypeTable::getTypeXinfa(int32 id)
{
	const TypeXinfa* pItem = NULL;
	std::unordered_map<int32, TypeXinfa>::iterator it = _typeXinfa.find(id);
	if (it != _typeXinfa.end())
	{
		pItem = &it->second;
	}
	return pItem;
}