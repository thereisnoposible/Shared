#pragma once
#include<unordered_map>
#include "timer/TimerManager.h"
namespace AOI
{
	class Entity;
	class Space;
}

class MapManager : public Singleton<MapManager>, Initialer
{
public:
	MapManager();
	~MapManager();

	AOI::Entity* FindEntity(int32 id);

	void AddEntity(int32 cellid, AOI::Entity*);
	void DeleteEntity(int32 cellid, AOI::Entity*);

	AOI::Space* GetSpace(int32 id);
private:
	std::unordered_map<int32, AOI::Space*> m_SpaceMap;
	std::unordered_map<int32, AOI::Entity*> m_Entities;
};