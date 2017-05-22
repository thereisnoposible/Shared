#pragma once

#include "../include/define/define.h"

#include <unordered_map>

#ifdef ZS_WINOS
#include <windows.h>
#endif 

#ifdef ZS_X64
#include "mysql/mysql64/include/mysql.h"
#include "mysql/mysql64/include/mysql_com.h"
#else
#include "mysql/mysql32/include/mysql.h"
#include "mysql/mysql32/include/mysql_com.h"
#endif 

#pragma comment(lib,"lib/libmysql.lib")

#include "../include/mysql/MysqlStmt.h"
#include "../include/helper/Helper.h"
#include <stdarg.h>
#include <iostream>

#include <boost/regex.hpp>

namespace xlib
{
	StmtBindData::StmtBindData(int count)
	{
		m_datas.resize(count);
		m_bd = new MYSQL_BIND[count];

		memset(m_bd, 0, sizeof(MYSQL_BIND)*count);
	}

	StmtBindData::~StmtBindData()
	{
	}

	void StmtBindData::CopyFrom(StmtBindData& from)
	{
		m_datas = from.m_datas;

		if (m_bd != nullptr)
		{
			delete[] m_bd;
		}

		m_bd = new MYSQL_BIND[m_datas.size()];

		memcpy(m_bd, from.m_bd, sizeof(MYSQL_BIND) * m_datas.size());

		for (int i = 0; i < (int)m_datas.size(); i++)
		{
			m_datas[i].buffer = new char[m_datas[i].length];
			m_bd[i].buffer = m_datas[i].buffer;
			m_bd[i].length = &m_datas[i].length;
		}

		m_Index = from.m_Index;
	}

	void StmtBindData::SetInt32(const std::string& key, int value)
	{
		std::vector<int>* pIndex = GetIndexes(key);
		if (pIndex == NULL)
			return;

		std::vector<int>& indexes = *pIndex;
		for (size_t i = 0; i < indexes.size(); i++)
		{
			SetInt32(indexes[i], value);
		}
	}
	void StmtBindData::SetInt32(int index, int value)
	{
#ifdef _DEBUG
		if (index < 0 || index >= (int)m_datas.size())
			abort();

		if ((m_datas)[index].buffer != nullptr)
			abort();
#endif

		int*p = new int;
		*p = value;

		StmtData temp(p, sizeof(int));
		m_datas[index] = temp;

		m_bd[index].buffer_type = MYSQL_TYPE_LONG;
		m_bd[index].buffer = m_datas[index].buffer;
		m_bd[index].length = &m_datas[index].length;
		m_bd[index].buffer_length = sizeof(int);
		m_bd[index].is_null = (my_bool*)0;
		m_bd[index].is_unsigned = 0;
	}

	void StmtBindData::SetInt64(const std::string& key, long long value)
	{
		std::vector<int>* pIndex = GetIndexes(key);
		if (pIndex == NULL)
			return;

		std::vector<int>& indexes = *pIndex;
		for (size_t i = 0; i < indexes.size(); i++)
		{
			SetInt64(indexes[i], value);
		}
	}
	void StmtBindData::SetInt64(int index, long long value)
	{
#ifdef _DEBUG
		if (index < 0 || index >= (int)m_datas.size())
			abort();

		if (m_datas[index].buffer != nullptr)
			abort();
#endif
		long long*p = new long long;
		*p = value;

		StmtData temp(p, sizeof(long long));
		m_datas[index] = temp;

		m_bd[index].buffer_type = MYSQL_TYPE_LONGLONG;
		m_bd[index].buffer = m_datas[index].buffer;
		m_bd[index].length = &m_datas[index].length;
		m_bd[index].buffer_length = sizeof(long long);
		m_bd[index].is_null = (my_bool*)0;
		m_bd[index].is_unsigned = 0;
	}

	void StmtBindData::SetString(const std::string& key, const char* str, int len)
	{
		std::vector<int>* pIndex = GetIndexes(key);
		if (pIndex == NULL)
			return;

		std::vector<int>& indexes = *pIndex;
		for (size_t i = 0; i < indexes.size(); i++)
		{
			SetString(indexes[i], str, len);
		}
	}

	void StmtBindData::SetString(int index, const char* str, int len)
	{
#ifdef _DEBUG
		if (index < 0 || index >= (int)m_datas.size())
			abort();

		if (m_datas[index].buffer != nullptr)
			abort();
#endif
		char* p = new char[len];

		memcpy(p, str, len);

		StmtData temp(p, len);
		m_datas[index] = temp;

		m_bd[index].buffer_type = MYSQL_TYPE_VAR_STRING;
		m_bd[index].buffer = m_datas[index].buffer;
		m_bd[index].length = &m_datas[index].length;
		m_bd[index].buffer_length = m_datas[index].length;
		m_bd[index].is_null = (my_bool*)0;
		m_bd[index].is_unsigned = 0;
	}

	void StmtBindData::SetString(const std::string& key, const std::string& str)
	{
		std::vector<int>* pIndex = GetIndexes(key);
		if (pIndex == NULL)
			return;

		std::vector<int>& indexes = *pIndex;
		for (size_t i = 0; i < indexes.size(); i++)
		{
			SetString(indexes[i], str);
		}
	}

	void StmtBindData::SetString(int index, const std::string& str)
	{
#ifdef _DEBUG
		if (index < 0 || index >= (int)m_datas.size())
			abort();

		if (m_datas[index].buffer != nullptr)
			abort();
#endif
		char* p = new char[str.length()];
		memcpy(p, str.c_str(), str.length());

		StmtData temp(p, str.length());
		m_datas[index] = temp;

		m_bd[index].buffer_type = MYSQL_TYPE_STRING;
		m_bd[index].buffer = m_datas[index].buffer;
		m_bd[index].length = &m_datas[index].length;
		m_bd[index].buffer_length = m_datas[index].length;
		m_bd[index].is_null = (my_bool*)0;
		m_bd[index].is_unsigned = 0;
	}

	void StmtBindData::SetBlob(const std::string& key, const void* str, int len)
	{
		std::vector<int>* pIndex = GetIndexes(key);
		if (pIndex == NULL)
			return;

		std::vector<int>& indexes = *pIndex;
		for (size_t i = 0; i < indexes.size(); i++)
		{
			SetBlob(indexes[i], str, len);
		}
	}

	void StmtBindData::SetBlob(int index, const void* str, int len)
	{
#ifdef _DEBUG
		if (index < 0 || index >= (int)m_datas.size())
			abort();

		if (m_datas[index].buffer != nullptr)
			abort();
#endif
		char* p = new char[len];

		memcpy(p, str, len);

		StmtData temp(p, len);
		m_datas[index] = temp;

		m_bd[index].buffer_type = MYSQL_TYPE_BLOB;
		m_bd[index].buffer = m_datas[index].buffer;
		m_bd[index].length = &m_datas[index].length;
		m_bd[index].buffer_length = m_datas[index].length;
		m_bd[index].is_null = (my_bool*)0;
		m_bd[index].is_unsigned = 0;
	}

	void StmtBindData::SetDateTime(const std::string& key, int year, int month, int day, int hour, int minute, int second)
	{
		std::vector<int>* pIndex = GetIndexes(key);
		if (pIndex == NULL)
			return;

		std::vector<int>& indexes = *pIndex;
		for (size_t i = 0; i < indexes.size(); i++)
		{
			SetDateTime(indexes[i], year, month, day, hour, minute, second);
		}
	}

	void StmtBindData::SetDateTime(int index, int year, int month, int day, int hour, int minute, int second)
	{
#ifdef _DEBUG
		if (index < 0 || index >= (int)m_datas.size())
			abort();

		if (m_datas[index].buffer != nullptr)
			abort();
#endif
		MYSQL_TIME* p = new MYSQL_TIME;
		p->year = year;
		p->month = month;
		p->day = day;
		p->hour = hour;
		p->minute = minute;
		p->second = second;
		p->second_part = 0;
		p->neg = false;
		p->time_type = MYSQL_TIMESTAMP_DATETIME;

		StmtData temp(p, sizeof(MYSQL_TIME));
		m_datas[index] = temp;

		m_bd[index].buffer_type = MYSQL_TYPE_DATETIME;
		m_bd[index].buffer = m_datas[index].buffer;
		m_bd[index].length = &m_datas[index].length;
		m_bd[index].buffer_length = m_datas[index].length;
		m_bd[index].is_null = (my_bool*)0;
		m_bd[index].is_unsigned = 0;
	}

	void StmtBindData::free()
	{
		if (m_bd)
		{
			delete[] m_bd;
			m_bd = nullptr;
		}

		for (int i = 0; i < (int)m_datas.size(); i++)
		{
			delete m_datas[i].buffer;
		}

		m_datas.clear();
	}

	MYSQL_BIND* StmtBindData::GetBind()
	{
		return m_bd;
	}

	std::vector<int>* StmtBindData::GetIndexes(const std::string& key)
	{
		std::unordered_map<std::string, std::vector<int>>::iterator it = m_Index.find(key);
#ifdef _DEBUG
		if (it == m_Index.end())
			abort();
#endif
		if (it == m_Index.end())
			return NULL;

		std::vector<int>* pItem = &it->second;
		return pItem;
	}

	void StmtExData::free()
	{
		if (param != nullptr)
		{
			param->free();
			delete param;
			param = nullptr;
		}
	}

	bool StmtExData::GetResult()
	{
		return result.bResult;
	}

	bool StmtExData::eof()
	{
		return ret != 0;
	}

	void StmtExData::nextRow()
	{
		ret = stmt->FormatResult(*this, m_Result);
	}

	void StmtExData::call_back(Stmt* st, bool res, StmtBindData* sbd)
	{
		stmt = st;
		result.bResult = res;
		m_Result = sbd;
		ret = -1;
		if (res == true)
			ret = stmt->FormatResult(*this, m_Result);

		if (cb != nullptr)
		{
			cb(this);
		}
	}

	const char* StmtExData::geterror()
	{
		return mysql_stmt_error(stmt->stmt);
	}

	StmtData* StmtExData::GetData(int index)
	{
		return result.GetData(index);
	}

	StmtData* StmtExData::GetData(const std::string& name)
	{
		return result.GetData(name);
	}

	int StmtExData::GetInt32(int index)
	{
		return *(int*)GetData(index)->buffer;
	}

	int StmtExData::GetInt32(const std::string& name)
	{
		StmtData* p = GetData(name);
		if (!p)
			return 0;
		return *(int*)p->buffer;
	}

	long long StmtExData::GetInt64(int index)
	{
		return *(long long*)GetData(index)->buffer;
	}

	long long StmtExData::GetInt64(const std::string& name)
	{
		StmtData* p = GetData(name);
		if (!p)
			return 0;
		return *(long long*)p->buffer;
	}

	char* StmtExData::GetString(int index)
	{
		return (char*)GetData(index)->buffer;
	}

	char* StmtExData::GetString(const std::string& name)
	{
		StmtData* p = GetData(name);
		if (!p)
			return "";
		return (char*)p->buffer;
	}

	time_t StmtExData::GetDateTime(int index)
	{
		MYSQL_TIME* p = (MYSQL_TIME*)GetData(index)->buffer;
		if (p->year < 1970)
			return 0;
		if (p->month <= 0)
			return 0;
		if (p->day <= 0)
			return 0;
		if (p->hour < 0)
			return 0;
		if (p->minute < 0)
			return 0;
		if (p->second < 0)
			return 0;
		if (p->year == 1970 && p->month == 1 && p->day == 1 && p->hour < 8)
			return 0;

		tm tmTime;
		memset(&tmTime, 0, sizeof(tm));
		tmTime.tm_year = p->year;
		tmTime.tm_mon = p->month;
		tmTime.tm_yday = p->day;
		tmTime.tm_hour = p->hour;
		tmTime.tm_min = p->minute;
		tmTime.tm_sec = p->second;

		time_t tTime = mktime(&tmTime);
		return tTime;
	}

	time_t StmtExData::GetDateTime(const std::string& name)
	{
		StmtData* pp = GetData(name);
		if (!pp)
			return 0;

		MYSQL_TIME* p = (MYSQL_TIME*)pp->buffer;
		if (p->year < 1970)
			return 0;
		if (p->month <= 0)
			return 0;
		if (p->day <= 0)
			return 0;
		if (p->hour < 0)
			return 0;
		if (p->minute < 0)
			return 0;
		if (p->second < 0)
			return 0;
		if (p->year == 1970 && p->month == 1 && p->day == 1 && p->hour < 8)
			return 0;

		tm tmTime;
		memset(&tmTime, 0, sizeof(tm));
		tmTime.tm_year = p->year;
		tmTime.tm_mon = p->month;
		tmTime.tm_yday = p->day;
		tmTime.tm_hour = p->hour;
		tmTime.tm_min = p->minute;
		tmTime.tm_sec = p->second;

		time_t tTime = mktime(&tmTime);
		return tTime;
	}

	int StmtExData::GetSize(int index)
	{
		return GetData(index)->length;
	}

	int StmtExData::GetSize(const std::string& name)
	{
		StmtData* pp = GetData(name);
		if (!pp)
			return 0;
		return pp->length;
	}

	StmtSyncResult::~StmtSyncResult()
	{
		if (res_param != nullptr)
		{
			pResult.free();
			res_param->free();
			delete res_param;

			if (stmt)
			{
				mysql_stmt_free_result(stmt);
				stmt = nullptr;
			}
		}
	}

	bool MysqlStmt::Open(const char* host, const char* user, const char* passwd, const char* db,
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

		return true;
	}

	void MysqlStmt::Close()
	{
		auto it = stmts.begin();
		for (; it != stmts.end(); ++it)
		{
			if (it->second.m_Result)
			{
				it->second.m_Result->free();
				delete it->second.m_Result;
				it->second.m_Result = nullptr;
			}

			auto itt = it->second.CircularQuerys.begin();
			for (; itt != it->second.CircularQuerys.end(); ++it)
			{
				(*itt)->free();
				delete *itt;
			}
			it->second.CircularQuerys.clear();

			mysql_stmt_close(it->second.stmt);
		}
		stmts.clear();

		if (mysql)
		{
			mysql_close(mysql);
			mysql = NULL;
		}
	}

	StmtBindData* MysqlStmt::PrepareQuery(const char* pSql)
	{
		Stmt temp(100);

		std::string m_sql(pSql);

		auto sqit = stmts.find(m_sql);
		if (sqit != stmts.end())
		{
			StmtBindData* pItem = sqit->second.GetNewStmtParam();
			return pItem;
		}

		std::string temp_sql = AnalyzeSql(&temp, m_sql);

		temp.stmt = mysql_stmt_init(mysql);
		//my_bool true_value = 1;
		//mysql_stmt_attr_set(temp.stmt, STMT_ATTR_UPDATE_MAX_LENGTH, (void*)&true_value);

		int rt = mysql_stmt_prepare(temp.stmt, temp_sql.c_str(), temp_sql.size());
		if (0 != rt)
		{
			mysql_stmt_close(temp.stmt);
			return NULL;
		}

		temp.m_numIn = mysql_stmt_param_count(temp.stmt);

		MYSQL_RES* ret = mysql_stmt_result_metadata(temp.stmt);
		if (ret)
		{
			int count = mysql_num_fields(ret);
			temp.m_Result = nullptr;
			//输出参数
			if (count > 0)
			{
				temp.m_Result = new StmtBindData(count);

				for (int i = 0; i < (int)temp.stmt->field_count; i++)
				{
					switch (temp.stmt->fields[i].type)
					{
					case MYSQL_TYPE_LONG:
						temp.m_Result->SetInt32(i, 0);
						break;
					case MYSQL_TYPE_DATETIME:
						temp.m_Result->SetDateTime(i, 0, 0, 0, 0, 0, 0);
						break;
					case MYSQL_TYPE_STRING:
					case MYSQL_TYPE_VAR_STRING:
						temp.m_Result->SetString(i, std::string(temp.stmt->fields[i].length, 0));
						break;
					case MYSQL_TYPE_BLOB:
					{
						std::string str(temp.stmt->fields[i].length, 0);
						temp.m_Result->SetBlob(i, str.c_str(), str.length());
					}
					break;
					case MYSQL_TYPE_LONGLONG:
						temp.m_Result->SetInt64(i, 0);
						break;
					default:
						std::cout << "unknown type" << temp.stmt->fields[i].type << "\n";
						mysql_stmt_close(temp.stmt);
						return NULL;
					}

					temp.m_Result->m_datas[i].name = temp.stmt->fields[i].name;
				}
			}
		}

		stmts.insert(std::make_pair(m_sql, temp));
		std::unordered_map<std::string, Stmt>::iterator iooo = stmts.find(m_sql);
		return iooo->second.GetNewStmtParam();
	}

	void MysqlStmt::AsynExecuteQueryVariable(std::string& sql, StmtBindData* param, std::function<void(StmtExData*)> cb)
	{
		auto sqit = stmts.find(sql);
		if (sqit == stmts.end())
		{
			PrepareQuery(sql.c_str());
			sqit = stmts.find(sql);

			if (sqit == stmts.end())
			{
				std::cout << "PrepareQuery failed :" << sql << "\n";
				return;
			}
		}

		sqit->second.AsynExecuteQueryVariable(param, cb);

	}
	void MysqlStmt::SyncExecuteQueryVariable(std::string& sql, StmtBindData* param, StmtSyncResult& res)
	{
		auto sqit = stmts.find(sql);
		if (sqit == stmts.end())
		{
			PrepareQuery(sql.c_str());
			sqit = stmts.find(sql);

			if (sqit == stmts.end())
			{
				std::cout << "PrepareQuery failed :" << sql << "\n";
				return;
			}
		}

		sqit->second.SyncExecuteQueryVariable(param, res);
	}

	std::string MysqlStmt::AnalyzeSql(Stmt* stmt, std::string& sql)
	{
		std::string temp_sql = sql;

		const char* str = temp_sql.c_str();
		int pos = Helper::FindString(str, temp_sql.size(), "?");

		std::vector<std::string> finder;
		finder.push_back(",");
		std::string tttt;
		tttt.assign(1, '\0');
		finder.push_back(tttt);
		finder.push_back(" ");
		finder.push_back(")");

		int index = -1;
		while (pos != -1)
		{
			index++;
			int len = strlen(str + pos + 1) + 1;

			size_t out_size = 0;
			size_t s_pos = Helper::FindString(str + pos + 1, len, finder, out_size);
			
			if (s_pos != -1)
			{
				std::string ss;
				ss.assign(str + pos + 1, s_pos);

				if (ss.empty())
					abort();

				temp_sql.erase(pos + 1, s_pos);
				stmt->m_Index[ss].push_back(index);
			}

			size_t t_pos = Helper::FindString(str + pos + 1, strlen(str + pos + 1), "?");
			if (t_pos == -1)
				break;
			pos += t_pos + 1;
		}

		return temp_sql;
	}

	bool MysqlStmt::Update()
	{
		bool bhang = true;

		auto it = stmts.begin();
		for (; it != stmts.end(); ++it)
		{
			if (it->second.update())
			{
				bhang = false;
			}
		}

		return bhang;
	}


	void Stmt::AsynExecuteQueryVariable(StmtBindData* param, std::function<void(StmtExData*)> cb)
	{
		StmtExData* temp = new StmtExData;
		temp->param = param;
		temp->cb = cb;

		if (CircularQuerys.size() == 1 && CircularQuerys.front() == nullptr)
		{
			CircularQuerys.front() = temp;
			return;
		}

		CircularQuerys.push_back(temp);

		//querys.push_back(temp);
	}

	void Stmt::SyncExecuteQueryVariable(StmtBindData* param, StmtSyncResult& res)
	{
		if (m_numIn > 0)
		{
			res.pResult.param = param;
		}

		if (m_Result != nullptr)
		{
			res.res_param = new StmtBindData(0);
			res.res_param->CopyFrom(*m_Result);
		}

		int ret = 0;
		if (res.pResult.param)
			ret = mysql_stmt_bind_param(stmt, res.pResult.param->GetBind());
		if (ret != 0)
		{
			res.pResult.result.bResult = false;
			return;
		}

		ret = mysql_stmt_execute(stmt);

		if (ret != 0)
		{
			std::cout << mysql_stmt_error(stmt);
			res.pResult.result.bResult = false;
			return;
		}

		if (res.res_param->m_datas.size() > 0)
		{
			ret = mysql_stmt_bind_result(stmt, res.res_param->GetBind());

			if (ret != 0)
			{
				std::cout << mysql_stmt_error(stmt);
				res.pResult.result.bResult = false;
				return;
			}

			ret = mysql_stmt_store_result(stmt);

			if (ret != 0)
			{
				return;

			}

			res.pResult.call_back(this, true, res.res_param);
		}

		res.pResult.result.bResult = true;
	}

	bool Stmt::update()
	{
		if (CircularQuerys.size() == 0)
			return false;

		StmtExData* pFront = CircularQuerys.front();
		if (CircularQuerys.size() == 1 && pFront == nullptr)
		{
			return false;
		}

		if (pFront != nullptr)
		{
			Execute(*pFront, m_Result);
			pFront->free();
			delete pFront;
		}

		CircularQuerys.front() = nullptr;		
		if (CircularQuerys.size() > 1)
		{
			CircularQuerys.pop_front();
		}

		return true;
	}

	void Stmt::Execute(StmtExData& row, StmtBindData* res)
	{
		int ret = 0;
		if (row.param)
			ret = mysql_stmt_bind_param(stmt, row.param->GetBind());
		if (ret != 0)
		{
			row.call_back(this, false, nullptr);
			return;
		}

		ret = mysql_stmt_execute(stmt);

		if (ret != 0)
		{
			row.call_back(this, false, nullptr);

			std::cout << mysql_stmt_error(stmt);
			return;
		}

		if (res && res->m_datas.size() > 0)
		{
			ret = mysql_stmt_bind_result(stmt, res->GetBind());

			if (ret != 0)
			{
				row.call_back(this, false, nullptr);
				std::cout << mysql_stmt_error(stmt);
				return;
			}

			ret = mysql_stmt_store_result(stmt);

			if (ret == 0)
			{
				row.call_back(this, true, res);
			}

			mysql_stmt_free_result(stmt);
		}

		return;
	}

	int Stmt::FormatResult(StmtExData& data, StmtBindData* result)
	{
		int ret = mysql_stmt_fetch(stmt);
		if (0 != ret)
		{
			return ret;
		}

		StmtResult& temp = data.result;
		temp.clear();

		for (int i = 0; i < (int)result->m_datas.size(); ++i)
		{
			temp.reference_data(result->m_datas[i].name, result->m_datas[i].buffer, result->m_datas[i].length);
		}

		return ret;
	}
}