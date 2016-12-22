#include "Witness.h"
#include "Entity.h"
#include "AOITrigger.h"
#include "EntityRef.h"

#define SAFE_RELEASE(p){if(p!=nullptr){delete p;p=nullptr;}}

namespace AOI
{
	Witness::Witness() :
		pEntity_(nullptr),
		aoiRadius_(0.0f),
		aoiHysteresisArea_(5.0f),
		pAOITrigger_(nullptr),
		pAOIHysteresisAreaTrigger_(nullptr),
		aoiEntities_(),
		clientAOISize_(0)
	{
	}

	//-------------------------------------------------------------------------------------
	Witness::~Witness()
	{
		pEntity_ = nullptr;
		SAFE_RELEASE(pAOITrigger_);
		SAFE_RELEASE(pAOIHysteresisAreaTrigger_);
	}

	void Witness::pEntity(Entity* pEntity)
	{
		pEntity_ = pEntity;
	}
	Entity* Witness::pEntity()
	{
		return pEntity_;
	}

	//-------------------------------------------------------------------------------------
	void Witness::attach(Entity* pEntity, float radius, float hyst)
	{
		pEntity_ = pEntity;
		lastBasePos.z = -FLT_MAX;
		setAoiRadius(radius, hyst);
	}

	//-------------------------------------------------------------------------------------
	void Witness::detach(Entity* pEntity)
	{
		clear(pEntity);
	}

	//-------------------------------------------------------------------------------------
	void Witness::clear(Entity* pEntity)
	{
		for (unsigned int i = 0; i < aoiEntities_.size(); i++)
		{
			aoiEntities_[i]->pEntity()->delWitnessed(pEntity_);
			delete aoiEntities_[i];			
		}

		pEntity_ = NULL;
		aoiRadius_ = 0.0f;
		aoiHysteresisArea_ = 5.0f;
		clientAOISize_ = 0;

		SAFE_RELEASE(pAOITrigger_);
		SAFE_RELEASE(pAOIHysteresisAreaTrigger_);

		aoiEntities_.clear();
	}

	//-------------------------------------------------------------------------------------
	const Math::Vector3& Witness::basePos()
	{
		return pEntity()->position();
	}

	//-------------------------------------------------------------------------------------
	void Witness::onEnterSpace(Space* pSpace)
	{
		installAOITrigger();
	}

	//-------------------------------------------------------------------------------------
	void Witness::onLeaveSpace(Space* pSpace)
	{
		uninstallAOITrigger();

		lastBasePos.z = -FLT_MAX;

		for (unsigned int i = 0; i < aoiEntities_.size(); ++i)
		{
			if ((aoiEntities_[i])->pEntity())
			{
				(aoiEntities_[i])->pEntity()->delWitnessed(pEntity_);
			}

			delete (aoiEntities_[i]);
		}

		aoiEntities_.clear();
		clientAOISize_ = 0;
	}

	//-------------------------------------------------------------------------------------
	void Witness::installAOITrigger()
	{
        if (pAOITrigger_)
        {
            // ������AOI�뾶Ϊ0������ص�½������������
            if (aoiRadius_ <= 0.f)
                return;

            // �����Ȱ�װpAOIHysteresisAreaTrigger_������һЩ�����������ִ���Ľ��
            // ���磺һ��Avatar���ý��뵽�����ʱ���ڰ�װAOI������������װ���������ʵ��onWitnessed������������������
            // ����AOI��������δ��ȫ��װ��ϵ��´������Ľڵ�old_xx�ȶ�Ϊ-FLT_MAX�����Ը�ʵ�����뿪���������ʱAvatar��AOI�������жϴ���
            // ����Ȱ�װpAOIHysteresisAreaTrigger_�򲻻ᴥ��ʵ�����AOI�¼��������ڰ�װpAOITrigger_ʱ�����¼�����������ֵ�����ʱҲ��֮ǰ�����뿪�¼���
            if (pAOIHysteresisAreaTrigger_ && pEntity_/*�������̿��ܵ������� */)
                pAOIHysteresisAreaTrigger_->reinstall((CoordinateNode*)pEntity_->pEntityCoordinateNode());

            if (pEntity_/*�������̿��ܵ������� */)
                pAOITrigger_->reinstall((CoordinateNode*)pEntity_->pEntityCoordinateNode());
        }
	}

	//-------------------------------------------------------------------------------------
	void Witness::uninstallAOITrigger()
	{
		if (pAOITrigger_)
			pAOITrigger_->uninstall();

		if (pAOIHysteresisAreaTrigger_)
			pAOIHysteresisAreaTrigger_->uninstall();

		// ֪ͨ����ʵ���뿪AOI
		std::vector<EntityRef*>::iterator iter = aoiEntities_.begin();
		for (; iter != aoiEntities_.end(); ++iter)
		{
			_onLeaveAOI((*iter));
		}
	}


	//-------------------------------------------------------------------------------------
	void Witness::setAoiRadius(float radius, float hyst)
	{
		aoiRadius_ = radius;
		aoiHysteresisArea_ = hyst;

		// ����λ��ͬ��ʹ�������λ��ѹ�����䣬���÷�ΧΪ-512~512֮�䣬��˳�����Χ������ͬ������
		// ������һ�����ƣ������Ҫ�������ֵ�ͻ���Ӧ�õ������굥λ����������Ŵ�ʹ�á�
		// �ο�: MemoryStream::appendPackXZ
		if (aoiRadius_ + aoiHysteresisArea_ > 512)
		{
			aoiRadius_ = 512 - 5.0f;
			aoiHysteresisArea_ = 5.0f;
			return;
		}

		if (aoiRadius_ > 0.f && pEntity_)
		{
			if (pAOITrigger_ == NULL)
			{
				pAOITrigger_ = new AOITrigger((CoordinateNode*)pEntity_->pEntityCoordinateNode(), aoiRadius_, aoiRadius_);

				// ���ʵ���Ѿ��ڳ����У���ô��Ҫ��װ
				if (((CoordinateNode*)pEntity_->pEntityCoordinateNode())->pCoordinateSystem())
					pAOITrigger_->install();
			}
			else
			{
				pAOITrigger_->update(aoiRadius_, aoiRadius_);

				// ���ʵ���Ѿ��ڳ����У���ô��Ҫ��װ
				if (!pAOITrigger_->isInstalled() && ((CoordinateNode*)pEntity_->pEntityCoordinateNode())->pCoordinateSystem())
					pAOITrigger_->install();
			}

			if (aoiHysteresisArea_ > 0.01f && pEntity_/*����update���̿��ܵ������� */)
			{
				if (pAOIHysteresisAreaTrigger_ == NULL)
				{
					pAOIHysteresisAreaTrigger_ = new AOITrigger((CoordinateNode*)pEntity_->pEntityCoordinateNode(),
						aoiHysteresisArea_ + aoiRadius_, aoiHysteresisArea_ + aoiRadius_);

					if (((CoordinateNode*)pEntity_->pEntityCoordinateNode())->pCoordinateSystem())
						pAOIHysteresisAreaTrigger_->install();
				}
				else
				{
					pAOIHysteresisAreaTrigger_->update(aoiHysteresisArea_ + aoiRadius_, aoiHysteresisArea_ + aoiRadius_);

					// ���ʵ���Ѿ��ڳ����У���ô��Ҫ��װ
					if (!pAOIHysteresisAreaTrigger_->isInstalled() && ((CoordinateNode*)pEntity_->pEntityCoordinateNode())->pCoordinateSystem())
						pAOIHysteresisAreaTrigger_->install();
				}
			}
			else
			{
				// ע�⣺�˴����������pAOIHysteresisAreaTrigger_�������update
				// ��Ϊ�뿪AOI���ж����pAOIHysteresisAreaTrigger_���ڣ���ô�������pAOIHysteresisAreaTrigger_�����AOI
				if (pAOIHysteresisAreaTrigger_)
					pAOIHysteresisAreaTrigger_->update(aoiHysteresisArea_ + aoiRadius_, aoiHysteresisArea_ + aoiRadius_);
			}
		}
		else
		{
			uninstallAOITrigger();
		}
	}

	//-------------------------------------------------------------------------------------
	 float Witness::aoiRadius() const
	{
		return aoiRadius_;
	}

	//-------------------------------------------------------------------------------------
	 float Witness::aoiHysteresisArea() const
	{
		return aoiHysteresisArea_;
	}

	//-------------------------------------------------------------------------------------
	void Witness::onEnterAOI(AOITrigger* pAOITrigger, Entity* pEntity)
	{
		// ����������Hysteresis������ô����������
		if (pAOIHysteresisAreaTrigger_ == pAOITrigger)
			return;

		pEntity_->onEnteredAoI(pEntity);

		EntityRef* iter = nullptr;

		for (unsigned int i = 0; i < aoiEntities_.size(); i++)
		{
			if (aoiEntities_[i]->id() == pEntity->id())
			{
				iter = aoiEntities_[i];
				break;
			}
		}

		if (iter != nullptr)
		{
			if (((iter)->flags() & ENTITYREF_FLAG_LEAVE_CLIENT_PENDING) > 0)
			{
				//DEBUG_MSG(boost::format("Witness::onEnterAOI: {} entity={}\n", 
				//	pEntity_->id(), pEntity->id()));

				// ���flags��ENTITYREF_FLAG_LEAVE_CLIENT_PENDING | ENTITYREF_FLAG_NORMAL״̬��ô����
				// ֻ��Ҫ�����뿪״̬�����仹ԭ��ENTITYREF_FLAG_NORMAL����
				// �����ENTITYREF_FLAG_LEAVE_CLIENT_PENDING״̬��ô��ʱӦ�ý�������Ϊ����״̬ ENTITYREF_FLAG_ENTER_CLIENT_PENDING
				if (((iter)->flags() & ENTITYREF_FLAG_NORMAL) > 0)
					(iter)->flags(ENTITYREF_FLAG_NORMAL);
				else
					(iter)->flags(ENTITYREF_FLAG_ENTER_CLIENT_PENDING);

				(iter)->pEntity(pEntity);
				pEntity->addWitnessed(pEntity_);
			}

			return;
		}

		//DEBUG_MSG(boost::format("Witness::onEnterAOI: {} entity={}\n", 
		//	pEntity_->id(), pEntity->id()));

		EntityRef* pEntityRef = new EntityRef(pEntity);
		pEntityRef->flags(pEntityRef->flags() | ENTITYREF_FLAG_ENTER_CLIENT_PENDING);
		aoiEntities_.push_back(pEntityRef);

		pEntity->addWitnessed(pEntity_);
	}

	//-------------------------------------------------------------------------------------
	void Witness::onLeaveAOI(AOITrigger* pAOITrigger, Entity* pEntity)
	{
		// ������ù�Hysteresis������ô�뿪Hysteresis��������뿪AOI
		if (pAOIHysteresisAreaTrigger_ && pAOIHysteresisAreaTrigger_ != pAOITrigger)
			return;

        pEntity_->onLeaveAoI(pEntity);

		EntityRef* iter = nullptr;

		for (unsigned int i = 0; i < aoiEntities_.size(); i++)
		{
			if (aoiEntities_[i]->id() == pEntity->id())
			{
				iter = aoiEntities_[i];
				break;
			}
		}

		if (iter == nullptr)
			return;

		_onLeaveAOI(iter);
	}

	//-------------------------------------------------------------------------------------
	void Witness::_onLeaveAOI(EntityRef* pEntityRef)
	{
		//DEBUG_MSG(boost::format("Witness::onLeaveAOI: {} entity={}\n", 
		//	pEntity_->id(), pEntityRef->id()));

		// ���ﲻdelete�� ������Ҫ��update������Ϊ�������ͻ���ʱ�ٽ���
		//delete (*iter);
		//aoiEntities_.erase(iter);

		pEntityRef->flags(((pEntityRef->flags() | ENTITYREF_FLAG_LEAVE_CLIENT_PENDING) & ~(ENTITYREF_FLAG_ENTER_CLIENT_PENDING)));

		if (pEntityRef->pEntity())
			pEntityRef->pEntity()->delWitnessed(pEntity_);

		pEntityRef->pEntity(NULL);
	}

	//-------------------------------------------------------------------------------------
	void Witness::resetAOIEntities()
	{
		clientAOISize_ = 0;
		for (unsigned int i = 0; i < aoiEntities_.size();)
		{
			if (((aoiEntities_[i])->flags() & ENTITYREF_FLAG_LEAVE_CLIENT_PENDING) > 0)
			{
				delete (aoiEntities_[i]);
				aoiEntities_.erase(aoiEntities_.begin() + i);
				continue;
			}

			(aoiEntities_[i])->flags(ENTITYREF_FLAG_ENTER_CLIENT_PENDING);
			i++;
		}
	}

	//-------------------------------------------------------------------------------------
	 std::vector<EntityRef*>& Witness::aoiEntities()
	{
		return aoiEntities_;
	}

	  EntityRef* Witness::getAOIEntityRef(int entityID)
	 {
		 EntityRef* iter = nullptr;
		 for (unsigned int i = 0; i < aoiEntities_.size(); i++)
		 {
			 if (aoiEntities_[i]->id() == entityID)
			 {
				 iter = aoiEntities_[i];
			 }
		 }

		 return iter;
	 }

	   bool Witness::entityInAOI(int entityID)
	  {
		  EntityRef* pEntityRef = getAOIEntityRef(entityID);

		  if (pEntityRef == NULL || pEntityRef->pEntity() == NULL || pEntityRef->flags() == ENTITYREF_FLAG_UNKONWN ||
			  (pEntityRef->flags() & (ENTITYREF_FLAG_ENTER_CLIENT_PENDING | ENTITYREF_FLAG_LEAVE_CLIENT_PENDING)) > 0)
			  return false;

		  return true;
	  }

	   //-------------------------------------------------------------------------------------
	    AOITrigger* Witness::pAOITrigger()
	   {
		   return pAOITrigger_;
	   }

	   //-------------------------------------------------------------------------------------
	    AOITrigger* Witness::pAOIHysteresisAreaTrigger()
	   {
		   return pAOIHysteresisAreaTrigger_;
	   }
}