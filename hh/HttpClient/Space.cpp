#include "Space.h"
#include "Entity.h"

namespace AOI
{
	void Space::OnEnterSpace(Entity* pEntity)
	{
		pEntity->installCoordinateNodes(&CoordinateSystem_);
	}
}