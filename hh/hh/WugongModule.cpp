#include "stdafx.h"
#include "WugongModule.h"
#include "Player.h"

WugongModule::WugongModule(std::string modulename, Player* pOwner) : BaseModule(modulename, pOwner)
{

}
WugongModule::~WugongModule()
{

}

void WugongModule::registmessage()
{

}
void WugongModule::unregistmessage()
{

}

bool WugongModule::Init()
{
	setInitOk();
	return true;
}

bool WugongModule::Learning(Wugong& wugong)
{
	if (_wugongMap.find(wugong.id) == _wugongMap.end())
	{
		_wugongMap.insert(std::make_pair(wugong.id, wugong));
	}

	return true;
}

int WugongModule::GetWugongLevel(int wugongid)
{
	int level = -1;
	std::hash_map<int, Wugong>::iterator it = _wugongMap.find(wugongid);
	if ( it!= _wugongMap.end())
	{
		level = it->second.level;
	}
	return level;
}
void WugongModule::AddWugongLevel(int wugongid)
{
	std::hash_map<int, Wugong>::iterator it = _wugongMap.find(wugongid);
	if (it != _wugongMap.end())
	{
		it->second.level += 1;
	}
}