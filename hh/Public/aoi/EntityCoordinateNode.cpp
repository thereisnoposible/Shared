#include "EntityCoordinateNode.h"
#include "Entity.h"

#include <set>
#include <algorithm>
#include <iterator>

namespace AOI
{
	//-------------------------------------------------------------------------------------
	class CoordinateNodeWrapX
	{
	public:
		CoordinateNodeWrapX(CoordinateNode* node, const Math::Vector3& originPos) :
			pNode_(node),
			pCurrentNode_(node),
			originPos_(originPos) {}

		void reset() { pCurrentNode_ = pNode_; }
		bool isEntityNode()const { return (pCurrentNode_->flags() & COORDINATE_NODE_FLAG_ENTITY) > 0; }
		CoordinateNode* currentNode()const { return pCurrentNode_; }
		Entity* currentNodeEntity()const { return static_cast<EntityCoordinateNode*>(pCurrentNode_)->pEntity(); }

		CoordinateNode* prev() {
			pCurrentNode_ = pCurrentNode_->pPrevX();
			return pCurrentNode_;
		}

		CoordinateNode* next() {
			pCurrentNode_ = pCurrentNode_->pNextX();
			return pCurrentNode_;
		}

		int compare() {
			float v = currentNodeEntity()->position().x;
			if (v == originPos_.x)
				return 0;
			else if (v > originPos_.x)
				return 1;
			else
				return -1;
		}

		float length() {
			return fabs(currentNodeEntity()->position().x - originPos_.x);
		}

	protected:
		CoordinateNode* pNode_;
		CoordinateNode* pCurrentNode_;
		const Math::Vector3& originPos_;
	};

	class CoordinateNodeWrapZ : public CoordinateNodeWrapX
	{
	public:
		CoordinateNodeWrapZ(CoordinateNode* node, const Math::Vector3& originPos) :
			CoordinateNodeWrapX(node, originPos) {}

		 CoordinateNode* prev() {
			pCurrentNode_ = pCurrentNode_->pPrevZ();
			return pCurrentNode_;
		}

		 CoordinateNode* next() {
			pCurrentNode_ = pCurrentNode_->pNextZ();
			return pCurrentNode_;
		}

		 int compare() {
			float v = currentNodeEntity()->position().z;
			if (v == originPos_.z)
				return 0;
			else if (v > originPos_.z)
				return 1;
			else
				return -1;
		}

		 float length() {
			return fabs(currentNodeEntity()->position().z - originPos_.z);
		}
	};

	class CoordinateNodeWrapY : public CoordinateNodeWrapX
	{
	public:
		CoordinateNodeWrapY(CoordinateNode* node, const Math::Vector3& originPos) :
			CoordinateNodeWrapX(node, originPos) {}

		 CoordinateNode* prev() {
			pCurrentNode_ = pCurrentNode_->pPrevY();
			return pCurrentNode_;
		}

		 CoordinateNode* next() {
			pCurrentNode_ = pCurrentNode_->pNextY();
			return pCurrentNode_;
		}

		 int compare() {
			float v = currentNodeEntity()->position().y;
			if (v == originPos_.y)
				return 0;
			else if (v > originPos_.y)
				return 1;
			else
				return -1;
		}

		 float length() {
			return fabs(currentNodeEntity()->position().y - originPos_.y);
		}
	};

	//-------------------------------------------------------------------------------------
	/**
	���������ĵ�����Ľڵ�
	ģ����� NODEWRAP ȡֵΪ��������֮һ��
	- CoordinateNodeWrapX
	- CoordinateNodeWrapZ
	- CoordinateNodeWrapY
	*/
	template <class NODEWRAP>
	CoordinateNode* findNearestNode(CoordinateNode* rootNode, const Math::Vector3& originPos)
	{
		CoordinateNode* pRN = NULL;
		CoordinateNode* pCoordinateNode = rootNode;

		// ���ҵ�һ��EntityNode��֧��
		{
			// ���ҵ�ǰ�ڵ㣬�Ҳ���������߱���Ѱ��
			NODEWRAP wrap(rootNode, originPos);
			do
			{
				if (wrap.isEntityNode())
				{
					pRN = wrap.currentNode();
					break;
				}
			} while (wrap.prev());

			// ����Ҳ����������ұ߱���Ѱ��
			if (!pRN)
			{
				wrap.reset();
				while (wrap.next())
				{
					if (wrap.isEntityNode())
					{
						pRN = wrap.currentNode();
						break;
					}
				}

				// �����ϲ������Ҳ���
				if (!pRN)
					return NULL;
			}
		}

		// �����������ʾһ�����ҵ��ˣ���ʼ����Ŀ��λ�������Node
		NODEWRAP wrap(pRN, originPos);
		int v = wrap.compare();

		if (v == 0)  // ���
		{
			return wrap.currentNode();
		}
		else if (v > 0)  // Entity Node�����ĵ���ұ�
		{
			pCoordinateNode = wrap.currentNode();
			while (wrap.prev())
			{
				if (wrap.isEntityNode())
				{
					// �����Ǵ����ĵ���ұ�����߱�����
					// ��˵�һ��positionС�����ĵ��entity��һ���������ĵ������
					if (wrap.compare() <= 0)
					{
						return wrap.currentNode();
					}
				}

				pCoordinateNode = wrap.currentNode();
			}
			return pCoordinateNode;
		}
		else   // Entity Node�����ĵ�����
		{
			pCoordinateNode = wrap.currentNode();
			while (wrap.next())
			{
				if (wrap.isEntityNode())
				{
					// �����Ǵ����ĵ��������ұ߱�����
					// ��˵�һ��position�������ĵ��entity��һ���������ĵ������
					if (wrap.compare() >= 0)
					{
						return wrap.currentNode();
					}
				}

				pCoordinateNode = wrap.currentNode();
			}

			return pCoordinateNode;
		}
	}

	//-------------------------------------------------------------------------------------
	/**
	����һ�����Ϸ��Ϸ�Χ��entity
	ģ����� NODEWRAP ȡֵΪ��������֮һ��
	- CoordinateNodeWrapX
	- CoordinateNodeWrapZ
	- CoordinateNodeWrapY
	*/
	template <class NODEWRAP>
	void entitiesInAxisRange(std::set<Entity*>& foundEntities, CoordinateNode* rootNode,
		const Math::Vector3& originPos, float radius, int entityUType)
	{
		CoordinateNode* pCoordinateNode = findNearestNode<NODEWRAP>(rootNode, originPos);
		if (!pCoordinateNode)
			return;

		NODEWRAP wrap(pCoordinateNode, originPos);

		// ����ڵ��Լ�Ҳ��������������Լ��ӽ�ȥ
		if (wrap.isEntityNode())
		{
			Entity* pEntity = wrap.currentNodeEntity();

			//if (entityUType == -1 || pEntity->pScriptModule()->getUType() == (ENTITY_SCRIPT_UID)entityUType)
			if (entityUType == -1)
			{
				if (wrap.length() <= radius)
				{
					foundEntities.insert(pEntity);
				}
			}
		}

		while (wrap.prev())
		{
			if (wrap.isEntityNode())
			{
				Entity* pEntity = wrap.currentNodeEntity();

				//if (entityUType == -1 || pEntity->pScriptModule()->getUType() == (ENTITY_SCRIPT_UID)entityUType)
				if (entityUType == -1)
				{
					if (wrap.length() <= radius)
					{
						foundEntities.insert(pEntity);
					}
					else
					{
						break;
					}
				}
			}
		};

		wrap.reset();

		while (wrap.next())
		{
			if (wrap.isEntityNode())
			{
				Entity* pEntity = wrap.currentNodeEntity();

				//if (entityUType == -1 || pEntity->pScriptModule()->getUType() == (ENTITY_SCRIPT_UID)entityUType)
				if (entityUType == -1)
				{
					if (wrap.length() <= radius)
					{
						foundEntities.insert(pEntity);
					}
					else
					{
						break;
					}
				}
			}
		};
	}


	EntityCoordinateNode::EntityCoordinateNode(Entity* pEntity) :
		CoordinateNode(NULL)
	{
		flags(COORDINATE_NODE_FLAG_ENTITY);
		pEntity_ = pEntity;
	}

	EntityCoordinateNode::~EntityCoordinateNode()
	{

	}

	float EntityCoordinateNode::xx() const
	{
		if (pEntity_ == NULL)
			return -FLT_MAX;

		return pEntity_->position().x;
	}
	float EntityCoordinateNode::yy() const
	{
		if (pEntity_ == NULL)
			return -FLT_MAX;
		return pEntity_->position().y;
	}
	float EntityCoordinateNode::zz() const
	{
		if (pEntity_ == NULL)
			return -FLT_MAX;
		return pEntity_->position().z;
	}

	void EntityCoordinateNode::update()
	{
		CoordinateNode::update();

		std::vector<CoordinateNode*>::iterator iter = watcherNodes_.begin();
		for (; iter != watcherNodes_.end(); ++iter)
		{
			(*iter)->update();
		}
	}

	//-------------------------------------------------------------------------------------
	void EntityCoordinateNode::onRemove()
	{
		std::vector<CoordinateNode*>::iterator iter = watcherNodes_.begin();
		for (; iter != watcherNodes_.end(); ++iter)
		{
			(*iter)->onParentRemove(this);
		}

		CoordinateNode::onRemove();
	}

	//-------------------------------------------------------------------------------------
	bool EntityCoordinateNode::addWatcherNode(CoordinateNode* pNode)
	{
		std::vector<CoordinateNode*>::iterator iter = std::find(watcherNodes_.begin(), watcherNodes_.end(), pNode);
		if (iter != watcherNodes_.end())
			return false;

		watcherNodes_.push_back(pNode);
		return true;
	}

	//-------------------------------------------------------------------------------------
	bool EntityCoordinateNode::delWatcherNode(CoordinateNode* pNode)
	{
		std::vector<CoordinateNode*>::iterator iter = std::find(watcherNodes_.begin(), watcherNodes_.end(), pNode);
		if (iter == watcherNodes_.end())
			return false;

		watcherNodes_.erase(iter);
		return true;
	}

	//-------------------------------------------------------------------------------------
	void EntityCoordinateNode::entitiesInRange(std::vector<Entity*>& foundEntities, CoordinateNode* rootNode,
		const Math::Vector3& originPos, float radius, int entityUType)
	{
		std::set<Entity*> entities_X;
		std::set<Entity*> entities_Z;

		entitiesInAxisRange<CoordinateNodeWrapX>(entities_X, rootNode, originPos, radius, entityUType);
		entitiesInAxisRange<CoordinateNodeWrapZ>(entities_Z, rootNode, originPos, radius, entityUType);

		// ����Y
		//if (CoordinateSystem::hasY)
		{
			std::set<Entity*> entities_Y;
			entitiesInAxisRange<CoordinateNodeWrapY>(entities_Y, rootNode, originPos, radius, entityUType);


			std::set<Entity*> res_set;
			set_intersection(entities_X.begin(), entities_X.end(), entities_Z.begin(), entities_Z.end(), std::inserter(res_set, res_set.end()));
			set_intersection(res_set.begin(), res_set.end(), entities_Y.begin(), entities_Y.end(), std::back_inserter(foundEntities));
		}
		//else
		//{
		//	set_intersection(entities_X.begin(), entities_X.end(), entities_Z.begin(), entities_Z.end(), std::back_inserter(foundEntities));
		//}
	}
}