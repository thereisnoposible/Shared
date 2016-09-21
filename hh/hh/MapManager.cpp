#include "stdafx.h"
#include "MapManager.h"
#include "Player.h"
#include "Cell.h"
MapManager* Singleton<MapManager>::single = nullptr;

MapManager::MapManager()
{
	InitialCell();
	m_Update = TimerManager::getInstance().AddIntervalTimer(0.5, boost::bind(&MapManager::Update, this));
}

MapManager::~MapManager()
{
	if (m_Update != nullptr)
	{
		TimerManager::getInstance().RemoveTimer(m_Update);
		m_Update = nullptr;
	}

	for (int i = 0; i <= 100; ++i)
	{
		if (m_CellMap[i] != nullptr)
		{
			delete m_CellMap[i];
			m_CellMap[i] = nullptr;
		}
	}
}

void MapManager::InitialCell()
{
	for (int i = 1; i <= 100; ++i)
	{
		m_CellMap[i] = new Cell(i);
	}
}

void MapManager::Update()
{
	std::unordered_map<int, Cell*>::iterator it = m_CellMap.begin();
	for (; it != m_CellMap.end(); ++it)
	{
		it->second->update();
	}
}

void MapManager::AddPlayer(Player* player)
{
	std::unordered_map<int, Cell*>::iterator it = m_CellMap.find(player->GetCellID());
	if (it != m_CellMap.end())
	{
		it->second->OnAddPlayer(player);
	}
	else
	{
		m_CellMap[1]->OnAddPlayer(player);
	}
}

void  MapManager::DeletePlayer(Player* pPlayer)
{
	std::unordered_map<int, Cell*>::iterator it = m_CellMap.find(pPlayer->GetCellID());
	if (it != m_CellMap.end())
	{
		it->second->OnRemovePlayer(pPlayer);
	}
}

Cell* MapManager::GetCell(int id)
{
	Cell* pItem = NULL;
	std::unordered_map<int,Cell*>::iterator it = m_CellMap.find(id);
	if (it != m_CellMap.end())
	{
		pItem = it->second;
	}
	return pItem;
}

void MapManager::BrocastPlayerMessage(Player*player, void (Player::*pfunc)(void*), void* param)
{
	std::unordered_map<int, Cell*>::iterator it = m_CellMap.find(player->GetCellID());
	if (it != m_CellMap.end())
	{
		Cell* pCell = it->second;
		pCell->BrocastPlayerMessage(player, pfunc, param);
	}
}
