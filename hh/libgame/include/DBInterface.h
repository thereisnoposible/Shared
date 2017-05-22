/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		DBINTERFACE.H
** @brief		数据库操作接口
**  
** Details	
**  
** @version	1.0
** @date		2014/7/28 10:13
**  
** 修订说明：	最初版本 
** 
****************************************************************************/


#pragma  once

#include "mysql.h"
#include "mysql_com.h"


class XClass MySQLQuery
{
	friend class DBInterface;
public:
	MySQLQuery();

	virtual ~MySQLQuery();

	uint32 numRow();

	uint32 numFields();

	int32 fieldIndex(const char* szField);

	bool  getFiledType(uint32 index, enum_field_types& filedtype)const ;

	const char* fieldName(int nCol);

	uint32 seekRow(uint32 offerset);

	int32 getIntField(int32 nField, int32 nNullValue=0);

	int32 getIntField(const char* szField, int32 nNullValue=0);

	int64 getLongField(const char* szField,int64 nNUllValue=0);

	int64 getLongField(int nField,int64 nNUllValue=0);

	float getFloatField(int nField, float fNullValue=0.0);

	float getFloatField(const char* szField, float fNullValue=0.0);

	const char* getStringField(int nField, const char* szNullValue="");

	const char* getStringField(const char* szField, const char* szNullValue="");

	void getBlobField(const char* szField, char* pdata, int32& len);

	time_t getDateField(const char* szField,time_t t=0);

	bool fieldIsNull(int nField);

	bool fieldIsNull(const char* szField);

	bool eof();

	void nextRow();

	void finalize();

	void freeRes();

	bool next_result();

	void setmysqlptr(MYSQL*   ptr)
	{
		_db_ptr=ptr;
	}


private:
	void checkVM();

public:
	MYSQL_RES*  _mysql_res;
	MYSQL_FIELD* _field;
	MYSQL_ROW  _row;
	MYSQL*     _db_ptr;
};



/**
* @brief		MySQL数据库操作接口类
* @details		
* @date			2014/7/28 13:27
* @note			无 
*/ 
class  XClass  DBInterface 
{
public:

	DBInterface();

	virtual ~DBInterface();

	bool open(const char* host, const char* user, const char* passwd, const char* db,
		uint32 port = 0, const char* charset="utf8", unsigned long set = 0);

	void close();

	MYSQL* getMysql();

	bool  querySQL(const xstring& sql, MySQLQuery& pQuery);

	int32 execSQL(const xstring& sql);

	bool  execProcedurce(const xstring& sql);

	bool  ping();

	bool shutDown();

	int32 reboot();

	int32 startTransaction();

	int32 commit();

	int32 rollback();

	const char * getClientInfo();

	unsigned long  getClientVersion();

	const char * getHostInfo();

	const char * getServerInfo();

	unsigned long  getServerVersion();

	const char *  getCharacterSetName();

	const char * getSysTime();

	int32 createDB(const char* name);

	int32 dropDB(const char* name);

	const char * getErrorMsg();

private:

	/* msyql 连接句柄 */
	MYSQL* _db_ptr;

	MySQLQuery _db_query;
};