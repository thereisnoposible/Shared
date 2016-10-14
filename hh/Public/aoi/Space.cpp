#include "Space.h"
#include "Entity.h"
#include "Witness.h"

namespace AOI
{
	void Space::addEntity(Entity* pEntity)
	{
		//entities_.push_back(pEntity);
		pEntity->onEnterSpace(this);
	}

	void Space::addEntityToNode(Entity* pEntity)
	{
		pEntity->installCoordinateNodes(&CoordinateSystem_);
	}

	void Space::onEnterWorld(Entity* pEntity)
	{
		//pEntity->installCoordinateNodes(&CoordinateSystem_);
		_onEnterWorld(pEntity);
	}

	void Space::_onEnterWorld(Entity* pEntity)
	{
		pEntity->pWitness()->onEnterSpace(this);
	}

	void Space::onLeaveWorld(Entity* pEntity)
	{
		pEntity->pWitness()->onLeaveSpace(this);
	}

	void Space::addEntityAndEnterWorld(Entity* pEntity, bool isRestore)
	{
		addEntity(pEntity);
		addEntityToNode(pEntity);
		onEnterWorld(pEntity);
	}
	void Space::removeEntity(Entity* pEntity)
	{
		onLeaveWorld(pEntity);

		// 这句必须在onLeaveWorld之后， 因为可能rangeTrigger需要参考pEntityCoordinateNode
		pEntity->uninstallCoordinateNodes(&CoordinateSystem_);
		pEntity->onLeaveSpace(this);
	}
}