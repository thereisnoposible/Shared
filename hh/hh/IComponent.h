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
	power_type_hp_rec,		//回血
	power_type_mc_rec,		//回蓝
};

enum com_type
{
    com_type_nothing,
    com_type_head,
    com_type_body,
    com_type_left_hand,
    com_type_right_hand,
    com_type_left_foot,
    com_type_right_foot,
};

enum com_property
{
	com_property_self,
	com_property_all,
};

struct IComponentPower
{
	int32 value;
	int32 maxvalue;
};

class IComponent
{
public:
	static IComponent* getIComponent()
	{
		IComponent* p = new IComponent();
		return p;
	}

	static void freeIComponent(IComponent* p)
	{
		p->RemoveSelf();
		delete p;
	}

    IComponent(com_type p = com_type_nothing)
    {
        parent = nullptr;
        type = p;
    }

	void RemoveSelf()
	{
		if (parent)
		{
			parent->DelComponent(this);
		}

		for (size_t i = 0; i < pTimer.size(); i++)
		{
			sApp.GetTimeWheel().RemoveTimer(pTimer[i]);
		}
		pTimer.clear();

		std::unordered_map<int32, std::set<IComponent*>>::iterator it = sub_Component.begin();
		for (; it != sub_Component.end(); ++it)
		{
			std::set<IComponent*>::iterator iit = it->second.begin();
			for (; iit != it->second.end(); ++iit)
			{
				(*iit)->parent = nullptr;
			}
		}
	}

    void AddComponent(IComponent* p)
    {
		if (p->parent)
		{
			p->parent->DelComponent(p);
		}

        sub_Component[p->type].insert(p);
        p->parent = this;
    }

    void DelComponent(IComponent* p)
    {
        sub_Component[p->type].erase(p);
        p->parent = nullptr;
    }

	std::unordered_map<int32, IComponentPower>& GetPower()
    {       
        return power;
    }

	void SetPowerLimit(int32 type, int32 maxnum)
	{
		power[type].maxvalue = maxnum;
	}

	void AddPower(int32 type, int32 num)
    {
        power[type].value += num;
		if (power[type].value > power[type].maxvalue)
			power[type].value = power[type].maxvalue;
    }

	void AddIntervalPower(int32 type, int32 num, int32 interval)
	{
		timewheel::Timer* p = sApp.GetTimeWheel().AddTimer(interval, std::bind(&IComponent::AddPower, this, type, num));
		pTimer.push_back(p);
	}

    IComponent* parent;
	std::unordered_map<int32, IComponentPower> power;
    std::unordered_map<int32, std::set<IComponent*>> sub_Component;
    com_type type;

	std::vector<timewheel::Timer*> pTimer;
};