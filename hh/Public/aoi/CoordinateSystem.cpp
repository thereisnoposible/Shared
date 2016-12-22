#include "CoordinateSystem.h"
#include "CoordinateNode.h"
#include <iostream>

namespace AOI
{
	CoordinateSystem::CoordinateSystem() :
		size_(0),
		first_x_coordinateNode_(NULL),
		first_y_coordinateNode_(NULL),
		first_z_coordinateNode_(NULL),
		dels_(),
		dels_count_(0),
		updating_(0)
	{
	}

	CoordinateSystem::~CoordinateSystem()
	{
		dels_.clear();
		dels_count_ = 0;

		if (first_x_coordinateNode_)
		{
			CoordinateNode* pNode = first_x_coordinateNode_;
			while (pNode != NULL)
			{
				CoordinateNode* pNextNode = pNode->pNextX();
				pNode->pCoordinateSystem(NULL);
				pNode->pPrevX(NULL);
				pNode->pNextX(NULL);
				pNode->pPrevY(NULL);
				pNode->pNextY(NULL);
				pNode->pPrevZ(NULL);
				pNode->pNextZ(NULL);

				delete pNode;

				pNode = pNextNode;
			}

			// 上面已经销毁过了
			first_x_coordinateNode_ = NULL;
			first_y_coordinateNode_ = NULL;
			first_z_coordinateNode_ = NULL;
		}
	}

	bool CoordinateSystem::insert(CoordinateNode* pNode)
	{
		// 如果链表是空的, 初始第一个和最后一个xz节点为该节点
		if (isEmpty())
		{
			first_x_coordinateNode_ = pNode;

			first_y_coordinateNode_ = pNode;

			first_z_coordinateNode_ = pNode;

			pNode->pPrevX(NULL);
			pNode->pNextX(NULL);
			pNode->pPrevY(NULL);
			pNode->pNextY(NULL);
			pNode->pPrevZ(NULL);
			pNode->pNextZ(NULL);
			pNode->x(pNode->xx());
			pNode->y(pNode->yy());
			pNode->z(pNode->zz());
			pNode->pCoordinateSystem(this);

			size_ = 1;

			// 只有一个节点不需要更新
			// update(pNode);
			pNode->resetOld();
			return true;
		}

		pNode->old_xx(-FLT_MAX);
		pNode->old_yy(-FLT_MAX);
		pNode->old_zz(-FLT_MAX);

		pNode->x(first_x_coordinateNode_->x());
		first_x_coordinateNode_->pPrevX(pNode);
		pNode->pNextX(first_x_coordinateNode_);
		first_x_coordinateNode_ = pNode;

		
		pNode->y(first_y_coordinateNode_->y());
		first_y_coordinateNode_->pPrevY(pNode);
		pNode->pNextY(first_y_coordinateNode_);
		first_y_coordinateNode_ = pNode;		

		pNode->z(first_z_coordinateNode_->z());
		first_z_coordinateNode_->pPrevZ(pNode);
		pNode->pNextZ(first_z_coordinateNode_);
		first_z_coordinateNode_ = pNode;

		pNode->pCoordinateSystem(this);
		++size_;

		update(pNode);
		return true;
	}

	void CoordinateSystem::update(CoordinateNode* pNode)
	{
		// 没有计数器支持，这个标记很可能中途被update子分支取消，因此没有意义
		//pNode->addFlags(COORDINATE_NODE_FLAG_PENDING);

		++updating_;

		if (pNode->xx() != pNode->old_xx())
		{
			while (true)
			{
				CoordinateNode* pCurrNode = pNode->pPrevX();
				while (pCurrNode && pCurrNode != pNode && pCurrNode->x() > pNode->xx())
				{
					pNode->x(pCurrNode->x());

					// 先把节点移动过去
					moveNodeX(pNode, pNode->xx(), pCurrNode);

					if (!pNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{
						pCurrNode->onNodePassX(pNode, false);
					}

					if (!pCurrNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{
						pNode->onNodePassX(pCurrNode, true);
					}

					if (pNode->pPrevX() == NULL)
						break;

					pCurrNode = pNode->pPrevX();
				}

				pCurrNode = pNode->pNextX();
				while (pCurrNode && pCurrNode != pNode && pCurrNode->x() < pNode->xx())
				{
					pNode->x(pCurrNode->x());

					// 先把节点移动过去
					moveNodeX(pNode, pNode->xx(), pCurrNode);

					if (!pNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{
						pCurrNode->onNodePassX(pNode, true);
					}

					if (!pCurrNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{
						pNode->onNodePassX(pCurrNode, false);
					}

					if (pNode->pNextX() == NULL)
						break;

					pCurrNode = pNode->pNextX();
				}

				if ((pNode->pPrevX() == NULL || (pNode->xx() >= pNode->pPrevX()->x())) &&
					(pNode->pNextX() == NULL || (pNode->xx() <= pNode->pNextX()->x())))
				{
					pNode->x(pNode->xx());
					break;
				}
			}
		}

		//if (CoordinateSystem::hasY && pNode->yy() != pNode->old_yy())
		{
			while (true)
			{
				CoordinateNode* pCurrNode = pNode->pPrevY();
				while (pCurrNode && pCurrNode != pNode && pCurrNode->y() > pNode->yy())
				{
					pNode->y(pCurrNode->y());

					// 先把节点移动过去
					moveNodeY(pNode, pNode->yy(), pCurrNode);

					if (!pNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{
						pCurrNode->onNodePassY(pNode, false);
					}

					if (!pCurrNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{
						pNode->onNodePassY(pCurrNode, true);
					}

					if (pNode->pPrevY() == NULL)
						break;

					pCurrNode = pNode->pPrevY();
				}

				pCurrNode = pNode->pNextY();
				while (pCurrNode && pCurrNode != pNode && pCurrNode->y() < pNode->yy())
				{
					pNode->y(pCurrNode->y());

					// 先把节点移动过去
					moveNodeY(pNode, pNode->yy(), pCurrNode);

					if (!pNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{

						pCurrNode->onNodePassY(pNode, true);
					}

					if (!pCurrNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{

						pNode->onNodePassY(pCurrNode, false);
					}

					if (pNode->pNextY() == NULL)
						break;

					pCurrNode = pNode->pNextY();
				}

				if ((pNode->pPrevY() == NULL || (pNode->yy() >= pNode->pPrevY()->y())) &&
					(pNode->pNextY() == NULL || (pNode->yy() <= pNode->pNextY()->y())))
				{
					pNode->y(pNode->yy());
					break;
				}
			}
		}

		if (pNode->zz() != pNode->old_zz())
		{
			while (true)
			{
				CoordinateNode* pCurrNode = pNode->pPrevZ();
				while (pCurrNode && pCurrNode != pNode && pCurrNode->z() > pNode->zz())
				{
					pNode->z(pCurrNode->z());

					// 先把节点移动过去
					moveNodeZ(pNode, pNode->zz(), pCurrNode);

					if (!pNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{

						pCurrNode->onNodePassZ(pNode, false);
					}

					if (!pCurrNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{

						pNode->onNodePassZ(pCurrNode, true);
					}

					if (pNode->pPrevZ() == NULL)
						break;

					pCurrNode = pNode->pPrevZ();
				}

				pCurrNode = pNode->pNextZ();
				while (pCurrNode && pCurrNode != pNode && pCurrNode->z() < pNode->zz())
				{
					pNode->z(pCurrNode->z());

					// 先把节点移动过去
					moveNodeZ(pNode, pNode->zz(), pCurrNode);

					if (!pNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{

						pCurrNode->onNodePassZ(pNode, true);
					}

					if (!pCurrNode->hasFlags(COORDINATE_NODE_FLAG_HIDE_OR_REMOVED))
					{

						pNode->onNodePassZ(pCurrNode, false);
					}

					if (pNode->pNextZ() == NULL)
						break;

					pCurrNode = pNode->pNextZ();
				}

				if ((pNode->pPrevZ() == NULL || (pNode->zz() >= pNode->pPrevZ()->z())) &&
					(pNode->pNextZ() == NULL || (pNode->zz() <= pNode->pNextZ()->z())))
				{
					pNode->z(pNode->zz());
					break;
				}
			}
		}


		pNode->resetOld();
		//pNode->removeFlags(COORDINATE_NODE_FLAG_PENDING);
		--updating_;

		//if (updating_ == 0)
		//	releaseNodes();
	}

	void CoordinateSystem::removeDelNodes()
	{
		if (dels_count_ == 0)
			return;

		std::list<CoordinateNode*>::iterator iter = dels_.begin();
		for (; iter != dels_.end(); ++iter)
		{
			removeReal((*iter));
		}

		dels_.clear();
		dels_count_ = 0;
	}

	void CoordinateSystem::releaseNodes()
	{
		removeDelNodes();

		std::list<CoordinateNode*>::reverse_iterator iter = releases_.rbegin();
		for (; iter != releases_.rend(); ++iter)
		{
			delete (*iter);
		}

		releases_.clear();
	}

	bool CoordinateSystem::removeReal(CoordinateNode* pNode)
	{
		if (pNode->pCoordinateSystem() == NULL)
		{
			return true;
		}

		// 如果是第一个节点
		if (first_x_coordinateNode_ == pNode)
		{
			first_x_coordinateNode_ = first_x_coordinateNode_->pNextX();

			if (first_x_coordinateNode_)
			{
				first_x_coordinateNode_->pPrevX(NULL);
			}
		}
		else
		{
			pNode->pPrevX()->pNextX(pNode->pNextX());

			if (pNode->pNextX())
				pNode->pNextX()->pPrevX(pNode->pPrevX());
		}

		
			// 如果是第一个节点
			if (first_y_coordinateNode_ == pNode)
			{
				first_y_coordinateNode_ = first_y_coordinateNode_->pNextY();

				if (first_y_coordinateNode_)
				{
					first_y_coordinateNode_->pPrevY(NULL);
				}
			}
			else
			{
				pNode->pPrevY()->pNextY(pNode->pNextY());

				if (pNode->pNextY())
					pNode->pNextY()->pPrevY(pNode->pPrevY());
			}
		

		// 如果是第一个节点
		if (first_z_coordinateNode_ == pNode)
		{
			first_z_coordinateNode_ = first_z_coordinateNode_->pNextZ();

			if (first_z_coordinateNode_)
			{
				first_z_coordinateNode_->pPrevZ(NULL);
			}
		}
		else
		{
			pNode->pPrevZ()->pNextZ(pNode->pNextZ());

			if (pNode->pNextZ())
				pNode->pNextZ()->pPrevZ(pNode->pPrevZ());
		}

		pNode->pPrevX(NULL);
		pNode->pNextX(NULL);
		pNode->pPrevY(NULL);
		pNode->pNextY(NULL);
		pNode->pPrevZ(NULL);
		pNode->pNextZ(NULL);
		pNode->pCoordinateSystem(NULL);

		releases_.push_back(pNode);

		--size_;
		return true;
	}

	void CoordinateSystem::moveNodeX(CoordinateNode* pNode, float px, CoordinateNode* pCurrNode)
	{
		if (pCurrNode != NULL)
		{
			if (pCurrNode->x() > px)
			{
				CoordinateNode* pPreNode = pCurrNode->pPrevX();
				pCurrNode->pPrevX(pNode);
				if (pPreNode)
				{
					pPreNode->pNextX(pNode);
					if (pNode == first_x_coordinateNode_ && pNode->pNextX())
						first_x_coordinateNode_ = pNode->pNextX();
				}
				else
				{
					first_x_coordinateNode_ = pNode;
				}

				if (pNode->pPrevX())
					pNode->pPrevX()->pNextX(pNode->pNextX());

				if (pNode->pNextX())
					pNode->pNextX()->pPrevX(pNode->pPrevX());

				pNode->pPrevX(pPreNode);
				pNode->pNextX(pCurrNode);
			}
			else
			{
				CoordinateNode* pNextNode = pCurrNode->pNextX();
				if (pNextNode != pNode)
				{
					pCurrNode->pNextX(pNode);
					if (pNextNode)
						pNextNode->pPrevX(pNode);

					if (pNode->pPrevX())
						pNode->pPrevX()->pNextX(pNode->pNextX());

					if (pNode->pNextX())
					{
						pNode->pNextX()->pPrevX(pNode->pPrevX());

						if (pNode == first_x_coordinateNode_)
							first_x_coordinateNode_ = pNode->pNextX();
					}

					pNode->pPrevX(pCurrNode);
					pNode->pNextX(pNextNode);
				}
			}
		}
	}

	void CoordinateSystem::moveNodeY(CoordinateNode* pNode, float py, CoordinateNode* pCurrNode)
	{
		if (pCurrNode != NULL)
		{
			if (pCurrNode->y() > py)
			{
				CoordinateNode* pPreNode = pCurrNode->pPrevY();
				pCurrNode->pPrevY(pNode);
				if (pPreNode)
				{
					pPreNode->pNextY(pNode);
					if (pNode == first_y_coordinateNode_ && pNode->pNextY())
						first_y_coordinateNode_ = pNode->pNextY();
				}
				else
				{
					first_y_coordinateNode_ = pNode;
				}

				if (pNode->pPrevY())
					pNode->pPrevY()->pNextY(pNode->pNextY());

				if (pNode->pNextY())
					pNode->pNextY()->pPrevY(pNode->pPrevY());

				pNode->pPrevY(pPreNode);
				pNode->pNextY(pCurrNode);
			}
			else
			{
				CoordinateNode* pNextNode = pCurrNode->pNextY();
				if (pNextNode != pNode)
				{
					pCurrNode->pNextY(pNode);
					if (pNextNode)
						pNextNode->pPrevY(pNode);

					if (pNode->pPrevY())
						pNode->pPrevY()->pNextY(pNode->pNextY());

					if (pNode->pNextY())
					{
						pNode->pNextY()->pPrevY(pNode->pPrevY());

						if (pNode == first_y_coordinateNode_)
							first_y_coordinateNode_ = pNode->pNextY();
					}

					pNode->pPrevY(pCurrNode);
					pNode->pNextY(pNextNode);
				}
			}
		}
	}

	void CoordinateSystem::moveNodeZ(CoordinateNode* pNode, float pz, CoordinateNode* pCurrNode)
	{
		if (pCurrNode != NULL)
		{
			if (pCurrNode->z() > pz)
			{
				CoordinateNode* pPreNode = pCurrNode->pPrevZ();
				pCurrNode->pPrevZ(pNode);
				if (pPreNode)
				{
					pPreNode->pNextZ(pNode);
					if (pNode == first_z_coordinateNode_ && pNode->pNextZ())
						first_z_coordinateNode_ = pNode->pNextZ();
				}
				else
				{
					first_z_coordinateNode_ = pNode;
				}

				if (pNode->pPrevZ())
					pNode->pPrevZ()->pNextZ(pNode->pNextZ());

				if (pNode->pNextZ())
					pNode->pNextZ()->pPrevZ(pNode->pPrevZ());

				pNode->pPrevZ(pPreNode);
				pNode->pNextZ(pCurrNode);
			}
			else
			{
				CoordinateNode* pNextNode = pCurrNode->pNextZ();
				if (pNextNode != pNode)
				{
					pCurrNode->pNextZ(pNode);
					if (pNextNode)
						pNextNode->pPrevZ(pNode);

					if (pNode->pPrevZ())
						pNode->pPrevZ()->pNextZ(pNode->pNextZ());

					if (pNode->pNextZ())
					{
						pNode->pNextZ()->pPrevZ(pNode->pPrevZ());

						if (pNode == first_z_coordinateNode_)
							first_z_coordinateNode_ = pNode->pNextZ();
					}

					pNode->pPrevZ(pCurrNode);
					pNode->pNextZ(pNextNode);
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------
	bool CoordinateSystem::remove(CoordinateNode* pNode)
	{
		pNode->addFlags(COORDINATE_NODE_FLAG_REMOVEING);
		pNode->onRemove();
		update(pNode);

		pNode->addFlags(COORDINATE_NODE_FLAG_REMOVED);

		// 由于在update过程中可能会因为多级update的进行导致COORDINATE_NODE_FLAG_PENDING标志被取消，因此此处并不能很好的判断
		// 除非实现了标记的计数器，这里强制所有的行为都放入dels_， 由releaseNodes在space中进行调用统一释放
		if (true /*pNode->hasFlags(COORDINATE_NODE_FLAG_PENDING)*/)
		{
			std::list<CoordinateNode*>::iterator iter = std::find(dels_.begin(), dels_.end(), pNode);
			if (iter == dels_.end())
			{
				dels_.push_back(pNode);
				++dels_count_;
			}
		}
		else
		{
			removeReal(pNode);
		}

		return true;
	}

	bool CoordinateSystem::isEmpty()
	{
		return first_x_coordinateNode_ == NULL && first_y_coordinateNode_ == NULL && first_z_coordinateNode_ == NULL;
	}
}