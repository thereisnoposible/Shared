#pragma once
#include "../include/define/define.h"

#include <unordered_map>

struct st_mysql;
struct st_mysql_res;

typedef char **MYSQL_ROW;		/* return data as array of strings */
typedef st_mysql MYSQL;
typedef st_mysql_res MYSQL_RES;

#pragma warning (push)

#pragma warning (disable: 4251)

namespace xlib
{
	class XDLL DBResult
	{
	public:
		friend class DBService;
		friend struct SqlThread;

		DBResult() :mysql(nullptr), pResult(nullptr), row(nullptr), isSucce(true){}
		~DBResult();
		bool eof();
		void nextRow();

		int getIntField(const char* szField);
		float getFloatField(const char* szField);
		long long getLongLongField(const char* szField);
		const char* getStringField(const char* szField);
		void getBlobField(const char* szField, char* pdata, int& len);

		bool IsSuccess();
		std::string& GetError();
	public:
		struct DBdata
		{
			char* pData;
			unsigned int length;
		};

	private:
		void GetResult(MYSQL_RES* p);
		MYSQL* mysql;
		MYSQL_RES* pResult;
		MYSQL_ROW  row;
		std::unordered_map<std::string, DBdata> rowData;
		std::string err;
		bool isSucce;
	};
}

#pragma warning (pop)