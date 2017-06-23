#include "../include/mysql/DBService.h"
#include "../include/mysql/DBSqlThread.h"

#include "../include/define/define.h"

#ifdef ZS_WINOS
#include <windows.h>
#endif 

#ifdef _WIN64
#include "mysql/mysql64/include/mysql.h"
#include "mysql/mysql64/include/mysql_com.h"
#else
#include "mysql/mysql32/include/mysql.h"
#include "mysql/mysql32/include/mysql_com.h"
#endif 

namespace xlib
{
	DBService::DBService(int count)
	{
		for (int i = 0; i < count; ++i)
		{
			SqlThread* sql = new SqlThread(std::bind(&DBService::OnGetResult, this, std::placeholders::_1));
			vecThread.push_back(sql);
		}

		mysql = nullptr;
	}

	DBService::~DBService()
	{
		for (int i = 0; i < (int)vecThread.size(); ++i)
		{
			delete vecThread[i];
		}

		if (mysql != nullptr)
			mysql_close(mysql);

	}
	void DBService::OnGetResult(SqlCommond& cmd)
	{
		boost::mutex::scoped_lock oLock(mutex);
		result.push_back(cmd);
		oLock.unlock();
	}

	void DBService::Update()
	{
		boost::mutex::scoped_lock oLock(mutex);
		std::vector<SqlCommond> temp = result;
		result.clear();
		oLock.unlock();

		for (int i = 0; i < (int)temp.size(); ++i)
		{
			if (temp[i].func)
				temp[i].func(*temp[i].result);
		}
	}

	bool DBService::Open(const char* host, const char* user, const char* passwd, const char* db,
		unsigned int port /* = 3306 */, const char* charset /* = "utf8" */, unsigned long flag /* = 0 */)
	{
		mysql = mysql_init(NULL);
		if (nullptr == mysql)
			return false;

		//支持中文处理
		mysql_options(mysql, MYSQL_SET_CHARSET_NAME, charset);

		///如果断线则重新在连接
		mysql_options(mysql, MYSQL_OPT_RECONNECT, "1");

		if (mysql_real_connect(mysql, host, user, passwd, db, port, nullptr, flag) == nullptr)
			return false;

		//选择制定的数据库失败
		//0表示成功，非0值表示出现错误。
		if (mysql_select_db(mysql, db) != 0)
		{
			mysql_close(mysql);
			mysql = NULL;
			return false;
		}

		for (int i = 0; i < (int)vecThread.size(); ++i)
		{
			vecThread[i]->Open(host, user, passwd, db, port, charset, flag);
		}

		return true;
	}

	bool DBService::syncQuery(const std::string& sql, DBResult& pResult)
	{
		int result = mysql_real_query(mysql, sql.c_str(), sql.size());
		if (result != 0)
		{
			return false;
		}

		pResult.GetResult(mysql_store_result(mysql));

		return pResult.pResult != nullptr;
	}

	bool DBService::syncExcute(const std::string& sql)
	{
		return mysql_real_query(mysql, sql.c_str(), sql.length()) == 0;
	}

	void DBService::asynQuery(uint64 key, const std::string& sql, std::function<void(DBResult& result)> func)
	{
		SqlCommond cmd;
		cmd.sql = sql;
		cmd.func = func;
		cmd.type = sql_query;
		cmd.result = std::shared_ptr<DBResult>(new DBResult);
		vecThread[key%vecThread.size()]->addQuerySql(cmd);
	}

	void DBService::asynExcute(uint64 key, const std::string& sql, std::function<void(DBResult& result)> func)
	{
		SqlCommond cmd;
		cmd.sql = sql;
		cmd.func = func;
		cmd.type = sql_exect;
		cmd.result = std::shared_ptr<DBResult>(new DBResult);
		vecThread[key%vecThread.size()]->addExcutSql(cmd);
	}

	const char* DBService::getError()
	{
		return mysql_error(mysql);
	}
}

