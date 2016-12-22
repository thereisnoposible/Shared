#include "stdafx.h"
#include "MapManager.h"
#include "Player.h"
#include "Cell.h"
#include "aoi/Space.h"
MapManager* Singleton<MapManager>::single = nullptr;

MapManager::MapManager()
{

}

MapManager::~MapManager()
{
	std::unordered_map<int32, AOI::Space*>::iterator it = m_SpaceMap.begin();
	for (; it != m_SpaceMap.end(); ++it)
	{
		delete it->second;
	}
}

AOI::Entity* MapManager::FindEntity(int32 id)
{
	AOI::Entity* pItem = nullptr;
	std::unordered_map<int32, AOI::Entity*>::iterator it = m_Entities.find(id);
	if (it != m_Entities.end())
	{
		pItem = it->second;
	}
	return pItem;
}

void MapManager::AddEntity(int32 space_id, AOI::Entity* entity)
{
	std::unordered_map<int32, AOI::Space*>::iterator it = m_SpaceMap.find(space_id);
	if (it != m_SpaceMap.end())
	{
		it->second->addEntityAndEnterWorld(entity);
	}
	else
	{
		AOI::Space* space = new AOI::Space();
		space->addEntityAndEnterWorld(entity);
		m_SpaceMap[space_id] = space;
	}

	m_Entities[entity->id()] = entity;
}

void MapManager::DeleteEntity(int32 space_id, AOI::Entity* entity)
{
	std::unordered_map<int32, AOI::Space*>::iterator it = m_SpaceMap.find(space_id);
	if (it != m_SpaceMap.end())
	{
		it->second->removeEntity(entity);
	}

	m_Entities.erase(entity->id());
}

AOI::Space* MapManager::GetSpace(int32 id)
{
	AOI::Space* pItem = NULL;
	std::unordered_map<int32, AOI::Space*>::iterator it = m_SpaceMap.find(id);
	if (it != m_SpaceMap.end())
	{
		pItem = it->second;
	}
	return pItem;
}
