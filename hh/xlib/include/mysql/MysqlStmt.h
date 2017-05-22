#pragma once
#include <functional>
#include <unordered_map>
#include <assert.h>
#include <thread>
#include <condition_variable>

#include "../define/define.h"

#pragma warning (push)

#pragma warning (disable: 4251)

struct st_mysql;
struct st_mysql_res;
struct st_mysql_bind;
struct st_mysql_stmt;

typedef char **MYSQL_ROW;		/* return data as array of strings */
typedef st_mysql MYSQL;
typedef st_mysql_res MYSQL_RES;
typedef st_mysql_bind MYSQL_BIND;
typedef st_mysql_stmt MYSQL_STMT;

namespace xlib
{
	struct StmtData;
	struct StmtExData;
	struct Stmt;

	template class _declspec(dllexport) std::function<void(StmtExData*)>;
	struct StmtData
	{
		friend class MysqlStmt;
		friend struct Stmt;

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

		const char* GetName()
		{
			return name.c_str();
		}

		void* buffer;
		unsigned long length;

	private:
		std::string name;
	};

	struct StmtExData;

	struct XDLL StmtBindData
	{
		friend struct Stmt;
		friend class MysqlStmt;

		StmtBindData(int count);
		~StmtBindData();

		void CopyFrom(StmtBindData& from);

		void SetInt32(const std::string& key, int value);
		void SetInt32(int index, int value);

		void SetInt64(const std::string& key, long long value);
		void SetInt64(int index, long long value);

		void SetString(const std::string& key, const char* str, int len);
		void SetString(int index, const char* str, int len);
		void SetString(const std::string& key, const std::string& str);
		void SetString(int index, const std::string& str);

		void SetBlob(const std::string& key, const void* str, int len);
		void SetBlob(int index, const void* str, int len);

		void SetDateTime(const std::string& key, int year, int month, int day, int hour, int minute, int second);
		void SetDateTime(int index, int year, int month, int day, int hour, int minute, int second);

		void free();

		MYSQL_BIND* GetBind();
	private:
		std::vector<int>* GetIndexes(const std::string& key);

		MYSQL_BIND* m_bd;
		std::unordered_map<std::string, std::vector<int>> m_Index;
		std::vector<StmtData> m_datas;
	};

	struct XDLL StmtResult
	{
		StmtResult()
		{
		}
		~StmtResult()
		{
		}
		void reference_data(std::string& name, void* data, int length)
		{
			StmtData temp(data, length);
			(map_datas)[name] = temp;
			vec_datas.push_back(temp);
		}

		void clear()
		{
			vec_datas.clear();
			map_datas.clear();
		}

		StmtData* GetData(int index)
		{
			return &(vec_datas)[index];
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

	struct XDLL Stmt
	{
		friend class MysqlStmt;

		Stmt(int count)
		{
			stmt = nullptr;
			m_Result = nullptr;
		}

		~Stmt()
		{
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
	private:
		//std::list<StmtExData> querys;
		std::list<StmtExData*> CircularQuerys;
		std::unordered_map<std::string, std::vector<int>> m_Index;
	};

	struct XDLL StmtExData
	{
		friend struct Stmt;

		void free();

		bool GetResult();

		bool eof();

		void nextRow();

		void call_back(Stmt* st, bool res, StmtBindData* sbd);

		const char* geterror();

		StmtData* GetData(int index);
		StmtData* GetData(const std::string& name);

		int GetInt32(int index);
		int GetInt32(const std::string& name);

		long long GetInt64(int index);
		long long GetInt64(const std::string& name);

		char* GetString(int index);
		char* GetString(const std::string& name);

		time_t GetDateTime(int index);
		time_t GetDateTime(const std::string& name);

		int GetSize(int index);
		int GetSize(const std::string& name);

		StmtBindData* param;
		StmtBindData* m_Result;
		StmtResult result;

		Stmt* stmt;
		int ret;

	private:
		std::function<void(StmtExData*)> cb;
	};

	struct XDLL StmtSyncResult
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

		~StmtSyncResult();
	};

	class XDLL MysqlStmt
	{
	public:
		bool Open(const char* host, const char* user, const char* passwd, const char* db,
			unsigned int port = 3306, const char* charset = "utf8", unsigned long flag = 0);

		void Close();

		StmtBindData* PrepareQuery(const char* pSql);
		void AsynExecuteQueryVariable(std::string& sql, StmtBindData* param, std::function<void(StmtExData*)> cb);
		void SyncExecuteQueryVariable(std::string& sql, StmtBindData* param, StmtSyncResult& res);

		bool Update();

		
	private:
		std::unordered_map<std::string, Stmt> stmts;
		std::string AnalyzeSql(Stmt* stmt, std::string& sql);
		MYSQL* mysql;
	};
}

#pragma warning (pop)