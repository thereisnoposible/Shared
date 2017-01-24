#pragma once
#include "NetService.h"
#include "Log.h"
#include "TimerManager.h"
#include "NetClient.h"
#include "MysqlStmt.h"
#include "DBPlayer.h"

class Application : public Singleton<Application>
{
public:
    Application();
    ~Application();

    void Init();
    void update(double diff);

    void OnConnect(ConnectPtr&);
    void OnDisConnect(ConnectPtr&);

    MysqlStmt& GetMysqlStmt()
    {
        return *m_pMysqlStmt;
    }

    NetService& GetNetService()
    {
        return *m_pNetService;
    }

    void InitInterface();
    void DestoryInterface();
private:
    void registerMessage();

    NetService* m_pNetService;
    MysqlStmt* m_pMysqlStmt;

    DBPlayer* m_pDBPlayer;
};

#define sApp Application::getInstance()
#define sStmt sApp.GetMysqlStmt()
#define sNetService sApp.GetNetService()