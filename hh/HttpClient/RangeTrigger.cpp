#include "RangeTrigger.h"
#include "CoordinateNode.h"
#include "RangeTriggerNode.h"

#include <math.h>

namespace AOI
{
	RangeTrigger::RangeTrigger(CoordinateNode* origin, float xz, float y):
	range_xz_(fabs(xz)),
	range_y_(fabs(y)),
	origin_(origin),
	positiveBoundary_(NULL),
	negativeBoundary_(NULL)
	{

	}
	RangeTrigger::~RangeTrigger()
	{
		uninstall();
	}

	bool RangeTrigger::install()
	{
		if (positiveBoundary_ == NULL)
			positiveBoundary_ = new RangeTriggerNode(this, 0, 0);
		else
			positiveBoundary_->range(0.0f, 0.0f);

		if (negativeBoundary_ == NULL)
			negativeBoundary_ = new RangeTriggerNode(this, 0, 0);
		else
			negativeBoundary_->range(0.0f, 0.0f);

		origin_->pCoordinateSystem()->insert(positiveBoundary_);
		origin_->pCoordinateSystem()->insert(negativeBoundary_);

		positiveBoundary_->old_xx(FLT_MAX);
		positiveBoundary_->old_yy(FLT_MAX);
		positiveBoundary_->old_zz(FLT_MAX);

		positiveBoundary_->range(range_xz_, range_y_);
		positiveBoundary_->old_range(range_xz_, range_y_);
		positiveBoundary_->update();

		negativeBoundary_->old_xx(-FLT_MAX);
		negativeBoundary_->old_yy(-FLT_MAX);
		negativeBoundary_->old_zz(-FLT_MAX);
		negativeBoundary_->range(-range_xz_, -range_y_);
		negativeBoundary_->old_range(-range_xz_, -range_y_);
		negativeBoundary_->update();
		return true;
	}
	bool RangeTrigger::uninstall()
	{
		if (positiveBoundary_ && positiveBoundary_->pCoordinateSystem())
		{
			positiveBoundary_->pRangeTrigger(NULL);
			positiveBoundary_->pCoordinateSystem()->remove(positiveBoundary_);
		}

		if (negativeBoundary_ && negativeBoundary_->pCoordinateSystem())
		{
			negativeBoundary_->pRangeTrigger(NULL);
			negativeBoundary_->pCoordinateSystem()->remove(negativeBoundary_);
		}

		// 此处不必release node， 节点的释放统一交给CoordinateSystem
		positiveBoundary_ = NULL;
		negativeBoundary_ = NULL;
		return true;
	}
	bool RangeTrigger::reinstall(CoordinateNode* pCoordinateNode)
	{
		uninstall();
		origin_ = pCoordinateNode;
		return install();
	}

	//-------------------------------------------------------------------------------------
	void RangeTrigger::onNodePassX(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront)
	{
		if (pNode == origin())
			return;

		bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
		bool isInZ = pRangeTriggerNode->isInZRange(pNode);

		if (wasInZ != isInZ)
			return;

		bool wasIn = false;
		bool isIn = false;

		//if (CoordinateSystem::hasY)
		{
			bool wasInY = pRangeTriggerNode->wasInYRange(pNode);
			bool isInY = pRangeTriggerNode->isInYRange(pNode);

			if (wasInY != isInY)
				return;

			wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInY && wasInZ;
			isIn = pRangeTriggerNode->isInXRange(pNode) && isInY && isInZ;
		}
		//else
		//{
		//	wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInZ;
		//	isIn = pRangeTriggerNode->isInXRange(pNode) && isInZ;
		//}

		if (wasIn == isIn)
			return;

		if (isIn)
		{
			this->onEnter(pNode);
		}
		else
		{
			this->onLeave(pNode);
		}
	}

	//-------------------------------------------------------------------------------------
	bool RangeTriggerNode::wasInYRange(CoordinateNode * pNode)
	{
		float originY = old_yy() - old_range_y_;

		volatile float lowerBound = originY - fabs(old_range_y_);
		volatile float upperBound = originY + fabs(old_range_y_);
		return (lowerBound < pNode->old_yy()) && (pNode->old_yy() <= upperBound);
	}

	//-------------------------------------------------------------------------------------
	void RangeTrigger::onNodePassY(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront)
	{
		if (pNode == origin())
			return;

		bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
		bool isInZ = pRangeTriggerNode->isInZRange(pNode);

		if (wasInZ != isInZ)
			return;

		bool wasInY = pRangeTriggerNode->wasInYRange(pNode);
		bool isInY = pRangeTriggerNode->isInYRange(pNode);

		if (wasInY == isInY)
			return;

		bool wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInY && wasInZ;
		bool isIn = pRangeTriggerNode->isInXRange(pNode) && isInY && isInZ;

		if (wasIn == isIn)
			return;

		if (isIn)
		{
			this->onEnter(pNode);
		}
		else
		{
			this->onLeave(pNode);
		}
	}

	//-------------------------------------------------------------------------------------
	void RangeTrigger::onNodePassZ(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront)
	{
		if (pNode == origin())
			return;

		//if (CoordinateSystem::hasY)
		{
			bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
			bool isInZ = pRangeTriggerNode->isInZRange(pNode);

			if (wasInZ == isInZ)
				return;

			bool wasIn = pRangeTriggerNode->wasInXRange(pNode) &&
				pRangeTriggerNode->wasInYRange(pNode) &&
				wasInZ;

			bool isIn = pRangeTriggerNode->isInXRange(pNode) &&
				pRangeTriggerNode->isInYRange(pNode) &&
				isInZ;

			if (wasIn == isIn)
				return;

			if (isIn)
			{
				this->onEnter(pNode);
			}
			else
			{
				this->onLeave(pNode);
			}
		}
		//else
		//{
		//	bool wasInZ = pRangeTriggerNode->wasInZRange(pNode);
		//	bool isInZ = pRangeTriggerNode->isInZRange(pNode);

		//	if (wasInZ == isInZ)
		//		return;

		//	bool wasIn = pRangeTriggerNode->wasInXRange(pNode) && wasInZ;
		//	bool isIn = pRangeTriggerNode->isInXRange(pNode) && isInZ;

		//	if (wasIn == isIn)
		//		return;

		//	if (isIn)
		//	{
		//		this->onEnter(pNode);
		//	}
		//	else
		//	{
		//		this->onLeave(pNode);
		//	}
		//}
	}

	//-------------------------------------------------------------------------------------
	void RangeTrigger::update(float xz, float y)
	{
		float old_range_xz_ = range_xz_;
		float old_range_y_ = range_y_;

		range(xz, y);

		if (positiveBoundary_)
		{
			positiveBoundary_->range(range_xz_, range_y_);
			positiveBoundary_->old_range(old_range_xz_, old_range_y_);
			positiveBoundary_->update();
		}

		if (negativeBoundary_)
		{
			negativeBoundary_->range(-range_xz_, -range_y_);
			negativeBoundary_->old_range(-old_range_xz_, -old_range_y_);
			negativeBoundary_->update();
		}
	}

	//-------------------------------------------------------------------------------------
	void RangeTrigger::range(float xz, float y)
	{
		range_xz_ = fabs(xz);
		range_y_ = fabs(y);
	}

	//-------------------------------------------------------------------------------------
	float RangeTrigger::range_xz() const
	{
		return range_xz_;
	}

	//-------------------------------------------------------------------------------------
	float RangeTrigger::range_y() const
	{
		return range_y_;
	}

	//-------------------------------------------------------------------------------------
	CoordinateNode* RangeTrigger::origin() const
	{
		return origin_;
	}

	//-------------------------------------------------------------------------------------
	void RangeTrigger::origin(CoordinateNode* pCoordinateNode)
	{
		origin_ = pCoordinateNode;
	}
}