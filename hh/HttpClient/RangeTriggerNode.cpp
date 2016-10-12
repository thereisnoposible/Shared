#include "RangeTriggerNode.h"
#include "EntityCoordinateNode.h"
#include "RangeTrigger.h"

namespace AOI
{
	RangeTriggerNode::RangeTriggerNode(RangeTrigger* pRangeTrigger, float xz, float y) :
		CoordinateNode(NULL),
		range_xz_(xz),
		range_y_(y),
		old_range_xz_(range_xz_),
		old_range_y_(range_y_),
		pRangeTrigger_(pRangeTrigger)
	{
		flags(COORDINATE_NODE_FLAG_HIDE);

		static_cast<EntityCoordinateNode*>(pRangeTrigger_->origin())->addWatcherNode(this);
	}

	RangeTriggerNode::~RangeTriggerNode()
	{
		static_cast<EntityCoordinateNode*>(pRangeTrigger_->origin())->delWatcherNode(this);
	}

	//-------------------------------------------------------------------------------------
	void RangeTriggerNode::onParentRemove(CoordinateNode* pParentNode)
	{
		if ((flags() & COORDINATE_NODE_FLAG_REMOVEING) <= 0)
			pParentNode->pCoordinateSystem()->remove(this);
	}

	//-------------------------------------------------------------------------------------
	float RangeTriggerNode::xx() const
	{
		if ((flags() & COORDINATE_NODE_FLAG_REMOVED) > 0 || pRangeTrigger_ == NULL)
			return -FLT_MAX;

		return pRangeTrigger_->origin()->xx() + range_xz_;
	}

	//-------------------------------------------------------------------------------------
	float RangeTriggerNode::yy() const
	{
		if ((flags() & COORDINATE_NODE_FLAG_REMOVED) > 0 || pRangeTrigger_ == NULL)
			return -FLT_MAX;

		return pRangeTrigger_->origin()->yy() + range_y_;
	}

	//-------------------------------------------------------------------------------------
	float RangeTriggerNode::zz() const
	{
		if ((flags() & COORDINATE_NODE_FLAG_REMOVED) > 0 || pRangeTrigger_ == NULL)
			return -FLT_MAX;

		return pRangeTrigger_->origin()->zz() + range_xz_;
	}

	//-------------------------------------------------------------------------------------
	void RangeTriggerNode::onNodePassX(CoordinateNode* pNode, bool isfront)
	{
		if ((flags() & COORDINATE_NODE_FLAG_REMOVED) <= 0 && pRangeTrigger_)
			pRangeTrigger_->onNodePassX(this, pNode, isfront);
	}

	//-------------------------------------------------------------------------------------
	void RangeTriggerNode::onNodePassY(CoordinateNode* pNode, bool isfront)
	{
		if ((flags() & COORDINATE_NODE_FLAG_REMOVED) <= 0 && pRangeTrigger_)
			pRangeTrigger_->onNodePassY(this, pNode, isfront);
	}

	//-------------------------------------------------------------------------------------
	void RangeTriggerNode::onNodePassZ(CoordinateNode* pNode, bool isfront)
	{
		if ((flags() & COORDINATE_NODE_FLAG_REMOVED) <= 0 && pRangeTrigger_)
			pRangeTrigger_->onNodePassZ(this, pNode, isfront);
	}

	//-------------------------------------------------------------------------------------
	bool RangeTriggerNode::isInXRange(CoordinateNode * pNode)
	{
		float originX = pRangeTrigger_->origin()->xx();

		volatile float lowerBound = originX - fabs(range_xz_);
		volatile float upperBound = originX + fabs(range_xz_);
		return (lowerBound < pNode->xx()) && (pNode->xx() <= upperBound);
	}

	//-------------------------------------------------------------------------------------
	bool RangeTriggerNode::isInYRange(CoordinateNode * pNode)
	{
		float originY = pRangeTrigger_->origin()->yy();

		volatile float lowerBound = originY - fabs(range_y_);
		volatile float upperBound = originY + fabs(range_y_);
		return (lowerBound < pNode->yy()) && (pNode->yy() <= upperBound);
	}

	//-------------------------------------------------------------------------------------
	bool RangeTriggerNode::isInZRange(CoordinateNode * pNode)
	{
		float originZ = pRangeTrigger_->origin()->zz();

		volatile float lowerBound = originZ - fabs(range_xz_);
		volatile float upperBound = originZ + fabs(range_xz_);
		return (lowerBound < pNode->zz()) && (pNode->zz() <= upperBound);
	}

	//-------------------------------------------------------------------------------------
	bool RangeTriggerNode::wasInXRange(CoordinateNode * pNode)
	{
		float originX = old_xx() - old_range_xz_;

		volatile float lowerBound = originX - fabs(old_range_xz_);
		volatile float upperBound = originX + fabs(old_range_xz_);
		return (lowerBound < pNode->old_xx()) && (pNode->old_xx() <= upperBound);
	}

	//-------------------------------------------------------------------------------------
	bool RangeTriggerNode::wasInZRange(CoordinateNode * pNode)
	{
		float originZ = old_zz() - old_range_xz_;

		volatile float lowerBound = originZ - fabs(old_range_xz_);
		volatile float upperBound = originZ + fabs(old_range_xz_);
		return (lowerBound < pNode->old_zz()) && (pNode->old_zz() <= upperBound);
	}

	//-------------------------------------------------------------------------------------
	void RangeTriggerNode::range(float xz, float y)
	{
		range_xz_ = xz;
		range_y_ = y;
	}

	//-------------------------------------------------------------------------------------
	void RangeTriggerNode::old_range(float xz, float y)
	{
		old_range_xz_ = xz;
		old_range_y_ = y;
	}

	//-------------------------------------------------------------------------------------
	float RangeTriggerNode::range_xz() const
	{
		return range_xz_;
	}

	//-------------------------------------------------------------------------------------
	float RangeTriggerNode::range_y() const
	{
		return range_y_;
	}

	//-------------------------------------------------------------------------------------
	RangeTrigger* RangeTriggerNode::pRangeTrigger() const
	{
		return pRangeTrigger_;
	}

	//-------------------------------------------------------------------------------------
	void RangeTriggerNode::pRangeTrigger(RangeTrigger* pRangeTrigger)
	{
		pRangeTrigger_ = pRangeTrigger;
	}
}