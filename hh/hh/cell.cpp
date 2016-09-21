#include "stdafx.h"
#include "Cell.h"
#include "MapManager.h"

#define KBEVec3Length(v)						(v)->length()
#define KBEVec3Normalize(v, vv)					(v)->normalise()

Cell::Cell(int _id) :id(_id), teleports(100,100,0)
{

}

void Cell::OnAddPlayer(Player* player)
{
	m_pPlayer[player->GetPlayerID()] = player;
	player->SetCellID(id);
	player->currpos = Position3D(0,0,0);
}

void Cell::OnRemovePlayer(Player* player)
{
	m_pPlayer.erase(player->GetPlayerID());
	m_pMoving.erase(player->GetPlayerID());
	if (m_pMoving.size() == 0)
		std::cout << "MovingZero -----------------------";
}

void Cell::OnRequestMove(Player* player)
{
	m_pMoving[player->GetPlayerID()] = player;
}

void Cell::OnMoveOver(Player* player)
{
	if (player->currpos == teleports)
	{
		Cell* pCell = MapManager::getInstance().GetCell(id + 1);
		if (pCell != NULL)
		{
			pCell->OnAddPlayer(player);
			OnRemovePlayer(player);
		}
	}
}

bool moving(Position3D& currpos, Position3D& dstPos, Direction3D& direction, float velocity_, float distance_)
{
	Position3D currpos_backup = currpos;
	Vector3 movement = dstPos - currpos;

	bool ret = true;
	float dist_len = KBEVec3Length(&movement);

	if (dist_len < velocity_ + distance_)
	{
		float y = currpos.y;

		if (distance_ > 0.0f)
		{
			// 单位化向量
			KBEVec3Normalize(&movement, &movement);

			if (dist_len > distance_)
			{
				movement *= distance_;
				currpos = dstPos - movement;
			}
		}
		else
		{
			currpos = dstPos;
		}

		ret = false;
	}
	else
	{
		// 单位化向量
		KBEVec3Normalize(&movement, &movement);

		// 移动位置
		movement *= velocity_;
		currpos += movement;
	}

	return ret;
}

void Cell::update()
{
	auto it = m_pMoving.begin();
	for (; it != m_pMoving.end();)
	{
		Player* player = it->second;
		if (!moving(player->currpos, player->dstpos, player->direction, player->speed, 0))
		{
			it = m_pMoving.erase(it);
			OnMoveOver(player);			
		}
		else
			++it;
		//std::cout << "cellid = " << id << "\n";
		//std::cout << "playerid = " << player->GetPlayerID() << " curr_x: " << player->currpos.x << " curr_y:" << player->currpos.y 
		//	<< " dst_x:" << player->dstpos.x << " dst_y:" << player->dstpos.y << "\n";
	}
}

void Cell::BrocastPlayerMessage(Player* player, void(Player::*pfunc)(void*), void* param)
{
	std::unordered_map<int, Player*>::iterator it = m_pPlayer.begin();
	for (; it != m_pPlayer.end(); ++it)
	{
		if (it->second->GetPlayerID() == player->GetPlayerID())
			continue;

		(it->second->*pfunc)(param);
	}

	for (int i = 0; i < (int)m_NearCell.size(); ++i)
	{
		m_NearCell[i]->OnBrocastPlayerMessage(player, pfunc, param);
	}
}

void Cell::OnBrocastPlayerMessage(Player* player, void(Player::*pfunc)(void*), void* param)
{
	std::unordered_map<int, Player*>::iterator it = m_pPlayer.begin();
	for (; it != m_pPlayer.end(); ++it)
	{
		(it->second->*pfunc)(param);
	}
}