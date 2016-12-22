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

	}

    //-------------------------------------------------------------------------------------
    void RangeTriggerNode::onTriggerUninstall()
    {
        if (pRangeTrigger_->origin())
            static_cast<EntityCoordinateNode*>(pRangeTrigger_->origin())->delWatcherNode(this);

        pRangeTrigger(NULL);
    }

    //-------------------------------------------------------------------------------------
    void RangeTriggerNode::onRemove()
    {
        CoordinateNode::onRemove();

        // 既然自己都要删除了，通知pRangeTrigger_卸载
        if (pRangeTrigger_)
            pRangeTrigger_->uninstall();
    }

    //-------------------------------------------------------------------------------------
    void RangeTriggerNode::onParentRemove(CoordinateNode* pParentNode)
    {
        // 既然自己都要删除了，通知pRangeTrigger_卸载
        if (pRangeTrigger_)
            pRangeTrigger_->uninstall();
    }

    //-------------------------------------------------------------------------------------
    float RangeTriggerNode::xx() const
    {
        if (hasFlags(COORDINATE_NODE_FLAG_REMOVED) || pRangeTrigger_ == NULL)
            return -FLT_MAX;

        return pRangeTrigger_->origin()->xx() + range_xz_;
    }

    //-------------------------------------------------------------------------------------
    float RangeTriggerNode::yy() const
    {
        if (hasFlags(COORDINATE_NODE_FLAG_REMOVED) || pRangeTrigger_ == NULL)
            return -FLT_MAX;

        return pRangeTrigger_->origin()->yy() + range_y_;
    }

    //-------------------------------------------------------------------------------------
    float RangeTriggerNode::zz() const
    {
        if (hasFlags(COORDINATE_NODE_FLAG_REMOVED) || pRangeTrigger_ == NULL)
            return -FLT_MAX;

        return pRangeTrigger_->origin()->zz() + range_xz_;
    }

    //-------------------------------------------------------------------------------------
    void RangeTriggerNode::onNodePassX(CoordinateNode* pNode, bool isfront)
    {
        if (!hasFlags(COORDINATE_NODE_FLAG_REMOVED) && pRangeTrigger_)
            pRangeTrigger_->onNodePassX(this, pNode, isfront);
    }

    //-------------------------------------------------------------------------------------
    void RangeTriggerNode::onNodePassY(CoordinateNode* pNode, bool isfront)
    {
        if (!hasFlags(COORDINATE_NODE_FLAG_REMOVED) && pRangeTrigger_)
            pRangeTrigger_->onNodePassY(this, pNode, isfront);
    }

    //-------------------------------------------------------------------------------------
    void RangeTriggerNode::onNodePassZ(CoordinateNode* pNode, bool isfront)
    {
        if (!hasFlags(COORDINATE_NODE_FLAG_REMOVED) && pRangeTrigger_)
            pRangeTrigger_->onNodePassZ(this, pNode, isfront);
    }

    //-------------------------------------------------------------------------------------
    bool RangeTriggerNode::isInXRange(CoordinateNode * pNode)
    {
        float originX = pRangeTrigger_->origin()->xx();

        volatile float lowerBound = originX - fabs(range_xz_);
        volatile float upperBound = originX + fabs(range_xz_);
        return (pNode->xx() >= lowerBound) && (pNode->xx() <= upperBound);
    }

    //-------------------------------------------------------------------------------------
    bool RangeTriggerNode::isInYRange(CoordinateNode * pNode)
    {
        float originY = pRangeTrigger_->origin()->yy();

        volatile float lowerBound = originY - fabs(range_y_);
        volatile float upperBound = originY + fabs(range_y_);
        return (pNode->yy() >= lowerBound) && (pNode->yy() <= upperBound);
    }

    //-------------------------------------------------------------------------------------
    bool RangeTriggerNode::isInZRange(CoordinateNode * pNode)
    {
        float originZ = pRangeTrigger_->origin()->zz();

        volatile float lowerBound = originZ - fabs(range_xz_);
        volatile float upperBound = originZ + fabs(range_xz_);
        return (pNode->zz() >= lowerBound) && (pNode->zz() <= upperBound);
    }

    //-------------------------------------------------------------------------------------
    bool RangeTriggerNode::wasInXRange(CoordinateNode * pNode, bool isfront)
    {
        float originX = old_xx() - old_range_xz_;
        //float originX = pRangeTrigger_->origin()->old_xx();

        volatile float lowerBound = originX - fabs(old_range_xz_);
        volatile float upperBound = originX + fabs(old_range_xz_);

        //只判定 进入range圈
        if (pNode->old_xx() == lowerBound)
        {
            if (isfront)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        if (pNode->old_xx() == upperBound)
        {
            if (isfront)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        return (pNode->old_xx() >= lowerBound) && (pNode->old_xx() <= upperBound);
    }

    //-------------------------------------------------------------------------------------
    bool RangeTriggerNode::wasInYRange(CoordinateNode * pNode, bool isfront)
    {
        float originY = old_yy() - old_range_y_;
        //float originY = pRangeTrigger_->origin()->old_yy();

        //如果是出range圈 则要判断=  如果是进range圈 则不需要判断=

        volatile float lowerBound = originY - fabs(old_range_y_); 
        volatile float upperBound = originY + fabs(old_range_y_);

        if (pNode->old_yy() == lowerBound)
        {
            if (isfront)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        if (pNode->old_yy() == upperBound)
        {
            if (isfront)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        return (pNode->old_yy() >= lowerBound) && (pNode->old_yy() <= upperBound);
    }

    //-------------------------------------------------------------------------------------
    bool RangeTriggerNode::wasInZRange(CoordinateNode * pNode, bool isfront)
    {
        float originZ = old_zz() - old_range_xz_;
        //float originZ = pRangeTrigger_->origin()->old_zz();

        volatile float lowerBound = originZ - fabs(old_range_xz_);
        volatile float upperBound = originZ + fabs(old_range_xz_);

        if (pNode->old_zz() == lowerBound)
        {
            if (isfront)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        if (pNode->old_zz() == upperBound)
        {
            if (isfront)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        return (pNode->old_zz() >= lowerBound) && (pNode->old_zz() <= upperBound);
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