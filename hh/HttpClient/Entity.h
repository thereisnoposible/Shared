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

	class Entity
	{
	public:
		Entity(int id);
		~Entity();
		Math::Vector3& position();
		void position(Math::Vector3& pos);

		EntityCoordinateNode* Entity::pEntityCoordinateNode() const;
		void Entity::pEntityCoordinateNode(EntityCoordinateNode* pNode);

		void installCoordinateNodes(CoordinateSystem* pCoordinateSystem);

		/**
		�۲���
		*/
		 Witness* pWitness() const;
		 void pWitness(Witness* w);

		/**
		����һ���۲��߹۲쵽��
		*/
		void addWitnessed(Entity* entity);

		/**
		�Ƴ�һ���۲�����Ĺ۲���
		*/
		void delWitnessed(Entity* entity);

		int id();
		void id(int v);

		unsigned int flags();
		void flags(unsigned int v);

		/**
		һ��entity������AOI����
		*/
		void onEnteredAoI(Entity* entity);

	private:
		int id_;
		unsigned int											flags_;

		EntityCoordinateNode* pEntityCoordinateNode_;
		CoordinateSystem* pCoordinateSystem_;
		Math::Vector3 position_;

		// �Ƿ��κι۲��߼��ӵ�
		std::list<int>											witnesses_;
		size_t													witnesses_count_;

		// �۲��߶���
		Witness*												pWitness_;
	};
}