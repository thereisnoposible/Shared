#include "AOITrigger.h"
#include "Witness.h"
#include "EntityCoordinateNode.h"
#include "Entity.h"

namespace AOI
{
	//-------------------------------------------------------------------------------------
	AOITrigger::AOITrigger(CoordinateNode* origin, float xz, float y) :
		RangeTrigger(origin, xz, y),
		pWitness_(static_cast<EntityCoordinateNode*>(origin)->pEntity()->pWitness())
	{
	}

	//-------------------------------------------------------------------------------------
	AOITrigger::~AOITrigger()
	{
	}

	//-------------------------------------------------------------------------------------
	void AOITrigger::onEnter(CoordinateNode * pNode)
	{
		if ((pNode->flags() & COORDINATE_NODE_FLAG_ENTITY) <= 0)
			return;

		EntityCoordinateNode* pEntityCoordinateNode = static_cast<EntityCoordinateNode*>(pNode);
		Entity* pEntity = pEntityCoordinateNode->pEntity();

		pWitness_->onEnterAOI(this, pEntity);
	}

	//-------------------------------------------------------------------------------------
	void AOITrigger::onLeave(CoordinateNode * pNode)
	{
		if ((pNode->flags() & COORDINATE_NODE_FLAG_ENTITY) <= 0)
			return;

		EntityCoordinateNode* pEntityCoordinateNode = static_cast<EntityCoordinateNode*>(pNode);
		Entity* pEntity = pEntityCoordinateNode->pEntity();

		pWitness_->onLeaveAOI(this, pEntity);
	}

	Witness* AOITrigger::pWitness() const
	{
		return pWitness_;
	}
}