#include "Entity.h"
#include "EntityCoordinateNode.h"
#include "CoordinateSystem.h"
#include "Witness.h"

#include <iostream>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if(p!=NULL){delete p;p=NULL;}
#endif // !SAFE_DELETE

namespace AOI
{
	Entity::Entity(int id, EntityType type) :
		id_(id)
	{
		pEntityCoordinateNode_ = new EntityCoordinateNode(this);
		if (type != EntityTypeProps)
			setWitness(new Witness);
	}
	Entity::~Entity()
	{      
        SAFE_DELETE(pEntityCoordinateNode_);
        SAFE_DELETE(pWitness_);
	}

	//-------------------------------------------------------------------------------------
	void Entity::setWitness(Witness* pWitness)
	{
		pWitness_ = pWitness;
		pWitness_->attach(this, 5, 1);
	}


	int Entity::id()
	{
		return id_;
	}
	void Entity::id(int v)
	{
		id_ = v;
	}

	unsigned int Entity::flags()
	{
		return flags_;
	}
	void Entity::flags(unsigned int v)
	{
		flags_ = v;
	}

	EntityType& Entity::entityType()
	{
		return type_;
	}

	//-------------------------------------------------------------------------------------
	 Witness* Entity::pWitness() const
	{
		return pWitness_;
	}

	//-------------------------------------------------------------------------------------
	 void Entity::pWitness(Witness* w)
	{
		pWitness_ = w;
	}

	Math::Vector3& Entity::position()
	{ 
		return position_; 
	}

	void Entity::position(Math::Vector3& pos)
	{
		Math::Vector3 movement = pos - position_;
		if (movement.length() < 0.0004f)
			return;
		last_position_ = position_;
		position_ = pos;
	}

	void Entity::installCoordinateNodes(CoordinateSystem* pCoordinateSystem)
	{
		pCoordinateSystem->insert(pEntityCoordinateNode_);
	}

	void Entity::uninstallCoordinateNodes(CoordinateSystem* pCoordinateSystem)
	{	
		pCoordinateSystem->remove((AOI::CoordinateNode*)pEntityCoordinateNode());
		pEntityCoordinateNode_ = new EntityCoordinateNode(this);	
		//pWitness_->installAOITrigger();
		//delete pEntityCoordinateNode_;
			
	}

	EntityCoordinateNode* Entity::pEntityCoordinateNode() const
	{
		return pEntityCoordinateNode_;
	}

	void Entity::pEntityCoordinateNode(EntityCoordinateNode* pNode)
	{
		pEntityCoordinateNode_ = pNode;
	}

	void Entity::addWitnessed(Entity* entity)
	{
		witnesses_.push_back(entity->id());
		++witnesses_count_;
	}

	void Entity::delWitnessed(Entity* entity)
	{
		witnesses_.remove(entity->id());
		--witnesses_count_;
	}

	void Entity::onEnteredAoI(Entity* entity)
	{
		std::cout << "id: " << id_ << " onEnteredAoI :" << entity->id() << "\n";
	}

    void Entity::onLeaveAoI(Entity* entity)
    {
        std::cout << "id: " << id_ << " onLeaveAoI :" << entity->id() << "\n";
    }

	std::list<int> Entity::GetWitnesses()
	{
		return witnesses_;
	}

	void Entity::onEnterSpace(Space* pSpace)
	{

	}
	void Entity::onLeaveSpace(Space* pSpace)
	{

	}
	void Entity::onOtherEntityMove(Entity* entity)
	{

	}
}