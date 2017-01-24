#include "stdafx.h"
#include "DBPlayer.h"
#include "Application.h"

DBPlayer::DBPlayer()
{
    Init();
    registerMessage();
}

DBPlayer::~DBPlayer()
{

}


void DBPlayer::Init()
{

}

void DBPlayer::registerMessage()
{
    sNetService.RegisterMessage(GM_INSERT_PLAYER, std::bind(&DBPlayer::processInsert, this, std::placeholders::_1));
    sNetService.RegisterMessage(GM_UPDATE_PLAYER, std::bind(&DBPlayer::processUpdate, this, std::placeholders::_1));
    sNetService.RegisterMessage(GM_REQUEST_PLAYER, std::bind(&DBPlayer::processRequest, this, std::placeholders::_1));
}

void DBPlayer::processInsert(PackPtr& pPack)
{
    pm_playerdata notify;
    CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
    
    std::string sql = "insert into player(account,id,jinbi,name) values(?account,?id,?jinbi,?name)";

    StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());
    pBind->SetString("account", notify.account());
    pBind->SetInt32("id", notify.id());
    pBind->SetInt32("jinbi", notify.jinbi());
    pBind->SetString("name", notify.name());

    sStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);
}
void DBPlayer::processUpdate(PackPtr& pPack)
{
    pm_playerdata notify;
    CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

    std::string sql = "update player set jinbi=?jinbi,name=?name";
    StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());
    pBind->SetInt32("jinbi", notify.jinbi());
    pBind->SetString("name", notify.name());

    sStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);
}
void DBPlayer::processRequest(PackPtr& pPack)
{
    std::string sql = "select * from player";

    sStmt.AsynExecuteQueryVariable(sql, nullptr, std::bind(&DBPlayer::cbRequest, this, std::placeholders::_1, pPack->m_Connect));
}

void DBPlayer::cbRequest(StmtExData* result, ConnectPtr& conn)
{
    if (!result->GetResult())
    {
        std::cout << result->geterror() << "\n";
    }

    pm_playerdata_db_response response;
    response.set_result(0);

    while (!result->eof())
    {
        pm_playerdata* pdate = response.add_datas();
        pdate->set_id(result->GetInt32("id"));
        pdate->set_jinbi(result->GetInt32("jinbi"));
        pdate->set_name(result->GetString("name"));
        pdate->set_account(result->GetString("account"));

        result->nextRow();
    }

    conn->SendBuffer(GM_REQUEST_PLAYER_RESPONSE, response, 0);
}
