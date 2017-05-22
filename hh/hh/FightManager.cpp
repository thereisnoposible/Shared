#include "stdafx.h"
#include "FightManager.h"
FightManager* Singleton<FightManager>::single = nullptr;

FightManager::FightManager()
{

}

FightManager::~FightManager()
{

}

void FightManager::CreateFight(FightBase* pFighter, FightBase* pDefender)
{
	int64 dbid = sApp.getDBID();
	FightInfo war(dbid, 10);
	war.AddPlayer(pFighter, 0);
	war.AddPlayer(pDefender, 9);
	war.Update();

	Fights[dbid] = war;
}

FightInfo* FightManager::GetFightInfo(int64 dbid)
{
	auto it = Fights.find(dbid);
	if (it == Fights.end())
		return;

	return &it->second;
}