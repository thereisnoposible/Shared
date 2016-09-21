#include "DBResult.h"
#include "Helper.h"

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

	return Helper::StringToInt(it->second.pData);
}

long long DBResult::getLongLongField(const char* szField)
{
	std::unordered_map<std::string, DBdata>::iterator it = rowData.find(szField);
	if (it == rowData.end())
		return 0;

	return Helper::StringToLongLong(it->second.pData);
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
		return ;

	len = it->second.length;
	memcpy((void*)pdata, it->second.pData, len);
}