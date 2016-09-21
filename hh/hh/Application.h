#pragma once
#include "NetService.h"
#include "Player.h"
#include "ModuleManager.h"
#include "Log.h"
#include "FightManager.h"
#include "PlayerMgr.h"
#include "MapManager.h"
#include "TimerManager.h"
#include "NetClient.h"

class Application : public NetObserver, public Singleton<Application>
{
public:
	Application();
	~Application();
	void run();
	void update(double diff);

	void OnConnect(ConnectPtr&);
	void OnDisConnect(ConnectPtr&);

	NetClient& GetAccountConnect();
private:
	NetClient* m_pAccount;

	NetService* m_pNetService;
	ModuleManager *m_pModuleMgr;
	LogService* m_pLogServer;
	TypeTable* m_pTypeTable;
	FightManager* m_pFightMgr;
	PlayerManager* m_pPlayerMgr;
	MapManager* m_pMapMgr;
	TimerManager* m_pTimerManager;

	DBService* m_pDBService;
};