#pragma once
#include <unordered_map>
#ifdef _WIN64
#include "mysql/mysql64/include/mysql.h"
#include "mysql/mysql64/include/mysql_com.h"
#else
#include "mysql/mysql32/include/mysql.h"
#include "mysql/mysql32/include/mysql_com.h"
#endif 

class DBResult
{
public:
	DBResult() :mysql(nullptr), pResult(nullptr), row(nullptr){}
	~DBResult()
	{
		if (pResult!=nullptr)
			mysql_free_result(pResult);
	}
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