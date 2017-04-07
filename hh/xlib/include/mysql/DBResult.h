#pragma once
#include <unordered_map>

struct st_mysql;
struct st_mysql_res;

typedef char **MYSQL_ROW;		/* return data as array of strings */
typedef st_mysql MYSQL;
typedef st_mysql_res MYSQL_RES;

namespace xlib
{
	class DBResult
	{
	public:
		DBResult() :mysql(nullptr), pResult(nullptr), row(nullptr){}
		~DBResult();
		bool eof();

		void GetResult(MYSQL_RES* p);
		void nextRow();

		int getIntField(const char* szField);

		long long getLongLongField(const char* szField);

		float getFloatField(const char* szField);

		const char* getStringField(const char* szField);

		void getBlobField(const char* szField, char* pdata, int& len);


	public:

		struct DBdata
		{
			char* pData;
			unsigned int length;
		};
		MYSQL* mysql;
		MYSQL_RES* pResult;
		MYSQL_ROW  row;
		std::unordered_map<std::string, DBdata> rowData;
	};
}

