#pragma once
#include "CoordinateSystem.h"

namespace AOI
{
	class Entity;

	class Space
	{
	public:
		void OnEnterSpace(Entity*);

	private:
		CoordinateSystem CoordinateSystem_;
	};
}