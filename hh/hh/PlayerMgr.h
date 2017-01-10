#pragma once
#include "Initialer.h"
#include "MysqlStmt.h"

class PlayerManager : public Singleton<PlayerManager>, public Initialer
{
public:
	struct Session
	{
		//Session() :playering(0){}
		unsigned int playerid;
		std::string acc_id;
	};
	PlayerManager(DBService* p);
	~PlayerManager();

	void OnDisConnect(ConnectPtr& playerid);

    std::unordered_map<ConnectPtr, Session>& GetPlayerMap();
    std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>& GetAccPlayerMap();

    MysqlStmt* m_pMysqlStmt;
protected:

	void registmessage();
	void unregistmessage();
	void init();
	void loadPlayer();

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
	std::unordered_map<std::string, ConnectPtr> m_Loging;
	
	std::hash_map<std::string, int> m_PlayerAddrToID;

	std::hash_map<std::string, Session> m_pAccPlayerGroup;

	std::unordered_map<ConnectPtr, Session> m_AllPlayer;

	std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>> m_AccPlayer;

	unsigned int _unique_player;

	DBService* m_pDBService;
};