#include "pch.h"
#include "DBService.h"


//----------------------------------------------------------------------------------
DBService::DBService(int32 num)
	: m_pDBInterface(NULL),m_NumOfConnect(num)
{
}


//----------------------------------------------------------------------------------
DBService::~DBService(void)
{
	for(int32 i=0;i<(int32)m_DBConnectCollect.size();++i)
	{
		SafeDelete( m_DBConnectCollect[i]);	
	}

	m_DBConnectCollect.clear();

	SafeDelete(m_pDBInterface);
}


//----------------------------------------------------------------------------------
bool DBService::Open(const char* host, const char* user, const char* passwd, const char* db,
		  unsigned int port, const char* charset, unsigned long flag)
{
	if(host==NULL||user==NULL||passwd==NULL||db==NULL||charset==NULL)
	{
		return false;
	}

	m_DBConnectCollect.clear();

	SafeDelete(m_pDBInterface);

	m_pDBInterface=new DBInterface();

	bool b=m_pDBInterface->open(host,user,passwd,db,port,charset,flag);

	if(b==false)
	{
		return false;
	}


	//创建多个数据库连接
	for(int32 i=0;i<m_NumOfConnect;++i)
	{
		DBConnect* pConnect= new DBConnect(host,user,passwd,db,port,charset,flag);
		//设置回调
		pConnect->setResultHandle(boost::bind(&DBService::onQueryResult,this,_1));
		m_DBConnectCollect.push_back(pConnect);
	}

	return true;
}


//----------------------------------------------------------------------------------
void DBService::onQueryResult(DBCommandPtr pCommand)
{
	boost::mutex::scoped_lock oLock(m_QueryResultMutex);
	m_QueryResultCollect.push_back(pCommand);
}


//----------------------------------------------------------------------------------
void DBService::Update()
{
	boost::mutex::scoped_lock oLock(m_QueryResultMutex);
	DBCommandCollect tmp = m_QueryResultCollect;
	m_QueryResultCollect.clear();
	oLock.unlock();

	//调用回调
	DBCommandCollect::iterator it = tmp.begin();
	DBCommandCollect::iterator itEnd = tmp.end();

	for(; it!=itEnd; it++)
	{
		DBCommandPtr& pCommand = *it;

		if(pCommand->m_cb!=NULL)
		{
			pCommand->m_cb(pCommand);
		}
	}
}


//----------------------------------------------------------------------------------
bool DBService::AsynQuerySQL(int32 ckey,const xstring& sql,DBCallBack cb,CustParament* userdata)
{
	if(sql.empty())
	{
		SafeDelete(userdata);
		return false;
	}

	int32 threadid = getThreadID(ckey);

	DBCommandPtr pCommand = DBCommandPtr(new DBCommand(sql,cb,DCT_querySQL,userdata));

	return m_DBConnectCollect[threadid]->addCommand(pCommand);
}


//----------------------------------------------------------------------------------
bool DBService::AsynExcuteSQL(int32 ckey,const xstring& sql,DBCallBack cb,CustParament* userdata)
{
	if(sql.empty())
	{
		SafeDelete(userdata);
		return false;
	}
	int32 threadid = getThreadID(ckey);

	DBCommandPtr pCommand = DBCommandPtr(new DBCommand(sql,cb,DCT_execSQL,userdata));

	return  m_DBConnectCollect[threadid]->addCommand(pCommand);
}


//----------------------------------------------------------------------------------
bool DBService::AsynExcuteProceduce(int32 ckey,const xstring& sql,DBCallBack cb,CustParament* userdata)
{
	if(sql.empty())
	{
		SafeDelete(userdata);
		return false;
	}
	int32 threadid = getThreadID(ckey);

	DBCommandPtr pCommand = DBCommandPtr(new DBCommand(sql,cb,DCT_execProcedurce,userdata));

	return  m_DBConnectCollect[threadid]->addCommand(pCommand);

}


//----------------------------------------------------------------------------------
bool DBService::AsynExcuteProceduceQuery(int32 ckey,const xstring& sql,DBCallBack cb,CustParament* userdata,const xstring& getresultSql)
{
	if(sql.empty())
	{
		SafeDelete(userdata);
		return false;
	}
	int32 threadid = getThreadID(ckey);

	DBCommandPtr pCommand = DBCommandPtr(new DBCommand(sql,cb,DCT_execProcedurceQuery,userdata,getresultSql));

	return  m_DBConnectCollect[threadid]->addCommand(pCommand);
}


//----------------------------------------------------------------------------------
bool DBService::SyncQuerySQL(const xstring& sql,MySQLQuery& query)
{
	if(m_pDBInterface==NULL)
	{
		return false;
	}

	return m_pDBInterface->querySQL(sql.c_str(),query);
}


//----------------------------------------------------------------------------------
int32 DBService::SyncExcuteSQL(const xstring& sql)
{
	if(m_pDBInterface==NULL)
	{
		return false;
	}

	return m_pDBInterface->execSQL(sql.c_str());
}


//----------------------------------------------------------------------------------
bool DBService::SyncExcuteProceduce(const xstring& sql)
{
	if(m_pDBInterface==NULL)
	{
		return false;
	}

	return m_pDBInterface->execProcedurce(sql.c_str());
}


//----------------------------------------------------------------------------------
bool DBService::SyncExcuteProceduceQuery(const xstring& sql,MySQLQuery& query,const xstring& getresultSql)
{
	if(m_pDBInterface==NULL)
	{
		return false;
	}

	bool b = m_pDBInterface->execProcedurce(sql.c_str());

	if(b==false)
	{
		return false;
	}

	if(getresultSql.empty()==false)
	{//有查询操作
		b=m_pDBInterface->querySQL(getresultSql,query);
		if(b==false)
		{
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------
MYSQL* DBService::GetMySqlConnect()
{
	CHECKERRORANDRETURNVALUE(m_pDBInterface != NULL, NULL);

	return m_pDBInterface->getMysql();
}

//---------------------------------------------------------------------------------------------
MYSQL* DBService::GetMySqlConnect(int32 ckey)
{
	CHECKERRORANDRETURNVALUE(m_pDBInterface != NULL, NULL);

	int32 threadid = getThreadID(ckey);
	return m_DBConnectCollect[threadid]->getDBInterface()->getMysql();
}