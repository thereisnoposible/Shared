#pragma once
#include "CoordinateNode.h"

namespace AOI
{
	class RangeTrigger;
	class RangeTriggerNode : public CoordinateNode
	{
	public:
		RangeTriggerNode::RangeTriggerNode(RangeTrigger* pRangeTrigger, float xz, float y);
		~RangeTriggerNode();

        void onTriggerUninstall();


		void range(float xz, float y);
		void old_range(float xz, float y);
		float range_xz() const;
		float range_y() const;

		RangeTrigger* pRangeTrigger() const;
		void pRangeTrigger(RangeTrigger* pRangeTrigger);

		/**
		(扩展坐标)
		x && z由不同的应用实现(从不同处获取)
		*/
		virtual float xx() const;
		virtual float yy() const;
		virtual float zz() const;

		bool isInXRange(CoordinateNode * pNode);
		bool isInYRange(CoordinateNode * pNode);
		bool isInZRange(CoordinateNode * pNode);

        bool wasInXRange(CoordinateNode * pNode, bool isfront);
        bool wasInYRange(CoordinateNode * pNode, bool isfront);
        bool wasInZRange(CoordinateNode * pNode, bool isfront);

		virtual void resetOld(){
			CoordinateNode::resetOld();
			old_range_xz_ = range_xz_;
			old_range_y_ = range_y_;
		}

		/**
		父节点删除
		*/
        virtual void onRemove();
		virtual void onParentRemove(CoordinateNode* pParentNode);

		/**
		某个节点变动经过了本节点
		@isfront: 向前移动还是向后移动
		*/
		virtual void onNodePassX(CoordinateNode* pNode, bool isfront);
		virtual void onNodePassY(CoordinateNode* pNode, bool isfront);
		virtual void onNodePassZ(CoordinateNode* pNode, bool isfront);

	protected:
		float range_xz_, range_y_, old_range_xz_, old_range_y_;
		RangeTrigger* pRangeTrigger_;
	};
}	