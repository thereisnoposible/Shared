#pragma once
#include "NetService.h"
#include "Player.h"
#include "ModuleManager.h"
#include "Log.h"
#include "PlayerMgr.h"
#include "MapManager.h"
#include "TimerManager.h"
#include "NetClient.h"
#include "AccountNetClient.h"

class Application : public NetObserver, public Singleton<Application>
{
public:
	Application();
	~Application();
	void run();
	void update(double diff);

	void OnConnect(ConnectPtr&);
	void OnDisConnect(ConnectPtr&);

	int64 getDBID();

    AccountNetClient& GetAccountConnect();
	NetClient& GetDataBaseConnect();

    void AllInitialOK();
private:
    AccountNetClient* m_pAccount;
	NetClient* m_pDatabase;

	NetService* m_pNetService;
	ModuleManager *m_pModuleMgr;
	LogService* m_pLogServer;
	TypeTable* m_pTypeTable;
	PlayerManager* m_pPlayerMgr;
	MapManager* m_pMapMgr;
	TimerManager* m_pTimerManager;

	DBService* m_pDBService;

	int64 m_dbid;
	int32 m_increment;
	int64 m_id;
};