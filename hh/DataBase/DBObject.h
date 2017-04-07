#pragma once

class MysqlStmt;
struct Stmt;
struct StmtExData;

class DBObject
{
public:
	DBObject();
	~DBObject();
	void registerMessage();
private:
	void processInsert(PackPtr& pPack);
	void processUpdate(PackPtr& pPack);
	void processDelete(PackPtr& pPack);
	void processRequest(PackPtr& pPack);
	void cbRequest(StmtExData* result, ConnectPtr& conn, int32 playerid);
};
