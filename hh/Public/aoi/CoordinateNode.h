#pragma once

#include "CoordinateSystem.h"

namespace AOI
{
#define COORDINATE_NODE_FLAG_UNKNOWN				0x00000000
#define COORDINATE_NODE_FLAG_ENTITY					0x00000001		// 一个Entity节点
#define COORDINATE_NODE_FLAG_TRIGGER				0x00000002		// 一个触发器节点
#define COORDINATE_NODE_FLAG_HIDE					0x00000004		// 隐藏节点(其他节点不可见)
#define COORDINATE_NODE_FLAG_REMOVEING				0x00000008		// 删除中的节点
#define COORDINATE_NODE_FLAG_REMOVED				0x00000010		// 删除节点
#define COORDINATE_NODE_FLAG_PENDING				0x00000020		// 这类节点处于update操作中。
#define COORDINATE_NODE_FLAG_ENTITY_NODE_UPDATING	0x00000040		// entity节点正在执行update操作
#define COORDINATE_NODE_FLAG_INSTALLING				0x00000080		// 节点正在安装操作

#define COORDINATE_NODE_FLAG_HIDE_OR_REMOVED		(COORDINATE_NODE_FLAG_REMOVED | COORDINATE_NODE_FLAG_HIDE)

	class CoordinateNode
	{
	public:
		CoordinateNode::CoordinateNode(CoordinateSystem* pCoordinateSystem) :
			pPrevX_(NULL),
			pNextX_(NULL),
			pPrevY_(NULL),
			pNextY_(NULL),
			pPrevZ_(NULL),
			pNextZ_(NULL),
			pCoordinateSystem_(pCoordinateSystem),
			x_(-FLT_MAX),
			y_(-FLT_MAX),
			z_(-FLT_MAX),
			old_xx_(-FLT_MAX),
			old_yy_(-FLT_MAX),
			old_zz_(-FLT_MAX),
			flags_(COORDINATE_NODE_FLAG_UNKNOWN)
		{
		}
		virtual ~CoordinateNode(){};

		virtual void update(){ if (pCoordinateSystem_){ pCoordinateSystem_->update(this); } }

		void flags(unsigned int v){ flags_ = v; }
		unsigned int flags() const{ return flags_; }

		/**
		(节点本身的坐标)
		x && z由不同的应用实现(从不同处获取)
		*/
		virtual float x() const { return x_; }
		virtual float y() const { return y_; }
		virtual float z() const { return z_; }

		virtual void x(float v){ x_ = v; }
		virtual void y(float v){ y_ = v; }
		virtual void z(float v){ z_ = v; }

		/**
		(扩展坐标)
		x && z由不同的应用实现(从不同处获取)
		*/
		virtual float xx() const { return 0.f; }
		virtual float yy() const { return 0.f; }
		virtual float zz() const { return 0.f; }

		void old_xx(float v) { old_xx_ = v; }
		void old_yy(float v) { old_yy_ = v; }
		void old_zz(float v) { old_zz_ = v; }

		float old_xx() const { return old_xx_; }
		float old_yy() const { return old_yy_; }
		float old_zz() const { return old_zz_; }

		virtual void resetOld(){
			old_xx_ = xx();
			old_yy_ = yy();
			old_zz_ = zz();
		}

		inline void pCoordinateSystem(CoordinateSystem* p){ pCoordinateSystem_ = p; }
		inline CoordinateSystem* pCoordinateSystem() const{ return pCoordinateSystem_; }

		/**
		获取链表的前后端指针
		*/
		inline CoordinateNode* pPrevX() const{ return pPrevX_; }
		inline CoordinateNode* pNextX() const{ return pNextX_; }
		inline CoordinateNode* pPrevY() const{ return pPrevY_; }
		inline CoordinateNode* pNextY() const{ return pNextY_; }
		inline CoordinateNode* pPrevZ() const{ return pPrevZ_; }
		inline CoordinateNode* pNextZ() const{ return pNextZ_; }

		/**
		设置链表的前后端指针
		*/
		inline void pPrevX(CoordinateNode* pNode){ pPrevX_ = pNode; }
		inline void pNextX(CoordinateNode* pNode){ pNextX_ = pNode; }
		inline void pPrevY(CoordinateNode* pNode){ pPrevY_ = pNode; }
		inline void pNextY(CoordinateNode* pNode){ pNextY_ = pNode; }
		inline void pPrevZ(CoordinateNode* pNode){ pPrevZ_ = pNode; }
		inline void pNextZ(CoordinateNode* pNode){ pNextZ_ = pNode; }

		/**
		某个节点变动经过了本节点
		@isfront: 向前移动还是向后移动
		*/
		virtual void onNodePassX(CoordinateNode* pNode, bool isfront){}
		virtual void onNodePassY(CoordinateNode* pNode, bool isfront){}
		virtual void onNodePassZ(CoordinateNode* pNode, bool isfront){}

		virtual void onRemove()
		{
			old_xx(x_);
			old_yy(y_);
			old_zz(z_);

			x_ = -FLT_MAX;
		}

		/**
		父节点删除
		*/
		virtual void onParentRemove(CoordinateNode* pParentNode){}

		inline bool hasFlags(unsigned int v) const { return (flags_ & v) > 0; }
		void addFlags(unsigned int v) { flags_ |= v; }
		void removeFlags(unsigned int v) { flags_ &= ~v; }

	protected:
		// 链表的前端和后端
		CoordinateNode* pPrevX_;
		CoordinateNode* pNextX_;
		CoordinateNode* pPrevY_;
		CoordinateNode* pNextY_;
		CoordinateNode* pPrevZ_;
		CoordinateNode* pNextZ_;

		CoordinateSystem* pCoordinateSystem_;

		float x_, y_, z_;
		float old_xx_, old_yy_, old_zz_;

		unsigned int flags_;
	};
}