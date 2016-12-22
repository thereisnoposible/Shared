#pragma once

#define ON_PLAYER_NETMSG(msgid,func) m_pOwner->RegisterNetMsgHandle(msgid,boost::bind(&func,this,_1))
#define ON_PLAYER_EVENT(msgid,func) m_pOwner->RegisterEventHandle(msgid,boost::bind(&func,this,_1))
#define ON_PLAYER_EVENTRANGE(eid_s,eid_e,func) m_pOwner->RegisterEventRange(eid_s,eid_e,boost::bind(&func,this,_1))

#include "g3dlite/G3D/Vector3.h"
#include "aoi/Entity.h"
typedef G3D::Vector3							Position3D;
typedef G3D::Vector3							Vector3;

struct Direction3D																										// 表示方向位置变量类型

{
	Direction3D() :dir(0.f, 0.f, 0.f) {};
	Direction3D(const Vector3 & v) :dir(v){}
	Direction3D(float r, float p, float y) :dir(r, p, y){}

	float roll() const{ return dir.x; }
	float pitch() const{ return dir.y; }
	float yaw() const{ return dir.z; }

	void roll(float v){ dir.x = v; }
	void pitch(float v){ dir.y = v; }
	void yaw(float v){ dir.z = v; }

	// roll, pitch, yaw
	Vector3 dir;
};

class Player :public AOI::Entity
{
public:
	typedef MessageHandle<PackPtr> NetMsgHandle;

	enum PlayerState
	{
		psOnline,
		psOffline,
	};

	enum PlayerStatus
	{
		psFree,
		psDazuo,
		psTiaoxi,
	};
public:
	
	Player(PlayerData& p);
	~Player();

	void OnPlyaerLogin(ConnectPtr& conn);
	void OnPlyaerLogout();
	void AddModule(BaseModule* pModule);
	BaseModule* GetModule(std::string modulename);

	void SendProtoBuf(int messageid, ::google::protobuf::Message &proto);
	void TriggerEvent(const BaseEvent& edata);
	void RegisterEventHandle(int evt, EventHandle::handle fun);
	void RegisterEventRange(int evt_s, int evt_e, EventHandle::handle fun);
	void RegisterNetMsgHandle(int msgid, NetMsgHandle::handle fun);
	void FireNetMsg(int msgid, PackPtr& pPack);

	void SetPlayerState(PlayerState _state);
	PlayerState GetPlayerState();
	void SetPlayerStatus(PlayerStatus _status);
	PlayerStatus GetPlayerStatus();

	void AddProp(PropItem& item);	
	int GetPlayerID();
	PlayerData& GetPlayerData();

	int GetCellID();
	void SetCellID(int id);
	int GetXuedian();
	void Addxuedian(int i);
	void SetPlayerID(int id);
	std::string GetPlayerAddress();

	int32 GetGengu();
	void AddHP(int32 num);

	ConnectPtr& GetConnect(){ return m_pConnect; }

	void onOtherEntityMove(Entity* entity);

	int speed;
protected:
	void processRequestMove(PackPtr& pPack);
private:
	PlayerData _playerdata;
	std::chrono::steady_clock::time_point starttime;

	EventHandle m_EventPump;				/**< 事件泵 */
	NetMsgHandle m_NetMsgPump;				/** 网络消息泵 */

	std::hash_map<std::string, BaseModule*> m_pModuleMap;
	int _UniqueCount;

	ConnectPtr m_pConnect;

	PlayerState state;
	PlayerStatus status;
};