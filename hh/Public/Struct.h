#pragma once
//#include "../new/proto/protobuf/hello.pb.h"
#include "/myproject/new/proto/protobuf/hello.pb.h"
struct VecPos
{
	int vecpos_x;
	int vecpos_y;

	unsigned int targePosX;
	unsigned int targePosY;
};

struct VecSpeed
{
	int vecspeed_x;
	int vecspeed_y;

	unsigned int targePosX;
	unsigned int targePosY;

	VecPos operator* (const long long usetime) const
	{
		VecPos vecpos;
		vecpos.vecpos_x = (int)(vecspeed_x * usetime);
		vecpos.vecpos_y = (int)(vecspeed_y * usetime);

		vecpos.targePosX = targePosX;
		vecpos.targePosY = targePosY;

		return vecpos;
	}
	VecPos operator* (const double usetime) const
	{
		VecPos vecpos;
		vecpos.vecpos_x = (int)(vecspeed_x * usetime);
		vecpos.vecpos_y = (int)(vecspeed_y * usetime);

		vecpos.targePosX = targePosX;
		vecpos.targePosY = targePosY;

		return vecpos;
	}
};

struct Vec
{
	int x;
	int y;
	unsigned int targePosX;
	unsigned int targePosY;

	//VecSpeed operator * (const SPEED& speed) const
	//{
	//	VecSpeed vecspeed;
	//	vecspeed.vecspeed_x = x*speed.speed_x;
	//	vecspeed.vecspeed_y = y*speed.speed_y;
	//	vecspeed.targePosX = targePosX;
	//	vecspeed.targePosY = targePosY;
	//	return vecspeed;
	//}
};

struct SPEED
{
	SPEED(int x, int y)
	{
		speed_x = x;
		speed_y = y;
	}
	unsigned int speed_x;
	unsigned int speed_y;

	VecSpeed operator * (const Vec& vec) const
	{
		VecSpeed vecspeed;
		vecspeed.vecspeed_x = vec.x * speed_x;
		vecspeed.vecspeed_y = vec.y * speed_y;
		vecspeed.targePosX = vec.targePosX;
		vecspeed.targePosY = vec.targePosY;
		return vecspeed;
	}
};

struct POS
{
	POS() : pos_x(0), pos_y(0), mapid(0)
	{

	}
	int pos_x;
	int pos_y;
	int mapid;
	POS(int x,int y) : pos_x(x), pos_y(y)
	{

	}
	POS operator+(const VecPos& vecPos) const
	{
		POS pos;
		int temp1 = vecPos.vecpos_x > 0 ? vecPos.vecpos_x : -vecPos.vecpos_x;
		int temp = (pos_x - vecPos.targePosX);
		int temp2 = temp > 0 ? temp : -temp;

		if (temp1 >= temp2)
		{
			pos.pos_x = vecPos.targePosX;
		}
		else
		{
			pos.pos_x = pos_x+vecPos.vecpos_x;
		}
		temp1 = vecPos.vecpos_y > 0 ? vecPos.vecpos_y : -vecPos.vecpos_y;
		temp = (pos_y - vecPos.targePosY);
		temp2 = temp > 0 ? temp : -temp;
		if (temp1 >= temp2)
		{
			pos.pos_y = vecPos.targePosY;
		}
		else
		{
			pos.pos_y = pos_y + vecPos.vecpos_y;
		}

		return pos;
	}

	POS& operator+=(const VecPos& vecPos)
	{
		*this = *this + vecPos;
		return *this;
	}
};

class MOVEINFO
{
public:
	MOVEINFO(int speed_x, int speed_y, int pos_x = 0, int pos_y = 0) : pos(pos_x, pos_y), speed(speed_x, speed_y){};
	POS pos;
	SPEED speed;
	Vec vec;			//方向
};

class Pawn
{
public:
	
	Pawn(int speed_x, int speed_y, int pos_x = 0, int pos_y = 0) :moveinfo(speed_x, speed_y, pos_x, pos_y){};
	MOVEINFO moveinfo;
	void calcVec(unsigned int x, unsigned int y)
	{
		moveinfo.vec.targePosX = x;
		moveinfo.vec.targePosY = y;
		if ((int)x > moveinfo.pos.pos_x)
			moveinfo.vec.x = 1;
		else if (x == moveinfo.pos.pos_x)
			moveinfo.vec.x = 0;
		else
			moveinfo.vec.x = -1;

		if ((int)y > moveinfo.pos.pos_y)
			moveinfo.vec.y = 1;
		else if (y == moveinfo.pos.pos_y)
			moveinfo.vec.y = 0;
		else
			moveinfo.vec.y = -1;
	}
};

struct PlayerData
{
	PlayerData()
	{
		memset(this, 0, sizeof(PlayerData)-2*sizeof(std::string));
	}
	void fromPBMessage(const pm_playerdata& data)
	{
		 id = data.id();
		 jinbi = data.jinbi();

		 hp = data.hp();
		 maxhp = data.maxhp();
		 mp = data.mp();
		 maxmp = data.maxmp();

		 liliang = data.liliang();
		 minjie = data.minjie();
		 sudu = data.sudu();
		  danshi = data.danshi();
		  jingqi = data.jingqi();
		 meili = data.meili();

		 liliang_exp = data.liliangexp();
		 minjie_exp = data.minjieexp();
		 sudu_exp = data.suduexp();
		 danshi_exp = data.danshiexp();
		 jingqi_exp = data.jingqiexp();
		 meili_exp = data.meiliexp();

		 max_liliang = data.maxliliang();
		 max_minjie = data.maxminjie();
		 max_sudu = data.maxsudu();
		 max_danshi = data.maxdanshi();
		 max_jingqi = data.maxjingqi();
		 max_meili = data.maxmeili();

		 gengu = data.gengu();
		 wuxing = data.wuxing();
		 fuyuan = data.fuyuan();

		 lixing = data.lixing();
		 ganxing = data.ganxing();

		 xuedian = data.xuedian();
		 name = data.name();
	};

	/**
	* @brief		转换成PBMessage
	*/
	void toPBMessage(pm_playerdata& data)
	{
		data.set_id(id);
		data.set_jinbi(jinbi);

		data.set_hp(hp);
		data.set_maxhp(maxhp);
		data.set_mp(mp);
		data.set_maxmp(maxmp);

		data.set_liliang(liliang);
		data.set_minjie(minjie);
		data.set_sudu(sudu);
		data.set_danshi(danshi);
		data.set_jingqi(jingqi);
		data.set_meili(meili);

		data.set_liliangexp(liliang_exp);
		data.set_minjieexp(minjie_exp);
		data.set_suduexp(sudu_exp);
		data.set_danshiexp(danshi_exp);
		data.set_jingqiexp(jingqi_exp);
		data.set_meiliexp(meili_exp);

		data.set_maxliliang(max_liliang);
		data.set_maxminjie(max_minjie);
		data.set_maxsudu(max_sudu);
		data.set_maxdanshi(max_danshi);
		data.set_maxjingqi(max_jingqi);
		data.set_maxmeili(max_meili);

		data.set_gengu(gengu);
		data.set_wuxing(wuxing);
		data.set_fuyuan(fuyuan);

		data.set_lixing(lixing);
		data.set_ganxing(ganxing);

		data.set_xuedian(xuedian);
		data.set_name(name);
	};
	unsigned int id;
	int jinbi;

	int hp;
	int maxhp;
	int mp;
	int maxmp;

	int liliang;
	int minjie;
	int sudu;
	int danshi;
	int jingqi;
	int meili;

	int liliang_exp;
	int minjie_exp;
	int sudu_exp;
	int danshi_exp;
	int jingqi_exp;
	int meili_exp;

	int max_liliang;
	int max_minjie;
	int max_sudu;
	int max_danshi;
	int max_jingqi;
	int max_meili;

	int gengu;
	int wuxing;
	int fuyuan;

	int lixing;
	int ganxing;

	int xuedian;
	
	int cellid;
	std::string name;
	std::string acc_id;
};

struct Wugong
{
	long long dbid;
	int id;
	int playerid;
	int type;
	int level;
	int exp;
};
struct WugongType
{
	int id;
	int type;
	int quality;
};
struct WugongLevelUpType
{
	int level;
	int quality;
	int needxuedian;
};
struct PropType
{
	int id;
	int type;
	int sub_type;
	int baccumulate;
};

struct TypeYaoWu
{
	int addhp;
	int addmp;
};

struct TypeTeshuYaoWu
{
	int addliliang;
	int addminjie;
	int addsudu;
	int adddanshi;
	int addjingqi;
	int addmeili;

	int addgengu;
	int addwuxing;
	int addfuyuan;

	int addlixing;
	int addganxing;
};

struct TypeWeapon
{
	int addWaigong;
};

struct TypeArmor
{
	int reduceWaigongDmg;
};

struct PropItem
{
	PropItem()
	{
		memset(this, 0, sizeof(PropItem));
	}
	long long dbid;
	int id;
	int naijiu;
	int num;
	union 
	{
		TypeYaoWu yaowu;
		TypeTeshuYaoWu teshuyaowu;
		TypeWeapon Weapon;
		TypeArmor Armor;
	};
	

};