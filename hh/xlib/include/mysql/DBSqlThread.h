#pragma once
#include <functional>
#include <list>
#include <mutex>
#include <boost/thread/condition_variable.hpp>
#include <thread>

#include "DBResult.h"

namespace xlib
{
	struct SqlCommond;

	struct SqlThread
	{
		SqlThread(std::function<void(SqlCommond&)> _func);
		~SqlThread();

		void close();

		bool Open(const char* host, const char* user, const char* passwd, const char* db,
			unsigned int port = 3306, const char* charset = "utf8", unsigned long flag = 0);

		void ThreadFunction();
		bool syncQuery(std::string& sql, DBResult* pResult);

		bool syncExcute(std::string& sql);

		void addQuerySql(SqlCommond& str);
		void addExcutSql(SqlCommond& str);
		std::function<void(SqlCommond&)> func;

		int ProcessSql(SqlCommond& str);
	private:
		std::thread* pthread;
		std::list<SqlCommond> querysql;
		//std::list<std::string> excutsql;
		boost::mutex mutex;
		boost::condition_variable variable;
		bool bExit;
		MYSQL* mysql;
	};
}
