
#include "../include/mysql2/DBInterface.h"
#include "../include/helper/Helper.h"
 
using namespace xlib;
//--------------------------------------------------------------------
MySQLQuery::MySQLQuery()
{
	_mysql_res = NULL;
	_field = NULL;
	_row = NULL;
}


//--------------------------------------------------------------------
MySQLQuery::~MySQLQuery()
{
	freeRes();
}

//--------------------------------------------------------------------
void MySQLQuery::freeRes()
{
	if ( _mysql_res != NULL )
	{
		mysql_free_result(_mysql_res);
		_mysql_res = NULL;
	}
}



//--------------------------------------------------------------------
uint32 MySQLQuery::numRow()
{
	if(_mysql_res==NULL)
		return 0;
	return (unsigned long)_mysql_res->row_count;
}

//--------------------------------------------------------------------
uint32 MySQLQuery::numFields()
{
	if(_mysql_res==NULL)
		return 0;
	return _mysql_res->field_count;
}
//--------------------------------------------------------------------
uint32 MySQLQuery::seekRow(uint32 offerset)
{
	if(_mysql_res==NULL)
		return 0;

	if ( offerset == numRow() )
	{
		offerset = numRow() -1;
	}

	mysql_data_seek(_mysql_res, offerset);
	_row = mysql_fetch_row(_mysql_res);
	return offerset;
}


//--------------------------------------------------------------------
bool   MySQLQuery::getFiledType(uint32 index, enum_field_types& filedtype) const 
{
	if(_mysql_res==NULL||index+1>_mysql_res->field_count)
	{
		return false;
	}

	mysql_field_seek(_mysql_res,index);
	MYSQL_FIELD* pFiled = mysql_fetch_field(_mysql_res);
	if(pFiled==NULL)
	{
		return false;
	}

	filedtype=pFiled->type;
	return true;

}

//--------------------------------------------------------------------

int32 MySQLQuery::fieldIndex(const char* szField)
{
	if ( NULL == _mysql_res )
		return -1;
	if ( NULL == szField )
		return -1;
	mysql_field_seek(_mysql_res, 0);//定位到第0列
	uint32 i = 0;
	while ( i < numFields() )
	{
		_field = mysql_fetch_field( _mysql_res );
		if ( _field != NULL && strcmp(_field->name, szField) == 0 )//找到
			return i;
		i++;
	}
	return -1;
}

//--------------------------------------------------------------------
const char* MySQLQuery::fieldName(int nCol)
{
	if ( _mysql_res == NULL )
		return NULL;
	mysql_field_seek(_mysql_res, nCol);
	_field = mysql_fetch_field(_mysql_res);
	if ( _field != NULL )
		return _field->name;
	else
		return  NULL;
}



//--------------------------------------------------------------------
int64  MySQLQuery::getLongField(const char* szField,int64 nNullValue)
{
	if ( NULL == _mysql_res || NULL == szField )
		return nNullValue;

	if ( NULL == _row )
		return nNullValue;
	const char* filed = getStringField(szField);
	if ( NULL == filed )
		return nNullValue;

#ifdef WIN32
	return _atoi64(filed);
#else
	return atoll(filed);
#endif

}


//---------------------------------------------------------------------
int64  MySQLQuery::getLongField(int nField,int64 nNullValue)
{
	if ( NULL == _mysql_res )
		return nNullValue;

	if ( nField + 1 > (int)numFields() )
		return nNullValue;

	if ( NULL == _row )
		return nNullValue;
	
#ifdef WIN32
	return _atoi64(_row[nField]);
#else
	return atoll(_row[nField]);
#endif


}

//--------------------------------------------------------------------
int32 MySQLQuery::getIntField(int32 nField, int32 nNullValue/*=0*/)
{
	if ( NULL == _mysql_res )
		return nNullValue;

	if ( nField + 1 > (int)numFields() )
		return nNullValue;

	if ( NULL == _row )
		return nNullValue;

	return atoi(_row[nField]);
}

//--------------------------------------------------------------------
int32 MySQLQuery::getIntField(const char* szField, int32 nNullValue/*=0*/)
{
	if ( NULL == _mysql_res || NULL == szField )
		return nNullValue;

	if ( NULL == _row )
		return nNullValue;
	const char* filed = getStringField(szField);
	if ( NULL == filed )
		return nNullValue;

	return Helper::StringToInt32(filed);
}

//--------------------------------------------------------------------
const char* MySQLQuery::getStringField(int32 nField, const char* szNullValue/*=""*/)
{
	if ( NULL == _mysql_res )
		return szNullValue;
	if ( nField + 1 > (int)numFields() )
		return szNullValue;
	if ( NULL == _row )
		return szNullValue;
	return _row[nField];
}


//--------------------------------------------------------------------
const char* MySQLQuery::getStringField(const char* szField, const char* szNullValue/*=""*/)
{
	if ( NULL == _mysql_res )
		return szNullValue;
	int32 nField = fieldIndex(szField);
	if ( nField == -1 )
		return szNullValue;

	return getStringField(nField);
}



time_t MySQLQuery::getDateField(const char* szField,time_t t)
{
	if(NULL == _mysql_res)
		return t;

	int32 nField = fieldIndex(szField);
	if(nField == -1)
		return t;

	std::string sDate = getStringField(nField);

	return Helper::String2Timet(sDate);
}


//--------------------------------------------------------------------
float MySQLQuery::getFloatField(int32 nField, float fNullValue/*=0.0*/)
{
	const char* field = getStringField(nField);
	if ( NULL == field )
		return fNullValue;
	return (float)atof(field);
}


//--------------------------------------------------------------------
float MySQLQuery::getFloatField(const char* szField, float fNullValue/*=0.0*/)
{
	const char* field = getStringField(szField);
	if ( NULL == field )
		return fNullValue;
	return (float)atof(field);
}


//--------------------------------------------------------------------
void MySQLQuery::getBlobField(const char* szField, char* pdata, int32& len)
{
	if (NULL == _mysql_res)
	{
		return;
	}

	len = 0;

	int32 nField = fieldIndex(szField);
	if (nField == -1)
	{
		return;
	}

	unsigned long* fieldlen = mysql_fetch_lengths(_mysql_res);
	len = fieldlen[nField];

	char* pDest = _row[nField];

	memcpy((void*)pdata, pDest, len);
}

//--------------------------------------------------------------------
void MySQLQuery::nextRow()
{
	if ( NULL == _mysql_res )
		return;
	_row = mysql_fetch_row(_mysql_res);
}

//--------------------------------------------------------------------
bool MySQLQuery::eof()
{
	if ( _row == NULL )
		return true;
	return false;
}

//--------------------------------------------------------------------
bool MySQLQuery::next_result()
{
	freeRes();
	int res=mysql_next_result(_db_ptr);
	if(res==0)
	{
		_mysql_res=mysql_store_result(_db_ptr);
		seekRow(0);
		return true;
	}
	return false;
}


//--------------------------------------------------------------------
DBInterface::DBInterface()
{
	_db_ptr = NULL;
	_logservice = NULL;
}


//--------------------------------------------------------------------
DBInterface::~DBInterface()
{
	close();

}

//--------------------------------------------------------------------
void DBInterface::setLogService2(LogService2* p)
{
	_logservice = p;
}


//--------------------------------------------------------------------
bool  DBInterface::open(const char* host, const char* user, const char* passwd, const char* db,
							uint32 port ,const char* charset, unsigned long flag )
{

	///如果已经打开了。直接关闭
	close();

	_db_ptr = mysql_init(NULL);
	if (NULL == _db_ptr)
	{
		return false;
	}

	//支持中文处理
	mysql_options(_db_ptr, MYSQL_SET_CHARSET_NAME,charset);

	///如果断线则重新在连接
	mysql_options(_db_ptr, MYSQL_OPT_RECONNECT,"1");

	//如果连接失败，返回NULL。对于成功的连接，返回值与第1个参数的值相同。
	if (NULL == mysql_real_connect(_db_ptr, host, user, passwd, db, port, NULL, flag))
	{
		return false;
	}
	
	//选择制定的数据库失败
	//0表示成功，非0值表示出现错误。
	if ( mysql_select_db( _db_ptr, db ) != 0 )
	{
		mysql_close(_db_ptr);
		_db_ptr = NULL;
		return false;
	}

	_db_query.setmysqlptr(_db_ptr); 

	return true;
}


//--------------------------------------------------------------------
void DBInterface::close()
{
	if ( _db_ptr != NULL )
	{
		mysql_close( _db_ptr );
		_db_ptr = NULL;
	}
}

//--------------------------------------------------------------------
MYSQL* DBInterface::getMysql()
{
	return _db_ptr;
}


//--------------------------------------------------------------------
bool  DBInterface::querySQL(const std::string& sql, MySQLQuery& pQuery)
{

	if(sql.length()==0)
	{
		return false;
	}

	if (pQuery._mysql_res!=NULL)
	{
		mysql_free_result(pQuery._mysql_res);
	}
	//
	if ( !mysql_real_query( _db_ptr, sql.c_str(), sql.length() ) )
	{
		pQuery._mysql_res=mysql_store_result( _db_ptr );
		pQuery.seekRow(0);
	}
	else
	{	
		const char* error=mysql_error(_db_ptr);
		if (_logservice)
			_logservice->LogMessage("%s,%s", sql.c_str(), error);

		return false;
	}
	return true;
}



//--------------------------------------------------------------------
int32 DBInterface::execSQL(const std::string& sql)
{
	if( !mysql_real_query( _db_ptr, sql.c_str(), sql.length() ) )
	{
		int effectrow = (int)mysql_affected_rows(_db_ptr);
		if (effectrow == -1)
		{
			if (_logservice)
				_logservice->LogMessage("%s", sql.c_str());
		}
		//得到受影响的行数
		return effectrow ;
	}
	else
	{
		const char* error=mysql_error(_db_ptr);

		if (_logservice)
			_logservice->LogMessage("%s,%s", sql.c_str(), error);

		//执行查询失败
		return -1;
	}
}


//--------------------------------------------------------------------
bool  DBInterface::execProcedurce(const std::string&  sql)
{
	if(_db_ptr==NULL||sql.length()==0)
		return false;

	int ret=mysql_real_query(_db_ptr,sql.c_str(),sql.length());
	if(ret!=0)
	{
		const char* error=mysql_error(_db_ptr);

		if (_logservice)
			_logservice->LogMessage("%s,%s", sql.c_str(), error);

		return false;
	}

	return true;
}


//--------------------------------------------------------------------
bool  DBInterface::ping()
{
	if( mysql_ping(_db_ptr) == 0 )
		return true;
	else
		return false;

	//mysql_affected_rows
}


//--------------------------------------------------------------------
bool  DBInterface::shutDown()
{
	if( mysql_shutdown(_db_ptr,SHUTDOWN_DEFAULT) == 0 )
		return true;
	else
		return false;
}

//--------------------------------------------------------------------
int32 DBInterface::reboot()
{
	if(!mysql_reload(_db_ptr))
		return 0;
	else
		return -1;
}


//--------------------------------------------------------------------
int32 DBInterface::startTransaction()
{
	if(!mysql_real_query(_db_ptr, "START TRANSACTION" ,(unsigned long)strlen("START TRANSACTION" )))
	{
		return 0;
	}
	else
	{
		//执行查询失败
		const char* error=mysql_error(_db_ptr);

		if (_logservice)
			_logservice->LogMessage("start transaction,%s", error);

		return -1;
	}

}


//--------------------------------------------------------------------
int32 DBInterface::commit()
{
	if(!mysql_real_query( _db_ptr, "COMMIT",(unsigned long)strlen("COMMIT" ) ))
	{
		return 0;
	}
	else
	{
		//执行查询失败
		const char* error=mysql_error(_db_ptr);

		if (_logservice)
			_logservice->LogMessage("commit,%s", error);

		return -1;
	}
}


//--------------------------------------------------------------------
int32 DBInterface::rollback()
{
	if(!mysql_real_query(_db_ptr, "ROLLBACK",(unsigned long)strlen("ROLLBACK" )) )
		return 0;
	else
		//执行查询失败
		return -1;
}


//--------------------------------------------------------------------
const char * DBInterface::getClientInfo()
{
	return mysql_get_client_info();
}


//--------------------------------------------------------------------
unsigned long  DBInterface::getClientVersion()
{
	return mysql_get_client_version();
}


//--------------------------------------------------------------------
const char * DBInterface::getHostInfo()
{
	return mysql_get_host_info(_db_ptr);
}


//--------------------------------------------------------------------
const char * DBInterface::getServerInfo()
{
	return mysql_get_server_info( _db_ptr );
}


//--------------------------------------------------------------------
unsigned long  DBInterface::getServerVersion()
{
	return mysql_get_server_version(_db_ptr);
}


//--------------------------------------------------------------------
const char *  DBInterface::getCharacterSetName()
{
	return mysql_character_set_name(_db_ptr);
}


//--------------------------------------------------------------------
const char * DBInterface::getSysTime()
{
	//return ExecQueryGetSingValue("select now()";
	return NULL;
}


//--------------------------------------------------------------------
int32 DBInterface::createDB(const char* name)
{
	char temp[1024];
	sprintf_s(temp,1024, "CREATE DATABASE %s", name);
	if(!mysql_real_query( _db_ptr, temp, strlen(temp)) )
		return 0;

	else
		//执行查询失败
		return -1;
}


//--------------------------------------------------------------------
int32 DBInterface::dropDB(const char*  name)
{
	char temp[1024];

	sprintf_s(temp, 1024,"DROP DATABASE %s", name);
	if(!mysql_real_query( _db_ptr, temp, strlen(temp)) )
		return 0;
	else
		//执行查询失败
		return -1;
}


//--------------------------------------------------------------------
const char* DBInterface::getErrorMsg()
{
	return mysql_error(_db_ptr);
}
