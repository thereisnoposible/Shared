#pragma once

#include "../include/define/define.h"
#include <string>
#include "DBResult.h"
#include <functional>
#include <vector>
#include <thread>
#include <boost/thread/mutex.hpp>
#include <unordered_map>
#include <condition_variable>
#include "DBSqlThread.h"

#pragma warning (push)

#pragma warning (disable: 4251)

namespace xlib
{
#define  NULLFUNC std::function<void(DBResult& result)>()

	enum SqlCommondType
	{
		sql_query,
		sql_exect,
	};

	struct SqlCommond
	{

		std::function<void(DBResult& result)> func;
		std::string sql;
		SqlCommondType type;
		std::shared_ptr<DBResult> result;
		//std::string err;
	};

	class XDLL DBService
	{
	public:
		DBService(int count);
		~DBService();

		void Update();

		bool Open(const char* host, const char* user, const char* passwd, const char* db,
			unsigned int port = 3306, const char* charset = "utf8", unsigned long flag = 0);

		bool syncQuery(const std::string& sql, DBResult& pResult);
		bool syncExcute(const std::string& sql);
		void asynQuery(uint64, const std::string& sql, std::function<void(DBResult& result)> func);
		void asynExcute(uint64, const std::string& sql, std::function<void(DBResult& result)> func);
		const char* getError();

		void OnGetResult(SqlCommond&);

	private:
		MYSQL* mysql;
		boost::mutex mutex;
		std::vector<SqlThread*> vecThread;
		std::vector<SqlCommond> result;
	};
}

#pragma warning (pop)