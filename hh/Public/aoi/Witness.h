#pragma once
#include <vector>
#include "Vector3.h"

namespace AOI
{
	class Space;
	class Entity;
	class AOITrigger;
	class EntityRef;

	class Witness
	{
	public:
		Witness();
		~Witness();

		 void pEntity(Entity* pEntity);
		 Entity* pEntity();

		void attach(Entity* pEntity, float radius, float hyst = 5.0f);
		void detach(Entity* pEntity);
		void clear(Entity* pEntity);

		void setAoiRadius(float radius, float hyst = 5.0f);

		 float aoiRadius() const;
		 float aoiHysteresisArea() const;

		 /**
		 基础位置， 如果有坐骑基础位置可能是坐骑等
		 */
		  const Math::Vector3& basePos();

		 bool update();

		 void onEnterSpace(Space* pSpace);
		 void onLeaveSpace(Space* pSpace);

		 void onEnterAOI(AOITrigger* pAOITrigger, Entity* pEntity);
		 void onLeaveAOI(AOITrigger* pAOITrigger, Entity* pEntity);
		 void _onLeaveAOI(EntityRef* pEntityRef);

		 std::vector<EntityRef*>& aoiEntities();

		 /** 获得aoientity的引用 */
		 EntityRef* getAOIEntityRef(int entityID);

		 /** entityID是否在aoi内 */
		  bool entityInAOI(int entityID);

		  AOITrigger* pAOITrigger();
		  AOITrigger* pAOIHysteresisAreaTrigger();

		 void installAOITrigger();
		 void uninstallAOITrigger();

		 /**
		 重置AOI范围内的entities， 使其同步状态恢复到最初未同步的状态
		 */
		 void resetAOIEntities();

	private:
		Entity*									pEntity_;

		// 当前entity的aoi半径
		float									aoiRadius_;
		// 当前entityAoi的一个滞后范围
		float									aoiHysteresisArea_;

		AOITrigger*								pAOITrigger_;
		AOITrigger*								pAOIHysteresisAreaTrigger_;

		std::vector<EntityRef*>					aoiEntities_;

		Math::Vector3							lastBasePos;

		unsigned short							clientAOISize_;
	};
}