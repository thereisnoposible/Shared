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

		positiveBoundary_->addFlags(COORDINATE_NODE_FLAG_INSTALLING);
		negativeBoundary_->addFlags(COORDINATE_NODE_FLAG_INSTALLING);

		origin_->pCoordinateSystem()->insert(positiveBoundary_);
		origin_->pCoordinateSystem()->insert(negativeBoundary_);

		/*
		ע�⣺�˴��������Ȱ�װnegativeBoundary_�ٰ�װpositiveBoundary_���������˳����ᵼ��AOI��BUG�����磺��һ��ʵ��enterAoi����ʱ�����˽���AOI��ʵ��
		��ʱʵ������ʱ��δ�����뿪AOI�¼�����δ����AOI�¼���������ʵ���AOI�б������õĸ����ٵ�ʵ����һ����Чָ�롣

		ԭ�����£�
		�����������Ȱ�װ��positiveBoundary_�����߽��ڰ�װ�����е�����һ��ʵ�����AOI�ˣ� Ȼ��������������п��������ˣ� ����һ���߽�negativeBoundary_��û�а�װ��
		���ڵ�ɾ��ʱ�����ýڵ��xxΪ-FLT_MAX��������negativeBoundary_�����뿪������positiveBoundary_���ܼ�鵽����߽�Ҳ�Ͳ��ᴥ��AOI�뿪�¼���
		*/
		negativeBoundary_->old_xx(-FLT_MAX);
		negativeBoundary_->old_yy(-FLT_MAX);
		negativeBoundary_->old_zz(-FLT_MAX);
		negativeBoundary_->range(-range_xz_, -range_y_);
		negativeBoundary_->old_range(-range_xz_, -range_y_);
		negativeBoundary_->update();
		negativeBoundary_->removeFlags(COORDINATE_NODE_FLAG_INSTALLING);

		// update���ܵ���ʵ�����ټ�ӵ����Լ������ã���ʱӦ�÷��ذ�װʧ��
		if (!negativeBoundary_)
			return false;

		positiveBoundary_->old_xx(FLT_MAX);
		positiveBoundary_->old_yy(FLT_MAX);
		positiveBoundary_->old_zz(FLT_MAX);

		positiveBoundary_->range(range_xz_, range_y_);
		positiveBoundary_->old_range(range_xz_, range_y_);
		positiveBoundary_->update();
		positiveBoundary_->removeFlags(COORDINATE_NODE_FLAG_INSTALLING);

		return positiveBoundary_ != NULL;
	}
	bool RangeTrigger::uninstall()
	{
        if (positiveBoundary_ && positiveBoundary_->pCoordinateSystem())
        {
            positiveBoundary_->onTriggerUninstall();
            positiveBoundary_->pCoordinateSystem()->remove(positiveBoundary_);
        }

        if (negativeBoundary_ && negativeBoundary_->pCoordinateSystem())
        {
            negativeBoundary_->onTriggerUninstall();
            negativeBoundary_->pCoordinateSystem()->remove(negativeBoundary_);
        }

        // �˴�����release node�� �ڵ���ͷ�ͳһ����CoordinateSystem
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

	bool RangeTrigger::isInstalled() const
	{
		return positiveBoundary_ && negativeBoundary_;
	}

    //-------------------------------------------------------------------------------------
    void RangeTrigger::onNodePassX(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront)
    {
        if (pNode == origin())
            return;

        bool wasInZ = pRangeTriggerNode->wasInZRange(pNode, isfront);
        bool isInZ = pRangeTriggerNode->isInZRange(pNode);

        // ���Z������б仯����Z�����жϣ����ȼ�Ϊzyx�������ſ��Ա�ֻ֤��һ��enter����leave
        if (wasInZ != isInZ)
            return;

        bool wasIn = false;
        bool isIn = false;

        // ����ͬʱ��������ᣬ ����ڵ�x���ڷ�Χ�ڣ�������������Ҳ�ڷ�Χ��
        {
            bool wasInY = pRangeTriggerNode->wasInYRange(pNode, isfront);
            bool isInY = pRangeTriggerNode->isInYRange(pNode);

            if (wasInY != isInY)
                return;

            wasIn = pRangeTriggerNode->wasInXRange(pNode, isfront) && wasInY && wasInZ;
            isIn = pRangeTriggerNode->isInXRange(pNode) && isInY && isInZ;
        }

        // ������û�з����仯�����
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
    void RangeTrigger::onNodePassY(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront)
    {
        if (pNode == origin())
            return;

        bool wasInZ = pRangeTriggerNode->wasInZRange(pNode, isfront);
        bool isInZ = pRangeTriggerNode->isInZRange(pNode);

        // ���Z������б仯����Z�����жϣ����ȼ�Ϊzyx�������ſ��Ա�ֻ֤��һ��enter����leave
        if (wasInZ != isInZ)
            return;

        bool wasInY = pRangeTriggerNode->wasInYRange(pNode, isfront);
        bool isInY = pRangeTriggerNode->isInYRange(pNode);

        if (wasInY == isInY)
            return;

        // ����ͬʱ��������ᣬ ����ڵ�x���ڷ�Χ�ڣ�������������Ҳ�ڷ�Χ��
        bool wasIn = pRangeTriggerNode->wasInXRange(pNode, isfront) && wasInY && wasInZ;
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

        {
            bool wasInZ = pRangeTriggerNode->wasInZRange(pNode, isfront);
            bool isInZ = pRangeTriggerNode->isInZRange(pNode);

            if (wasInZ == isInZ)
                return;

            bool wasIn = pRangeTriggerNode->wasInXRange(pNode, isfront) &&
                pRangeTriggerNode->wasInYRange(pNode, isfront) &&
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