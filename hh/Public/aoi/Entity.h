#pragma once
#include "Vector3.h"
#include <list>

namespace AOI
{
#define ENTITYREF_FLAG_UNKONWN							0x00000000
#define ENTITYREF_FLAG_ENTER_CLIENT_PENDING				0x00000001	// ����ͻ����б�־
#define ENTITYREF_FLAG_LEAVE_CLIENT_PENDING				0x00000002	// �뿪�ͻ����б�־
#define ENTITYREF_FLAG_NORMAL							0x00000004	// ����״̬

	class CoordinateSystem;
	class EntityCoordinateNode;
	class Witness;
	class Space;

	enum EntityType
	{
		EntityTypePlayer,
		EntityTypeProps,
		EntityTypeNpc,
	};

	class Entity
	{
	public:
		Entity(int id, EntityType type);
		~Entity();
		Math::Vector3& position();
		void position(Math::Vector3& pos);

		EntityCoordinateNode* Entity::pEntityCoordinateNode() const;
		void Entity::pEntityCoordinateNode(EntityCoordinateNode* pNode);

		void installCoordinateNodes(CoordinateSystem* pCoordinateSystem);
		void uninstallCoordinateNodes(CoordinateSystem* pCoordinateSystem);

		/**
		�����뿪space�Ȼص�
		*/
		virtual void onEnterSpace(Space* pSpace);
		virtual void onLeaveSpace(Space* pSpace);

		virtual void onOtherEntityMove(Entity* entity);

		/**
		�۲���
		*/
		 Witness* pWitness() const;
		 void pWitness(Witness* w);

		 void setWitness(Witness* pWitness);

		/**
		����һ���۲��߹۲쵽��
		*/
		void addWitnessed(Entity* entity);

		/**
		�Ƴ�һ���۲�����Ĺ۲���
		*/
		void delWitnessed(Entity* entity);

		std::list<int> GetWitnesses();

		int id();
		void id(int v);

		unsigned int flags();
		void flags(unsigned int v);

		EntityType& entityType();

		/**
		һ��entity������AOI����
		*/
		virtual void onEnteredAoI(Entity* entity);

        virtual void onLeaveAoI(Entity* entity);

	private:
		int id_;
		unsigned int											flags_;
		EntityType type_;

		EntityCoordinateNode* pEntityCoordinateNode_;
		CoordinateSystem* pCoordinateSystem_;
		Math::Vector3 position_;
		Math::Vector3 last_position_;

		// �Ƿ��κι۲��߼��ӵ�
		std::list<int>											witnesses_;
		size_t													witnesses_count_;

		// �۲��߶���
		Witness*												pWitness_;
	};
}