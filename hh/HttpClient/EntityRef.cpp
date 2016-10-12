#include "EntityRef.h"
#include "Entity.h"

namespace AOI
{
	EntityRef::EntityRef(Entity* pEntity) :
		id_(0),
		pEntity_(pEntity),
		flags_(ENTITYREF_FLAG_UNKONWN)
	{
		id_ = pEntity->id();
	}

	//-------------------------------------------------------------------------------------
	EntityRef::EntityRef() :
		id_(0),
		pEntity_(NULL),
		flags_(ENTITYREF_FLAG_UNKONWN)
	{
	}

	//-------------------------------------------------------------------------------------
	EntityRef::~EntityRef()
	{
	}

	//-------------------------------------------------------------------------------------
	void EntityRef::pEntity(Entity* e)
	{
		pEntity_ = e;

		if (e)
			id_ = e->id();
	}
}