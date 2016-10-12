#pragma once 
#include "RangeTrigger.h"

namespace AOI
{
	class Witness;

	class AOITrigger :public RangeTrigger
	{
	public:
		AOITrigger(CoordinateNode* origin, float xz = 0.0f, float y = 0.0f);
		virtual ~AOITrigger();

		/**
		某个节点进入或者离开了rangeTrigger
		*/
		virtual void onEnter(CoordinateNode * pNode);
		virtual void onLeave(CoordinateNode * pNode);

		Witness* pWitness() const;

	protected:
		Witness* pWitness_;
	};
}