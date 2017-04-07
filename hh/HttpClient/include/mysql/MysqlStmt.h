#pragma once
#include <functional>
#include <unordered_map>
#include <assert.h>
#include <thread>
#include <condition_variable>

namespace xlib
{
	struct StmtData
	{
		StmtData()
		{
			buffer = nullptr;
			length = 0;
		}

		StmtData(void*d, int l)
		{
			buffer = d;
			length = l;
		}

		std::string name;
		void* buffer;
		unsigned long length;
	};

	struct StmtExData;

	struct StmtBindData
	{
		StmtBindData(int count)
		{
			m_datas.resize(count);
			m_bd = new MYSQL_BIND[count];

			memset(m_bd, 0, sizeof(MYSQL_BIND)*count);
		}

		void CopyFrom(StmtBindData& from)
		{
			m_datas = from.m_datas;

			if (m_bd != nullptr)
			{
				delete[] m_bd;
			}

			m_bd = new MYSQL_BIND[m_datas.size()];

			memcpy(m_bd, from.m_bd, sizeof(MYSQL_BIND)*m_datas.size());

			for (int i = 0; i < (int)m_datas.size(); i++)
			{
				m_datas[i].buffer = new char[m_datas[i].length];
				m_bd[i].buffer = m_datas[i].buffer;
				m_bd[i].length = &m_datas[i].length;
			}

			m_Index = from.m_Index;
		}

		void SetInt32(const std::string& key, int value)
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
		void SetInt32(int index, int value)
		{
#ifdef _DEBUG
			if (index < 0 || index >= (int)m_datas.size())
				abort();

			if (m_datas[index].buffer != nullptr)
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

		void SetInt64(const std::string& key, long long value)
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
		void SetInt64(int index, long long value)
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

		void SetString(const std::string& key, const char* str, int len)
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

		void SetString(int index, const char* str, int len)
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

		void SetString(const std::string& key, const std::string& str)
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

		void SetString(int index, const std::string& str)
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

		void SetBlob(const std::string& key, const void* str, int len)
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

		void SetBlob(int index, const void* str, int len)
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

		void SetDateTime(const std::string& key, int year, int month, int day, int hour, int minute, int second)
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

		void SetDateTime(int index, int year, int month, int day, int hour, int minute, int second)
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

		void free()
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

		MYSQL_BIND* GetBind()
		{
			return m_bd;
		}

		std::unordered_map<std::string, std::vector<int>> m_Index;
		std::vector<StmtData> m_datas;
	private:
		std::vector<int>* GetIndexes(const std::string& key)
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

		MYSQL_BIND* m_bd;
	};

	struct StmtResult
	{
		void reference_data(std::string& name, void* data, int length)
		{
			StmtData temp(data, length);
			map_datas[name] = temp;
			vec_datas.push_back(temp);
		}

		void clear()
		{
			vec_datas.clear();
			map_datas.clear();
		}

		StmtData* GetData(int index)
		{
			return &vec_datas[index];
		}

		StmtData* GetData(const std::string& name)
		{
			StmtData* pItem = nullptr;
			auto it = map_datas.find(name);
			if (it != map_datas.end())
			{
				pItem = &it->second;
			}
			return pItem;
		}

		bool bResult;

	private:
		std::vector<StmtData> vec_datas;
		std::unordered_map<std::string, StmtData> map_datas;
	};

	struct StmtSyncResult;

	struct Stmt
	{
		Stmt(int count)
		{
			stmt = nullptr;
			m_Result = nullptr;
			CircularQuerys.resize(count);
		}

		MYSQL_STMT* stmt;
		StmtBindData* m_Result;

		int m_numIn;
		StmtBindData* GetNewStmtParam()
		{
			StmtBindData* pItem = new StmtBindData(m_numIn);
			pItem->m_Index = m_Index;
			return pItem;
		}

		void AsynExecuteQueryVariable(StmtBindData* param, std::function<void(StmtExData*)> cb);
		void SyncExecuteQueryVariable(StmtBindData* param, StmtSyncResult& res);

		bool update();
		void Execute(StmtExData& row, StmtBindData* res);

		int FormatResult(StmtExData& row, StmtBindData* m_Result);

		std::list<StmtExData> querys;
		std::vector<std::vector<StmtExData>> CircularQuerys;
		std::unordered_map<std::string, std::vector<int>> m_Index;
		int in_pos = 0;
		int out_pos = 0;
	};

	struct StmtExData
	{
		void free()
		{
			if (param != nullptr)
			{
				param->free();
				delete param;
				param = nullptr;
			}
		}

		bool GetResult()
		{
			return result.bResult;
		}

		bool eof()
		{
			return ret != 0;
		}

		void nextRow()
		{
			ret = stmt->FormatResult(*this, m_Result);
		}

		void call_back(Stmt* st, bool res, StmtBindData* sbd)
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

		const char* geterror()
		{
			return mysql_stmt_error(stmt->stmt);
		}

		StmtData* GetData(int index)
		{
			return result.GetData(index);
		}

		StmtData* GetData(const std::string& name)
		{
			return result.GetData(name);
		}

		int GetInt32(int index)
		{
			return *(int*)GetData(index)->buffer;
		}

		int GetInt32(const std::string& name)
		{
			StmtData* p = GetData(name);
			if (!p)
				return 0;
			return *(int*)p->buffer;
		}

		long long GetInt64(int index)
		{
			return *(long long*)GetData(index)->buffer;
		}

		long long GetInt64(const std::string& name)
		{
			StmtData* p = GetData(name);
			if (!p)
				return 0;
			return *(long long*)p->buffer;
		}

		char* GetString(int index)
		{
			return (char*)GetData(index)->buffer;
		}

		char* GetString(const std::string& name)
		{
			StmtData* p = GetData(name);
			if (!p)
				return "";
			return (char*)p->buffer;
		}

		time_t GetDateTime(int index)
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

		time_t GetDateTime(const std::string& name)
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

		int GetSize(int index)
		{
			return GetData(index)->length;
		}

		int GetSize(const std::string& name)
		{
			StmtData* pp = GetData(name);
			if (!pp)
				return 0;
			return pp->length;
		}

		StmtBindData* param;
		StmtBindData* m_Result;
		StmtResult result;

		Stmt* stmt;
		int ret;
		std::function<void(StmtExData*)> cb;
	};

	struct StmtSyncResult
	{
		StmtSyncResult()
		{
			pResult.param = nullptr;
			res_param = nullptr;
			stmt = nullptr;
		}
		StmtExData pResult;
		StmtBindData* res_param;

		MYSQL_STMT* stmt;

		~StmtSyncResult()
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
	};

	class MysqlStmt
	{
	public:
		bool Open(const char* host, const char* user, const char* passwd, const char* db,
			unsigned int port = 3306, const char* charset = "utf8", unsigned long flag = 0);

		void Close();

		StmtBindData* PrepareQuery(const char* pSql);
		void AsynExecuteQueryVariable(std::string& sql, StmtBindData* param, std::function<void(StmtExData*)> cb);
		void SyncExecuteQueryVariable(std::string& sql, StmtBindData* param, StmtSyncResult& res);

		bool Update();

		std::unordered_map<std::string, Stmt> stmts;
	private:
		std::string AnalyzeSql(Stmt* stmt, std::string& sql);
		MYSQL* mysql;
	};
}