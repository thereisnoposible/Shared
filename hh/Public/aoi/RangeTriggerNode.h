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
		(��չ����)
		x && z�ɲ�ͬ��Ӧ��ʵ��(�Ӳ�ͬ����ȡ)
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
		���ڵ�ɾ��
		*/
        virtual void onRemove();
		virtual void onParentRemove(CoordinateNode* pParentNode);

		/**
		ĳ���ڵ�䶯�����˱��ڵ�
		@isfront: ��ǰ�ƶ���������ƶ�
		*/
		virtual void onNodePassX(CoordinateNode* pNode, bool isfront);
		virtual void onNodePassY(CoordinateNode* pNode, bool isfront);
		virtual void onNodePassZ(CoordinateNode* pNode, bool isfront);

	protected:
		float range_xz_, range_y_, old_range_xz_, old_range_y_;
		RangeTrigger* pRangeTrigger_;
	};
}	