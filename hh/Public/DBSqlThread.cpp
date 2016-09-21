#include "DBSqlThread.h"
#include "DBService.h"
#include <iostream>

SqlThread::SqlThread(std::function<void(SqlCommond&)> _func) :bExit(false), mysql(nullptr)
{
	func = _func;
	pthread = new std::thread(&SqlThread::ThreadFunction, this);
}
SqlThread::~SqlThread()
{
	close();
	pthread->join();

	delete pthread;
	pthread = nullptr;

	if (mysql != nullptr)
	{
		mysql_close(mysql);
		mysql = nullptr;
	}
}

void SqlThread::close()
{
	bExit = true;
	variable.notify_all();
}

bool SqlThread::Open(const char* host, const char* user, const char* passwd, const char* db,
	unsigned int port, const char* charset, unsigned long flag)
{
	if (mysql != nullptr)
	{
		mysql_close(mysql);
		mysql = nullptr;
	}

	mysql = mysql_init(NULL);
	if (nullptr == mysql)
		return false;

	////支持中文处理
	//mysql_options(mysql, MYSQL_SET_CHARSET_NAME, charset);

	/////如果断线则重新在连接
	//mysql_options(mysql, MYSQL_OPT_RECONNECT, "1");

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

	return true;
}
void SqlThread::ThreadFunction()
{
	while (!bExit)
	{
		std::unique_lock<std::mutex> oLock(mutex);
		if (querysql.size() == 0)
		{
			variable.wait(oLock);
			continue;
		}
		SqlCommond commd = (querysql.front());
		querysql.pop_front();
		oLock.unlock();

		int res = ProcessSql(commd);
		switch (res)
		{
		case 1:
			std::cout << mysql_error(mysql) << "\n";
			break;
		case 0:
			if (!commd.func._Empty())
				commd.func(commd.result);
		case -1:
			break;
		default:
			break;
		}
	}	

	std::list<SqlCommond>::iterator it = querysql.begin();
	for (; it != querysql.end(); ++it)
	{
		int res = ProcessSql(*it);
		switch (res)
		{
		case 1:
			std::cout << "error :" << mysql_error(mysql);
			break;
		case 0:
			if (!(*it).func._Empty())
				(*it).func((*it).result);
		case -1:
			break;
		default:
			break;
		}
	}
}
bool SqlThread::syncQuery(std::string& sql, DBResult* pResult)
{
	int result = mysql_real_query(mysql, sql.c_str(), sql.size());
	if (result != 0)
	{
		return false;
	}

	pResult->GetResult(mysql_store_result(mysql));

	return pResult->pResult != nullptr;
}

bool SqlThread::syncExcute(std::string& sql)
{
	return mysql_real_query(mysql, sql.c_str(), sql.length()) == 0;
}

int SqlThread::ProcessSql(SqlCommond& str)
{
	int res = -1;
	switch (str.type)
	{
	case sql_query:
		if (syncQuery(str.sql, &str.result))
			res = 0;
		else
			res = 1;
		break;
	case sql_exect:
		if (syncExcute(str.sql))
			res = 0;
		else
			res = 1;
		break;
	}

	return res;
}

void SqlThread::addQuerySql(SqlCommond& str)
{
	std::unique_lock<std::mutex> oLock(mutex);
	querysql.push_back(str);
	oLock.unlock();
	variable.notify_all();
}
void SqlThread::addExcutSql(SqlCommond& str)
{
	std::unique_lock<std::mutex> oLock(mutex);
	querysql.push_back(str);
	oLock.unlock();
	variable.notify_all();
}