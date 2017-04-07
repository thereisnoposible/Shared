#include "stdafx.h"
#include "Application.h"
#include "DBObject.h"

DBObject::DBObject()
{
	registerMessage();
}
DBObject::~DBObject()
{

}
void DBObject::registerMessage()
{
	sNetService.RegisterMessage(GM_INSERT_OBJECT, std::bind(&DBObject::processInsert, this, std::placeholders::_1));
	sNetService.RegisterMessage(GM_UPDATE_OBJECT, std::bind(&DBObject::processUpdate, this, std::placeholders::_1));
	sNetService.RegisterMessage(GM_DELETE_OBJECT, std::bind(&DBObject::processDelete, this, std::placeholders::_1));
	sNetService.RegisterMessage(GM_OBJECT_DATA_DB, std::bind(&DBObject::processRequest, this, std::placeholders::_1));
}
void DBObject::processInsert(PackPtr& pPack)
{
	pm_object notify;
	CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	std::string sql = "insert into object(dbid,playerid,prop_id,prop_num) values(?dbid,?playerid,?prop_id,?prop_num)";

	StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());
	pBind->SetInt64("dbid",notify.dbid());
	pBind->SetInt32("playerid",notify.playerid());
	pBind->SetInt32("prop_id",notify.prop_id());
	pBind->SetInt32("prop_num",notify.prop_num());
	sStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);
}

void DBObject::processUpdate(PackPtr& pPack)
{
	pm_object notify;
	CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	std::string sql = "update object set playerid=?playerid,prop_id=?prop_id,prop_num=?prop_num";
	StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());
	pBind->SetInt64("dbid",notify.dbid());
	pBind->SetInt32("playerid",notify.playerid());
	pBind->SetInt32("prop_id",notify.prop_id());
	pBind->SetInt32("prop_num",notify.prop_num());
	sStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);
}

void DBObject::processRequest(PackPtr& pPack)
{
	pm_object_data_db request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	std::string sql = "select * from object where playerid = ?playerid";
	StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());
	pBind->SetInt32("playerid",request.playerid());
	sStmt.AsynExecuteQueryVariable(sql, nullptr, std::bind(&DBObject::cbRequest, this, std::placeholders::_1, pPack->m_Connect, request.playerid()));
}

void DBObject::processDelete(PackPtr& pPack)
{
	pm_object notify;
	CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	std::string sql = "delete from object where dbid = ?dbid";
	StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());
	pBind->SetInt64("dbid",notify.dbid());
	sStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);
}

void DBObject::cbRequest(StmtExData* result, ConnectPtr& conn, int32 playerid)
{
	if (!result->GetResult())
	{
		std::cout << result->geterror() << "\n";
		return;
	}

	pm_object_data_response_db response;
	response.set_result(0);
	response.set_playerid(playerid);

	while (!result->eof())
	{
		pm_object* pdata = response.add_datas();
		pdata->set_dbid(result->GetInt64("dbid"));
		pdata->set_playerid(result->GetInt32("playerid"));
		pdata->set_prop_id(result->GetInt32("prop_id"));
		pdata->set_prop_num(result->GetInt32("prop_num"));
		result->nextRow();
	}

	conn->SendBuffer(GM_OBJECT_DATA_RESPONSE_DB, response, 0);
}

