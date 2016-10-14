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
		 ����λ�ã� ������������λ�ÿ����������
		 */
		  const Math::Vector3& basePos();

		 bool update();

		 void onEnterSpace(Space* pSpace);
		 void onLeaveSpace(Space* pSpace);

		 void onEnterAOI(AOITrigger* pAOITrigger, Entity* pEntity);
		 void onLeaveAOI(AOITrigger* pAOITrigger, Entity* pEntity);
		 void _onLeaveAOI(EntityRef* pEntityRef);

		 std::vector<EntityRef*>& aoiEntities();

		 /** ���aoientity������ */
		 EntityRef* getAOIEntityRef(int entityID);

		 /** entityID�Ƿ���aoi�� */
		  bool entityInAOI(int entityID);

		  AOITrigger* pAOITrigger();
		  AOITrigger* pAOIHysteresisAreaTrigger();

		 void installAOITrigger();
		 void uninstallAOITrigger();

		 /**
		 ����AOI��Χ�ڵ�entities�� ʹ��ͬ��״̬�ָ������δͬ����״̬
		 */
		 void resetAOIEntities();

	private:
		Entity*									pEntity_;

		// ��ǰentity��aoi�뾶
		float									aoiRadius_;
		// ��ǰentityAoi��һ���ͺ�Χ
		float									aoiHysteresisArea_;

		AOITrigger*								pAOITrigger_;
		AOITrigger*								pAOIHysteresisAreaTrigger_;

		std::vector<EntityRef*>					aoiEntities_;

		Math::Vector3							lastBasePos;

		unsigned short							clientAOISize_;
	};
}