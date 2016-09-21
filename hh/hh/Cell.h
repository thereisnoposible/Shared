#pragma once
#include<unordered_map>

#include "Player.h"

class Cell
{
public:
	Cell(int _id);

	void OnRequestMove(Player*);
	void OnMoveOver(Player*);

	void OnPlayerMove(Player*);

	void OnAddPlayer(Player*);
	void OnRemovePlayer(Player*);
	void update();

	void BrocastPlayerMessage(Player*, void(Player::*pfunc)(void*),void* param);
	void OnBrocastPlayerMessage(Player*, void(Player::*pfunc)(void*), void* param);
private:
	int id;
	std::unordered_map<int,Player*> m_pPlayer;
	std::unordered_map<int, Player*> m_pMoving;
	std::vector<Cell*> m_NearCell;

	Position3D teleports;
};