#pragma once
#include <unordered_map>
#include "Application.h"

enum power_type
{
	power_type_nothing = 0,
	power_type_attack,      //攻击
	power_type_equip,       //装备
	power_type_walk,        //走路
	power_type_run,         //跑步
	power_type_fly,         //飞行


	power_type_hp,			//气血
	power_type_mp,			//回蓝
	power_type_power,
};

enum com_type
{
	com_type_nothing,
	com_type_head,
	com_type_body,
	com_type_hand,
	com_type_foot,
};

enum com_property
{
	com_property_self,
	com_property_parent,
};

struct IComponentPower
{
	int32 type;
	int32 value = 0;
	int32 maxvalue = 0x7FFFFFFF;
	com_property proper;
};

struct IComponentCB
{
	std::function<bool(int32)> comp;
	std::function<void(INode*)> callback;
};

class INode
{
public:
	static INode* Create(com_type pp = com_type_nothing)
	{
		INode* p = new INode(pp);
		return p;
	}

	//只有在确定不使用了 才可以释放
	static void Free(INode* p)
	{
		p->RemoveSelf();
		delete p;
	}

	static void delayFreeINode(INode* p, long long interval)
	{
		tTimer->AddTimer(interval, std::bind(&INode::Free, p), 1);
	}

	static timewheel::TimeWheel* tTimer;

	void toPMessage(pm_icomponent_data& data)
	{
		data.set_dbid((int64)this);
		data.set_type(type);
		data.set_bactive(bActive);
		auto pit = components.begin();
		for (; pit != components.end(); ++pit)
		{
			auto sit = pit->second.begin();
			for (; sit != pit->second.end(); ++sit)
			{
				pm_icomponent_power* pData = data.add_powers();
				pData->set_type(sit->second.type);
				pData->set_value(sit->second.value);
				pData->set_maxvalue(sit->second.maxvalue);
				pData->set_proper(sit->second.proper);
			}
		}

		auto sit = childnode.begin();
		for (; sit != childnode.end(); ++sit)
		{
			pm_icomponent_data* pData = data.add_sub_datas();
			(*sit)->toPMessage(*pData);
		}

	}

	void RemoveSelf()
	{
		if (parent)
		{
			parent->DelINode(this);
		}

		for (size_t i = 0; i < pTimer.size(); i++)
		{
			tTimer->RemoveTimer(pTimer[i]);
		}
		pTimer.clear();

		components.clear();

		auto it = childnode.begin();
		for (; it != childnode.end(); ++it)
		{
			(*it)->parent = nullptr;
		}

		bActive = false;
	}

	void AddINode(INode* p)
	{
		if (bActive == false)
			return;

		assert(p != this);

		if (p->parent)
		{
			p->parent->DelINode(p);
		}

		childnode.insert(p);
		p->parent = this;
	}

	void DelINode(INode* p)
	{
		childnode.erase(p);

		if (p->parent == this)
			p->parent = nullptr;
	}

	void AddIComponent(int32 type, int32 num, com_property proper)
	{
		if (bActive == false)
			return;

		addIComponent(type, num, proper);
	}

	void AddIntervalIComponent(int32 type, int32 num, int32 interval, com_property proper)
	{
		if (bActive == false)
			return;

		timewheel::Timer* p = tTimer->AddTimer(interval, std::bind(&INode::addIComponent, this, type, num, proper));
		pTimer.push_back(p);
	}

	int32 GetTypeCount(int32 type)
	{
		if (bActive == false)
			return 0;

		int32 count = 0;
		auto it = components.find(com_property_self);
		if (it != components.end())
		{
			auto sit = it->second.find(type);
			if (sit != it->second.end())
				count += sit->second.value;
		}

		auto subit = childnode.begin();
		for (; subit != childnode.end(); ++subit)
		{
			count += (*subit)->getTypeCount(type, com_property_parent);
		}

		return count;
	}

	IComponentPower* GetComponent(int32 type, com_property proper)
	{
		if (bActive == false)
			return nullptr;

		auto it = components.find(proper);
		if (it == components.end())
			return nullptr;

		auto sit = it->second.find(type);
		if (sit == it->second.end())
			return nullptr;

		return &sit->second;
	}

	INode* GetChildNode(size_t temp)
	{
		INode* p = (INode*)temp;

		auto it = childnode.find(p);
		if (it == childnode.end())
			return nullptr;

		return p;
	}

	void SetCallBack(int32 type, std::function<bool(int32)> cmpp, std::function<void(INode*)> cb)
	{
		IComponentCB temp;
		temp.comp = cmpp;
		temp.callback = cb;

		callback[type] = temp;
	}

	INode* parent;
	std::set<INode*> childnode;
	std::unordered_map<com_property, std::unordered_map<int32, IComponentPower>> components;
	
	com_type type;

	std::vector<timewheel::Timer*> pTimer;
	std::unordered_map<int32, IComponentCB> callback;

	bool bActive = true;
private:
	void addIComponent(int32 type, int32 num, com_property proper)
	{
		auto it = components.find(proper);
		if (it == components.end())
		{
			components[proper] = std::unordered_map<int32, IComponentPower>();
			it = components.find(proper);
		}

		std::unordered_map<int32, IComponentPower>& temp = it->second;

		temp[type].type = type;
		temp[type].proper = proper;
		temp[type].value += num;
		if (temp[type].value > temp[type].maxvalue)
			temp[type].value = temp[type].maxvalue;

		if (proper == com_property_self)
		{
			int32 count = GetTypeCount(type);

			if (callback.find(type) != callback.end())
			{
				if (callback[type].comp(count))
				{
					callback[type].callback(this);
				}
			}
		}
	}

	int32 getTypeCount(int32 type, com_property proper)
	{
		if (bActive == false)
			return 0;

		int32 count = 0;
		auto it = components.find(proper);
		if (it != components.end())
		{
			auto sit = it->second.find(type);
			if (sit != it->second.end())
				count += sit->second.value;
		}

		if (proper == com_property_self)
		{
			auto subit = childnode.begin();
			for (; subit != childnode.end(); ++subit)
			{
				count += (*subit)->getTypeCount(type, com_property_parent);
			}
		}

		return count;
	}

	INode(com_type p = com_type_nothing)
	{
		parent = nullptr;
		type = p;
	}

	~INode(){}
};