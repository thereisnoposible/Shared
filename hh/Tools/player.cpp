#include "stdafx.h"
#include "Application.h"
#include "DBPlayer.h"

DBPlayer::DBPlayer()
{
Init();
registerMessage();
}
DBPlayer::~DBPlayer()
{

}
DBPlayer::Init()
{

}
DBPlayer::registerMessage()
{
	sNetService.RegisterMessage(GM_NOTIFY_INSERT_MAOXIAN, std::bind(&DBPlayer::processInsert, this, std::placeholders::_1));
	sNetService.RegisterMessage(GM_NOTIFY_UPDATE_MAOXIAN, std::bind(&DBPlayer::processUpdate, this, std::placeholders::_1));
	sNetService.RegisterMessage(GM_NOTIFY_DELETE_MAOXIAN, std::bind(&DBPlayer::processDelete, this, std::placeholders::_1));
	sNetService.RegisterMessage(GM_REQUEST_MAOXIAN_INFO, std::bind(&DBPlayer::processRequest, this, std::placeholders::_1));
}
void DBPlayer::processInsert(PackPtr& pPack)
{
	pm_playerdata notify;
	CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	std::string sql = "insert into player(id,jinbi,name,account) values(?id,?jinbi,?name,?account)";

	StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());
	pBind->SetInt32("id",notify.id());
	pBind->SetInt32("jinbi",notify.jinbi());
	pBind->SetString("name",notify.name());
	pBind->SetString("account",notify.account());
	sStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);
}

void DBPlayer::processUpdate(PackPtr& pPack)
{
	pm_playerdata notify;
	CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	std::string sql = "update player set jinbi=?jinbi,name=?name,account=?account";
	StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());
	pBind->SetInt32("id",notify.id());
	pBind->SetInt32("jinbi",notify.jinbi());
	pBind->SetString("name",notify.name());
	pBind->SetString("account",notify.account());
	sStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);
}

void DBPlayer::processRequest(PackPtr& pPack)
{
	pm_request_maoxian_info request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	std::string sql = "select * from player where playerid = ?playerid";
	StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());
	pBind->SetInt32("playerid",request.playerid());
	sStmt.AsynExecuteQueryVariable(sql, nullptr, std::bind(&DBPlayer::cbRequest, this, std::placeholders::_1, pPack->m_Connect));
}

void DBPlayer::cbRequest(StmtExData* result, ConnectPtr& conn)
{
	if (!result->GetResult())
	{
		std::cout << result->geterror() << "\n;"
	}

	pm_playerdata_db_response response;
	response.set_result(0);

	while (!result->eof())
	{
		pm_playerdata* pdata = response.add_datas();
		pdate->set_id(result->GetInt32("id"));
		pdate->set_jinbi(result->GetInt32("jinbi"));
		pdate->set_name(result->GetString("name"));
		pdate->set_account(result->GetString("account"));
		result->nextRow();
	}

	conn->SendBuffer(GM_MAOXIAN_INFO_RESPONSE, response, 0);
}

