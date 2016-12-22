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
            // 在设置AOI半径为0后掉线重登陆会出现这种情况
            if (aoiRadius_ <= 0.f)
                return;

            // 必须先安装pAOIHysteresisAreaTrigger_，否则一些极端情况会出现错误的结果
            // 例如：一个Avatar正好进入到世界此时正在安装AOI触发器，而安装过程中这个实体onWitnessed触发导致自身被销毁了
            // 由于AOI触发器并未完全安装完毕导致触发器的节点old_xx等都为-FLT_MAX，所以该实体在离开坐标管理器时Avatar的AOI触发器判断错误
            // 如果先安装pAOIHysteresisAreaTrigger_则不会触发实体进入AOI事件，这样在安装pAOITrigger_时触发事件导致上面出现的问题时也能之前捕获离开事件了
            if (pAOIHysteresisAreaTrigger_ && pEntity_/*上面流程可能导致销毁 */)
                pAOIHysteresisAreaTrigger_->reinstall((CoordinateNode*)pEntity_->pEntityCoordinateNode());

            if (pEntity_/*上面流程可能导致销毁 */)
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

		// 通知所有实体离开AOI
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

		// 由于位置同步使用了相对位置压缩传输，可用范围为-512~512之间，因此超过范围将出现同步错误
		// 这里做一个限制，如果需要过大的数值客户端应该调整坐标单位比例，将其放大使用。
		// 参考: MemoryStream::appendPackXZ
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

				// 如果实体已经在场景中，那么需要安装
				if (((CoordinateNode*)pEntity_->pEntityCoordinateNode())->pCoordinateSystem())
					pAOITrigger_->install();
			}
			else
			{
				pAOITrigger_->update(aoiRadius_, aoiRadius_);

				// 如果实体已经在场景中，那么需要安装
				if (!pAOITrigger_->isInstalled() && ((CoordinateNode*)pEntity_->pEntityCoordinateNode())->pCoordinateSystem())
					pAOITrigger_->install();
			}

			if (aoiHysteresisArea_ > 0.01f && pEntity_/*上面update流程可能导致销毁 */)
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

					// 如果实体已经在场景中，那么需要安装
					if (!pAOIHysteresisAreaTrigger_->isInstalled() && ((CoordinateNode*)pEntity_->pEntityCoordinateNode())->pCoordinateSystem())
						pAOIHysteresisAreaTrigger_->install();
				}
			}
			else
			{
				// 注意：此处如果不销毁pAOIHysteresisAreaTrigger_则必须是update
				// 因为离开AOI的判断如果pAOIHysteresisAreaTrigger_存在，那么必须出了pAOIHysteresisAreaTrigger_才算出AOI
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
		// 如果进入的是Hysteresis区域，那么不产生作用
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

				// 如果flags是ENTITYREF_FLAG_LEAVE_CLIENT_PENDING | ENTITYREF_FLAG_NORMAL状态那么我们
				// 只需要撤销离开状态并将其还原到ENTITYREF_FLAG_NORMAL即可
				// 如果是ENTITYREF_FLAG_LEAVE_CLIENT_PENDING状态那么此时应该将它设置为进入状态 ENTITYREF_FLAG_ENTER_CLIENT_PENDING
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
		// 如果设置过Hysteresis区域，那么离开Hysteresis区域才算离开AOI
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

		// 这里不delete， 我们需要待update将此行为更新至客户端时再进行
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