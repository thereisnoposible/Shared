#pragma once
#include <list>
#include <memory>
#include <set>
#include <vector>

namespace xlib
{
	struct POSLocal
	{
		POSLocal() :para(nullptr)
		{

		}

		long long getID()
		{
			return ((long long)(x) << 32) + y;
		}

		int x;
		int y;
		int size;
		int distance;
		int dis_size;
		std::shared_ptr<POSLocal> para;
	};

	class AStar
	{
	public:
		AStar() :map(nullptr), map_x(0), map_y(0){}
		void LoadMap(char*_map, int _x, int _y);
		std::vector<POSLocal> GetDistance(POSLocal& curr, POSLocal&dst);

	private:
		int calc_distance(int x, int y, int dst_x, int dst_y);
		std::vector<std::shared_ptr<POSLocal>> findohterpos(std::shared_ptr<POSLocal>& the, POSLocal& dst);
		std::shared_ptr<POSLocal> getBestPOS();
		void InsertOpenPOS(std::shared_ptr<POSLocal>& temp);

		std::set<long long> closelist;
		std::list<std::shared_ptr<POSLocal>> openlist;
		char* map;
		int map_x;
		int map_y;
	};
}