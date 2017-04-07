
#include "../include/mysql/DBResult.h"
#include "../include/helper/Helper.h"

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
	DBResult::~DBResult()
	{
		if (pResult != nullptr)
			mysql_free_result(pResult);
	}

	bool DBResult::eof()
	{
		if (row == NULL)
			return true;
		return false;
	}

	void DBResult::GetResult(MYSQL_RES* p)
	{
		if (pResult != nullptr)
		{
			mysql_free_result(pResult);
		}

		pResult = p;
		row = mysql_fetch_row(pResult);

		if (row != nullptr)
		{
			mysql_field_seek(pResult, 0);//定位到第0列
			MYSQL_FIELD* pField;
			int i = 0;
			unsigned long* fieldlen = mysql_fetch_lengths(pResult);
			while ((pField = mysql_fetch_field(pResult)) != NULL)
			{
				rowData[pField->name].pData = row[i];
				rowData[pField->name].length = fieldlen[i];
				i++;
			}
		}
	}

	void DBResult::nextRow()
	{
		rowData.clear();
		row = mysql_fetch_row(pResult);

		if (row != nullptr)
		{
			mysql_field_seek(pResult, 0);//定位到第0列
			MYSQL_FIELD* pField;
			int i = 0;
			unsigned long* fieldlen = mysql_fetch_lengths(pResult);
			while ((pField = mysql_fetch_field(pResult)) != NULL)
			{
				rowData[pField->name].pData = row[i];
				rowData[pField->name].length = fieldlen[i];
				i++;
			}
		}
	}

	int DBResult::getIntField(const char* szField)
	{
		std::unordered_map<std::string, DBdata>::iterator it = rowData.find(szField);
		if (it == rowData.end())
			return 0;

		return Helper::StringToInt32(it->second.pData);
	}

	long long DBResult::getLongLongField(const char* szField)
	{
		std::unordered_map<std::string, DBdata>::iterator it = rowData.find(szField);
		if (it == rowData.end())
			return 0;

		return Helper::StringToInt64(it->second.pData);
	}

	float DBResult::getFloatField(const char* szField)
	{
		std::unordered_map<std::string, DBdata>::iterator it = rowData.find(szField);
		if (it == rowData.end())
			return 0.0;

		return (float)atof(it->second.pData);
	}



	const char* DBResult::getStringField(const char* szField)
	{
		std::unordered_map<std::string, DBdata>::iterator it = rowData.find(szField);
		if (it == rowData.end())
			return "";

		return it->second.pData;
	}


	void DBResult::getBlobField(const char* szField, char* pdata, int& len)
	{
		std::unordered_map<std::string, DBdata>::iterator it = rowData.find(szField);
		if (it == rowData.end())
			return;

		len = it->second.length;
		memcpy((void*)pdata, it->second.pData, len);
	}
}

