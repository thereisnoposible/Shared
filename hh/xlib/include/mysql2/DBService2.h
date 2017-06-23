#pragma once

#include "../include/define/define.h"
#include "DBConnect.h"
#include "../log/LogService.h"
#pragma warning (push)

#pragma warning (disable: 4251)
namespace xlib
{
	class XDLL DBService2
	{
	public:
		DBService2(int32 num = 1);
		~DBService2(void);

	public:
		void SetLogService(LogService2* p);
		bool Open(const char* host, const char* user, const char* passwd, const char* db,
			unsigned int port = 3306, const char* charset = "utf8", unsigned long flag = 0);

		void Update();

		bool AsynQuerySQL(int32 ckey, const std::string& sql, DBCallBack cb, CustParament* userdata);

		bool AsynExcuteSQL(int32 ckey, const std::string& sql, DBCallBack cb, CustParament* userdata);

		bool AsynExcuteProceduce(int32 ckey, const std::string& sql, DBCallBack cb, CustParament* userdata);

		bool AsynExcuteProceduceQuery(int32 ckey, const std::string& sql, DBCallBack cb, CustParament* userdata, const std::string& getresultSql);

		bool SyncQuerySQL(const std::string& sql, MySQLQuery& query);

		int32 SyncExcuteSQL(const std::string& sql);

		bool SyncExcuteProceduce(const std::string& sql);

		bool SyncExcuteProceduceQuery(const std::string& sql, MySQLQuery& query, const std::string& getresultSql);

		MYSQL* GetMySqlConnect();

		MYSQL* GetMySqlConnect(int32 ckey);

	protected:
		int32 getThreadID(int32 ckey) { return (std::abs(ckey) % (int32)m_DBConnectCollect.size()); }

		void onQueryResult(DBCommandPtr pCommand);

	private:
		DBConnectCollect m_DBConnectCollect;			/** 数据库连接集合 */
		DBInterface* m_pDBInterface;					/** 同步数据库操作对象 */

		DBCommandCollect m_QueryResultCollect;			/** 操作结果集合 */
		boost::mutex	m_QueryResultMutex;				/** 操作结果集合的互斥保护 */


		int32 m_NumOfConnect;

		LogService2* m_pLogService2;
	};

}

#pragma warning (pop)