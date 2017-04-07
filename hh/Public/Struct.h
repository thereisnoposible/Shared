#pragma once
//#include "../new/proto/protobuf/hello.pb.h"
#include "../new/proto/protobuf/player.pb.h"
#include "../new/proto/protobuf/login.pb.h"
#include "../new/proto/protobuf/xinfa.pb.h"

typedef unsigned int uint;
#ifdef _MSC_VER
typedef __int8  int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#else
typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
#endif

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

	}
	void fromPBMessage(const pm_playerdata& data)
	{
		 id = data.id();
		 jinbi = data.jinbi();
		 name = data.name();
         account = data.account();
	};

	/**
	* @brief		转换成PBMessage
	*/
	void toPBMessage(pm_playerdata& data)
	{
		data.set_id(id);
		data.set_jinbi(jinbi);
		data.set_name(name);
        data.set_account(account);
	};
	int32 id;
	int32 jinbi;
	
	std::string name;
	std::string account;
};

struct Xinfa
{
	void fromPBMessage(const pm_xinfa& data)
	{
		dbid = data.dbid();
		playerid = data.playerid();
		id = data.id();
		level = data.level();
		exp = data.exp();
	};

	/**
	* @brief		转换成PBMessage
	*/
	void toPBMessage(pm_xinfa& data)
	{
		data.set_dbid(dbid);
		data.set_playerid(playerid);
		data.set_id(id);
		data.set_level(level);
		data.set_exp(exp);
	};

	int64 dbid;
	int32 playerid;
	int32 id;
	int32 level;
	int32 exp;

	int32 mp;
	int32 max_mp;
};

struct TypeXinfa
{
	int32 id;
	int32 add_ratio;
	int32 add_count;
	int32 recovery_mp_ratio;
	int32 recovery_mp_count;
	int32 damage_ratio;
	int32 damage_count;
	int32 recovery_hp_ratio;
	int32 recovery_hp_count;
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


struct EntityData
{
    long long llData[1024];
    char cData[1024][1024];
};