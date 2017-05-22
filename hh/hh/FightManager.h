#pragma once

class FightBase
{
public:
	virtual void Fight(FightInfo&) = 0;
	virtual void OnFightStart(FightInfo&) = 0;
	virtual void OnFightOver(FightInfo&) = 0;

	virtual int32 GetYisu() = 0;
	virtual int32 GetGongsu() = 0;
};

struct PlayerFightInfo
{
	FightBase* player;
	int32 yisu_value;
	int32 gongsu_value;
	int32 pos;
	int32 left_yisu;	//Ê£ÓàËÙ¶È
	int32 left_gongsu;  //Ê£ÓàÃô½Ý
};

class FightInfo
{
public:
	FightInfo(int64 d, int32 count)
	{
		dbid = d;
		Maps.resize(count);
	}

	void AddPlayer(FightBase* p, int32 pos)
	{
		PlayerFightInfo temp;
		temp.player = p;
		temp.pos = pos;
		temp.left_yisu = 0;
		temp.left_gongsu = 0;

		Players.insert(std::make_pair(p, temp));
		Yisus[p->GetYisu()]++;
		Gongsus[p->GetGongsu()]++;
		Maps[pos].insert(p);
	}

	void RemovePlayer(FightBase* p)
	{
		auto it = Players.find(p);
		if (it == Players.end())
			return;

		auto yisuit = Yisus.find(it->second.player->GetYisu());
		if (yisuit != Yisus.end())
		{
			yisuit->second -= 1;
			if (yisuit->second <= 0)
				Yisus.erase(yisuit);
		}

		auto gongsuit = Gongsus.find(it->second.player->GetGongsu());
		if (gongsuit != Gongsus.end())
		{
			gongsuit->second -= 1;
			if (gongsuit->second <= 0)
				Gongsus.erase(gongsuit);
		}

		Maps[it->second.pos].erase(p);
		Players.erase(p);
	}
	bool IsSelfFight(FightBase* p)
	{
		if (ShotSequence[ShotSequence.size() - 1].player != p)
			return false;

		return true;
	}

	void Update()
	{
		assert(Yisus.size() != Gongsus.size());
		if (Yisus.size() <= 1)
			return;

		auto it_yisu = Yisus.begin();
		it_yisu++;
		auto it_gongsu = Gongsus.begin();
		it_gongsu++;

		int32 lower_value = it_yisu->first;
		if (it_gongsu->first < it_yisu->first)
			lower_value = it_gongsu->first;

		ShotSequence.clear();
		for (auto it = Players.begin(); it != Players.end(); ++it)
		{
			PlayerFightInfo& temp = it->second;			
			
			temp.yisu_value = 0;
			int32 temp_yisu = temp.player->GetYisu() + temp.left_yisu * lower_value / 10000;
			temp.yisu_value = temp_yisu / lower_value;
			temp.left_yisu = temp_yisu % lower_value * 10000 / lower_value;			

			temp.gongsu_value = 0;
			int32 temp_gongsu = temp.player->GetGongsu() + temp.left_gongsu * lower_value / 10000;
			temp.gongsu_value = temp_gongsu / lower_value;		
			temp.left_gongsu = temp_gongsu % lower_value * 10000 / lower_value;

			ShotSequence.push_back(temp);
		}

		std::sort(ShotSequence.begin(), ShotSequence.end(), boost::bind([](PlayerFightInfo& left, PlayerFightInfo& right){
			return left.player->GetYisu() > right.player->GetYisu();
		}, _1, _2));

		ShotSequence[ShotSequence.size() - 1].player->Fight(*this);
	}

	void OnFightStart()
	{
		for (auto it = Players.begin(); it != Players.end(); ++it)
		{
			PlayerFightInfo& temp = it->second;

			temp.player->OnFightStart(*this);
		}
	}
	void RandOver(FightBase* p)
	{
		if (ShotSequence.size() <= 1)
			return;

		if (ShotSequence[ShotSequence.size() - 1].player != p)
			return;

		ShotSequence.pop_back();
		if (ShotSequence.size() != 0)
		{
			ShotSequence[ShotSequence.size() - 1].player->Fight(*this);
			return;
		}

		Update();
	}

private:
	int64 dbid;
	std::map<FightBase*, PlayerFightInfo> Players;
	std::vector<PlayerFightInfo> ShotSequence;
	std::map<int32, int32> Yisus;
	std::map<int32, int32> Gongsus;
	std::vector<std::set<FightBase*>> Maps;
};

class FightManager : public Singleton<FightManager>
{
public:
	FightManager();
	~FightManager();

	void CreateFight(FightBase* pFighter, FightBase* pDefender);
	FightInfo* GetFightInfo(int64 dbid);
private:
	std::map<int64, FightInfo> Fights;
};