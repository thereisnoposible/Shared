#pragma once
#include "CoordinateNode.h"
#include <vector>
#include "Vector3.h"

namespace AOI
{
	class Entity;
	class EntityCoordinateNode : public CoordinateNode
	{
	public:
		EntityCoordinateNode(Entity* pEntity);
		virtual ~EntityCoordinateNode();
		float xx() const;
		float yy() const;
		float zz() const;

		void update();

		Entity* pEntity() const { return pEntity_; }
		void pEntity(Entity* pEntity) { pEntity_ = pEntity; }

		bool addWatcherNode(CoordinateNode* pNode);
		bool delWatcherNode(CoordinateNode* pNode);

		static void entitiesInRange(std::vector<Entity*>& foundEntities, CoordinateNode* rootNode,
			const Math::Vector3& orginPos, float radius, int entityUType = -1);

		virtual void onRemove();
	protected:
		Entity* pEntity_;
		std::vector<CoordinateNode*> watcherNodes_;
	};
}