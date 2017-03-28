#pragma once
#include "Initialer.h"
#include "MysqlStmt.h"

class PlayerManager : public Singleton<PlayerManager>, public Initialer
{
public:
	struct Session
	{
		Player* player;
		std::string accid;
	};
	PlayerManager(DBService* p);
	~PlayerManager();

	void OnDisConnect(ConnectPtr& playerid);

    std::unordered_map<ConnectPtr, Session>& GetPlayerMap();
	std::unordered_map<int32, Player*>& GetPlayerIDMap();
	std::unordered_map<std::string, std::unordered_map<int, Player*>>& GetAccPlayerMap();

    void loadPlayer(pm_playerdata_db_response& response);

	void Broadcast(int id, const ::google::protobuf::Message& mess, int except);
protected:

	void registmessage();
	void unregistmessage();
	void init();

	void onPlayerMessage(PackPtr& pPack);

	void CreateAccount(PackPtr& pPack);

	void AccountCheck(PackPtr& pPack);
	void AccountCheckResponse(PackPtr& pPack);

	void LoginGame(PackPtr& pPack);

	void CreatePlayer(PackPtr& pPack);

	void InitPlayer(PlayerData& player);

	void HeartBeat(PackPtr& pPack);

    void InsertPlayer(PlayerData& data);

private:
	std::unordered_map<ConnectPtr, Session> m_AllPlayer;
	std::unordered_map<int32, Player*> m_AllPlayerID;
	std::unordered_map<std::string, std::unordered_map<int, Player*>> m_AccPlayer;

	unsigned int _unique_player;

	DBService* m_pDBService;
};