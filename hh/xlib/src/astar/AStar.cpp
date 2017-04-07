#include "../include/astar/AStar.h"

namespace xlib
{
	void AStar::LoadMap(char*_map, int _x, int _y)
	{
		map = _map;
		map_x = _x;
		map_y = _y;
	}

	std::vector<POSLocal> AStar::GetDistance(POSLocal& curr, POSLocal&dst)
	{
		std::vector<POSLocal> distan;

		if (curr.x<0 || curr.x>map_x || curr.y<0 || curr.y>map_y
			|| dst.x<0 || dst.x>map_x || dst.y<0 || dst.y>map_y
			|| map[dst.x*map_y + dst.y] == 1 || map[curr.x*map_y + curr.y] == 1)
		{
			return distan;
		}
		openlist.clear();
		closelist.clear();
		std::shared_ptr<POSLocal> pCurr(new POSLocal);
		pCurr->x = curr.x;
		pCurr->y = curr.y;
		pCurr->distance = calc_distance(pCurr->x, pCurr->y, dst.x, dst.y);
		pCurr->size = 0;
		pCurr->dis_size = pCurr->distance + pCurr->size;
		openlist.push_back(pCurr);
		closelist.insert(curr.getID());

		std::shared_ptr<POSLocal>& best = getBestPOS();
		while (!(best->x == dst.x && best->y == dst.y))
		{
			std::vector<std::shared_ptr<POSLocal>>& other = findohterpos(best, dst);

			openlist.pop_front();
			closelist.insert(best->getID());
			for (int i = 0; i < (int)other.size(); i++)
			{
				if (closelist.find(other[i]->getID()) != closelist.end())
					continue;

				for (std::list<std::shared_ptr<POSLocal>>::iterator it = openlist.begin(); it != openlist.end(); ++it)
				{
					if (it->get()->x == other[i]->x && it->get()->y == other[i]->y)
					{
						if (it->get()->dis_size < other[i]->dis_size)
						{
							other[i]->para = it->get()->para;
						}
						break;
					}
				}
				InsertOpenPOS(other[i]);
			}
			if (openlist.size() == 0)
			{
				return distan;
			}
			best = getBestPOS();
		}

		while (best.get() != nullptr)
		{
			distan.push_back(*best);
			best = best->para;
		}

		return distan;
	}

	int AStar::calc_distance(int x, int y, int dst_x, int dst_y)
	{
		return (x - dst_x)*(x - dst_x) + (y - dst_y)*(y - dst_y);
	}

	std::vector<std::shared_ptr<POSLocal>> AStar::findohterpos(std::shared_ptr<POSLocal>& the, POSLocal& dst)
	{
		std::vector<std::shared_ptr<POSLocal>> other;

		if (the->x - 1 >= 0)
		{
			if (map[(the->x - 1) * map_y + the->y] != 1)
			{
				std::shared_ptr<POSLocal> temp(new POSLocal);
				temp->x = the->x - 1;
				temp->y = the->y;
				temp->para = the;
				temp->size = temp->para->size + 1;
				temp->distance = calc_distance(temp->x, temp->y, dst.x, dst.y);
				temp->dis_size = temp->size + temp->distance;
				other.push_back(temp);
			}
		}
		if (the->x + 1 < map_x)
		{
			if (map[(the->x + 1) * map_y + the->y] != 1)
			{
				std::shared_ptr<POSLocal> temp(new POSLocal);
				temp->x = the->x + 1;
				temp->y = the->y;
				temp->para = the;
				temp->size = temp->para->size + 1;
				temp->distance = calc_distance(temp->x, temp->y, dst.x, dst.y);
				temp->dis_size = temp->size + temp->distance;
				other.push_back(temp);
			}
		}
		if (the->y - 1 >= 0)
		{
			if (map[the->x * map_y + the->y - 1] != 1)
			{
				std::shared_ptr<POSLocal> temp(new POSLocal);
				temp->x = the->x;
				temp->y = the->y - 1;
				temp->para = the;
				temp->size = temp->para->size + 1;
				temp->distance = calc_distance(temp->x, temp->y, dst.x, dst.y);
				temp->dis_size = temp->size + temp->distance;
				other.push_back(temp);
			}
		}
		if (the->y + 1 < map_y)
		{
			if (map[the->x * map_y + (the->y + 1)] != 1)
			{
				std::shared_ptr<POSLocal> temp(new POSLocal);
				temp->x = the->x;
				temp->y = the->y + 1;
				temp->para = the;
				temp->size = temp->para->size + 1;
				temp->distance = calc_distance(temp->x, temp->y, dst.x, dst.y);
				temp->dis_size = temp->size + temp->distance;
				other.push_back(temp);
			}
		}
		return other;
	}

	std::shared_ptr<POSLocal> AStar::getBestPOS()
	{
		return openlist.front();
	}

	void AStar::InsertOpenPOS(std::shared_ptr<POSLocal>& temp)
	{
		std::list<std::shared_ptr<POSLocal>>::iterator fir = openlist.begin();
		bool bInsert = false;
		for (; fir != openlist.end(); ++fir)
		{
			if (fir->get()->dis_size > temp->dis_size)
			{
				openlist.insert(fir, (temp));
				bInsert = true;
				break;
			}
		}
		if (bInsert == false)
		{
			openlist.push_back(temp);
		}
	}
}