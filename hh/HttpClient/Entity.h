#pragma once
#include "Vector3.h"
#include <list>

namespace AOI
{
#define ENTITYREF_FLAG_UNKONWN							0x00000000
#define ENTITYREF_FLAG_ENTER_CLIENT_PENDING				0x00000001	// 进入客户端中标志
#define ENTITYREF_FLAG_LEAVE_CLIENT_PENDING				0x00000002	// 离开客户端中标志
#define ENTITYREF_FLAG_NORMAL							0x00000004	// 常规状态

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
		观察者
		*/
		 Witness* pWitness() const;
		 void pWitness(Witness* w);

		/**
		自身被一个观察者观察到了
		*/
		void addWitnessed(Entity* entity);

		/**
		移除一个观察自身的观察者
		*/
		void delWitnessed(Entity* entity);

		int id();
		void id(int v);

		unsigned int flags();
		void flags(unsigned int v);

		/**
		一个entity进入了AOI区域
		*/
		void onEnteredAoI(Entity* entity);

	private:
		int id_;
		unsigned int											flags_;

		EntityCoordinateNode* pEntityCoordinateNode_;
		CoordinateSystem* pCoordinateSystem_;
		Math::Vector3 position_;

		// 是否被任何观察者监视到
		std::list<int>											witnesses_;
		size_t													witnesses_count_;

		// 观察者对象
		Witness*												pWitness_;
	};
}