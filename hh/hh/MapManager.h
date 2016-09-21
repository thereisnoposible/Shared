#pragma once
#include<unordered_map>
#include "TimerManager.h"

class Cell;
class MapManager : public Singleton<MapManager>
{
public:
	MapManager();
	~MapManager();

	void InitialCell();

	void AddPlayer(Player*);
	void DeletePlayer(Player*);

	void Update();

	Cell* GetCell(int id);

	void BrocastPlayerMessage(Player*, void (Player::*pfunc)(void*),void* param);
private:
	boost::mutex _mutex;
	std::unordered_map<int, Cell*> m_CellMap;

	TMHANDLE m_Update;
};