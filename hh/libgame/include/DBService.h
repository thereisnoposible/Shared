#pragma once

#include "DBConnect.h"
#include "Singleton.h"

class XClass DBService
{
public:
	DBService(int32 num = 1);
	~DBService(void);

public:
	bool Open(const char* host, const char* user, const char* passwd, const char* db,
		unsigned int port = 3306, const char* charset="utf8", unsigned long flag = 0);

	void Update();

	bool AsynQuerySQL(int32 ckey,const xstring& sql,DBCallBack cb,CustParament* userdata);

	bool AsynExcuteSQL(int32 ckey,const xstring& sql,DBCallBack cb,CustParament* userdata);

	bool AsynExcuteProceduce(int32 ckey,const xstring& sql,DBCallBack cb,CustParament* userdata);

	bool AsynExcuteProceduceQuery(int32 ckey,const xstring& sql,DBCallBack cb,CustParament* userdata,const xstring& getresultSql);

	bool SyncQuerySQL(const xstring& sql,MySQLQuery& query);

	int32 SyncExcuteSQL(const xstring& sql);

	bool SyncExcuteProceduce(const xstring& sql);

	bool SyncExcuteProceduceQuery(const xstring& sql,MySQLQuery& query,const xstring& getresultSql);

	MYSQL* GetMySqlConnect();

	MYSQL* GetMySqlConnect(int32 ckey);

protected:
	int32 getThreadID(int32 ckey) { return (std::abs(ckey) % (int32)m_DBConnectCollect.size()) ;}

	void onQueryResult(DBCommandPtr pCommand);

private:
	DBConnectCollect m_DBConnectCollect;			/** ���ݿ����Ӽ��� */
	DBInterface* m_pDBInterface;					/** ͬ�����ݿ�������� */

	DBCommandCollect m_QueryResultCollect;			/** ����������� */
	boost::mutex	m_QueryResultMutex;				/** ����������ϵĻ��Ᵽ�� */


	int32 m_NumOfConnect;
};

