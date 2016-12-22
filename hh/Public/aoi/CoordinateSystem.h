#pragma once
#include <list>

namespace AOI
{
	class CoordinateNode;
	class CoordinateSystem
	{
	public:
		CoordinateSystem();
		~CoordinateSystem();

		bool insert(CoordinateNode* pNode);

		/**
		将节点从list中移除
		*/
		bool remove(CoordinateNode* pNode);
		bool removeReal(CoordinateNode* pNode);
		void removeDelNodes();
		void releaseNodes();

		/**
		当某个节点有变动时，需要更新它在list中的
		相关位置等信息
		*/
		void update(CoordinateNode* pNode);

	protected:
		bool isEmpty();

		void moveNodeX(CoordinateNode* pNode, float px, CoordinateNode* pCurrNode);
		void moveNodeY(CoordinateNode* pNode, float py, CoordinateNode* pCurrNode);
		void moveNodeZ(CoordinateNode* pNode, float pz, CoordinateNode* pCurrNode);

	private:
		unsigned int size_;

		// 链表的首尾指针
		CoordinateNode* first_x_coordinateNode_;
		CoordinateNode* first_y_coordinateNode_;
		CoordinateNode* first_z_coordinateNode_;

		std::list<CoordinateNode*> dels_;
		size_t dels_count_;

		std::list<CoordinateNode*> releases_;

		int updating_;
	};
}