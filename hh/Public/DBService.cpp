#include "DBService.h"
#include "DBSqlThread.h"

DBService::DBService(int count)
{
	for (int i = 0; i < count; ++i)
	{
		SqlThread* sql = new SqlThread(std::bind(&DBService::OnGetResult, this, std::placeholders::_1));
		vecThread.push_back(sql);
	}
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
	std::unique_lock<std::mutex> oLock(mutex);
	result.push_back(cmd);
}

void DBService::Update()
{
	std::unique_lock<std::mutex> oLock(mutex);
	std::vector<SqlCommond>&& temp = std::move(result);
	result.clear();
	mutex.unlock();

	for (int i = 0; i < (int)temp.size(); ++i)
	{
		temp[i].func(temp[i].result);
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

void DBService::asynQuery(unsigned int key, const std::string& sql, std::function<void(DBResult& result)> func)
{
	SqlCommond cmd;
	cmd.sql = sql;
	cmd.func = func;
	cmd.type = sql_query;
	vecThread[key%vecThread.size()]->addQuerySql(cmd);
}

void DBService::asynExcute(unsigned int key, const std::string& sql, std::function<void(DBResult& result)> func)
{
	SqlCommond cmd;
	cmd.sql = sql;
	cmd.func = func;
	cmd.type = sql_exect;
	vecThread[key%vecThread.size()]->addExcutSql(cmd);
}

const char* DBService::getError()
{
	return mysql_error(mysql);
}