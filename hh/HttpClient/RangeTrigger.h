#pragma once

namespace AOI
{
	class CoordinateNode;
	class RangeTriggerNode;
	class RangeTrigger
	{
	public:
		RangeTrigger(CoordinateNode* origin, float xz, float y);
		virtual ~RangeTrigger();

		bool install();
		bool uninstall();
		bool reinstall(CoordinateNode* pCoordinateNode);

		void range(float xz, float y);
		float range_xz() const;
		float range_y() const;

		CoordinateNode* origin() const;
		void origin(CoordinateNode* pCoordinateNode);

		/**
		���·�Χ����
		*/
		virtual void update(float xz, float y);

		/**
		ĳ���ڵ��������뿪��rangeTrigger
		*/
		virtual void onEnter(CoordinateNode * pNode) = 0;
		virtual void onLeave(CoordinateNode * pNode) = 0;

		/**
		ĳ���ڵ�䶯�����˱��ڵ�
		@isfront: ��ǰ�ƶ���������ƶ�
		*/
		virtual void onNodePassX(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront);
		virtual void onNodePassY(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront);
		virtual void onNodePassZ(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode, bool isfront);

	private:
		float range_xz_, range_y_;

		CoordinateNode* origin_;

		RangeTriggerNode* positiveBoundary_;
		RangeTriggerNode* negativeBoundary_;
	};
}