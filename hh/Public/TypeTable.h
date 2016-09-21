#pragma once

#include "Singleton.h"
#include <hash_map>
#include "Struct.h"
#include "Enum.h"
class TypeTable : public Singleton<TypeTable>
{
public:
	TypeTable();
	~TypeTable();

	const WugongType* GetWugongType(int id);

	const WugongLevelUpType* GetWugongLevelUpType(int quality,int level);

	const PropType* TypeTable::GetPropType(int id);
protected:
	void loadAllTable();

	void loadTypeProp();

	void loadTypeWugong();

	void loadTypeWugongLevelUp();

private:
	std::hash_map<int, WugongType> _wugongMap;
	std::hash_map<int, std::hash_map<int, WugongLevelUpType>> _wugongLevelUpMap;
	std::hash_map<int, PropType> _propMap;
};