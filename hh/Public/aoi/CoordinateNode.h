#pragma once

#include "CoordinateSystem.h"

namespace AOI
{
#define COORDINATE_NODE_FLAG_UNKNOWN				0x00000000
#define COORDINATE_NODE_FLAG_ENTITY					0x00000001		// һ��Entity�ڵ�
#define COORDINATE_NODE_FLAG_TRIGGER				0x00000002		// һ���������ڵ�
#define COORDINATE_NODE_FLAG_HIDE					0x00000004		// ���ؽڵ�(�����ڵ㲻�ɼ�)
#define COORDINATE_NODE_FLAG_REMOVEING				0x00000008		// ɾ���еĽڵ�
#define COORDINATE_NODE_FLAG_REMOVED				0x00000010		// ɾ���ڵ�
#define COORDINATE_NODE_FLAG_PENDING				0x00000020		// ����ڵ㴦��update�����С�
#define COORDINATE_NODE_FLAG_ENTITY_NODE_UPDATING	0x00000040		// entity�ڵ�����ִ��update����
#define COORDINATE_NODE_FLAG_INSTALLING				0x00000080		// �ڵ����ڰ�װ����

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
		(�ڵ㱾�������)
		x && z�ɲ�ͬ��Ӧ��ʵ��(�Ӳ�ͬ����ȡ)
		*/
		virtual float x() const { return x_; }
		virtual float y() const { return y_; }
		virtual float z() const { return z_; }

		virtual void x(float v){ x_ = v; }
		virtual void y(float v){ y_ = v; }
		virtual void z(float v){ z_ = v; }

		/**
		(��չ����)
		x && z�ɲ�ͬ��Ӧ��ʵ��(�Ӳ�ͬ����ȡ)
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
		��ȡ�����ǰ���ָ��
		*/
		inline CoordinateNode* pPrevX() const{ return pPrevX_; }
		inline CoordinateNode* pNextX() const{ return pNextX_; }
		inline CoordinateNode* pPrevY() const{ return pPrevY_; }
		inline CoordinateNode* pNextY() const{ return pNextY_; }
		inline CoordinateNode* pPrevZ() const{ return pPrevZ_; }
		inline CoordinateNode* pNextZ() const{ return pNextZ_; }

		/**
		���������ǰ���ָ��
		*/
		inline void pPrevX(CoordinateNode* pNode){ pPrevX_ = pNode; }
		inline void pNextX(CoordinateNode* pNode){ pNextX_ = pNode; }
		inline void pPrevY(CoordinateNode* pNode){ pPrevY_ = pNode; }
		inline void pNextY(CoordinateNode* pNode){ pNextY_ = pNode; }
		inline void pPrevZ(CoordinateNode* pNode){ pPrevZ_ = pNode; }
		inline void pNextZ(CoordinateNode* pNode){ pNextZ_ = pNode; }

		/**
		ĳ���ڵ�䶯�����˱��ڵ�
		@isfront: ��ǰ�ƶ���������ƶ�
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
		���ڵ�ɾ��
		*/
		virtual void onParentRemove(CoordinateNode* pParentNode){}

		inline bool hasFlags(unsigned int v) const { return (flags_ & v) > 0; }
		void addFlags(unsigned int v) { flags_ |= v; }
		void removeFlags(unsigned int v) { flags_ &= ~v; }

	protected:
		// �����ǰ�˺ͺ��
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