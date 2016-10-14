#pragma once
#include "CoordinateSystem.h"
#include <vector>

namespace AOI
{
	class Entity;

	class Space
	{
	public:
		void addEntity(Entity* pEntity);
		void addEntityToNode(Entity* pEntity);

		void onEnterWorld(Entity*);
		void _onEnterWorld(Entity* pEntity);
		void onLeaveWorld(Entity* pEntity);

		void addEntityAndEnterWorld(Entity* pEntity, bool isRestore = false);
		void removeEntity(Entity* pEntity);

	private:
		CoordinateSystem CoordinateSystem_;

		std::vector<Entity*> entities_;
	};
}