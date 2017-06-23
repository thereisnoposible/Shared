
#include "../include/mysql2/DBService2.h"

namespace xlib
{
	//----------------------------------------------------------------------------------
	DBService2::DBService2(int32 num)
		: m_pDBInterface(NULL), m_NumOfConnect(num), m_pLogService2(NULL)
	{
	}


	//----------------------------------------------------------------------------------
	DBService2::~DBService2(void)
	{
		for (int32 i = 0; i < (int32)m_DBConnectCollect.size(); ++i)
		{
			if (m_DBConnectCollect[i])
			{
				delete m_DBConnectCollect[i];
				m_DBConnectCollect[i] = nullptr;
			}
		}

		m_DBConnectCollect.clear();

		if (m_pDBInterface)
		{
			delete m_pDBInterface;
			m_pDBInterface = nullptr;
		}
	}

	//----------------------------------------------------------------------------------
	void DBService2::SetLogService(LogService2* p)
	{
		m_pLogService2 = p;
	}

	//----------------------------------------------------------------------------------
	bool DBService2::Open(const char* host, const char* user, const char* passwd, const char* db,
		unsigned int port, const char* charset, unsigned long flag)
	{
		if (host == NULL || user == NULL || passwd == NULL || db == NULL || charset == NULL)
		{
			return false;
		}

		m_DBConnectCollect.clear();

		if (m_pDBInterface)
		{
			delete m_pDBInterface;
			m_pDBInterface = nullptr;
		}

		m_pDBInterface = new DBInterface();
		m_pDBInterface->setLogService2(m_pLogService2);
		bool b = m_pDBInterface->open(host, user, passwd, db, port, charset, flag);

		if (b == false)
		{
			return false;
		}


		//创建多个数据库连接
		for (int32 i = 0; i < m_NumOfConnect; ++i)
		{
			DBConnect* pConnect = new DBConnect(host, user, passwd, db, port, charset, flag, m_pLogService2);
			//设置回调
			pConnect->setResultHandle(boost::bind(&DBService2::onQueryResult, this, _1));
			m_DBConnectCollect.push_back(pConnect);
		}

		return true;
	}


	//----------------------------------------------------------------------------------
	void DBService2::onQueryResult(DBCommandPtr pCommand)
	{
		boost::mutex::scoped_lock oLock(m_QueryResultMutex);
		m_QueryResultCollect.push_back(pCommand);
	}


	//----------------------------------------------------------------------------------
	void DBService2::Update()
	{
		boost::mutex::scoped_lock oLock(m_QueryResultMutex);
		DBCommandCollect tmp = m_QueryResultCollect;
		m_QueryResultCollect.clear();
		oLock.unlock();

		//调用回调
		DBCommandCollect::iterator it = tmp.begin();
		DBCommandCollect::iterator itEnd = tmp.end();

		for (; it != itEnd; it++)
		{
			DBCommandPtr& pCommand = *it;

			if (pCommand->m_cb != NULL)
			{
				pCommand->m_cb(pCommand);
			}
		}
	}


	//----------------------------------------------------------------------------------
	bool DBService2::AsynQuerySQL(int32 ckey, const std::string& sql, DBCallBack cb, CustParament* userdata)
	{
		if (sql.empty())
		{
			if (userdata)
			{
				delete userdata;
				userdata = nullptr;
			}
			return false;
		}

		int32 threadid = getThreadID(ckey);

		DBCommandPtr pCommand = DBCommandPtr(new DBCommand(sql, cb, DCT_querySQL, userdata));

		return m_DBConnectCollect[threadid]->addCommand(pCommand);
	}


	//----------------------------------------------------------------------------------
	bool DBService2::AsynExcuteSQL(int32 ckey, const std::string& sql, DBCallBack cb, CustParament* userdata)
	{
		if (sql.empty())
		{
			if (userdata)
			{
				delete userdata;
				userdata = nullptr;
			}
			return false;
		}
		int32 threadid = getThreadID(ckey);

		DBCommandPtr pCommand = DBCommandPtr(new DBCommand(sql, cb, DCT_execSQL, userdata));

		return  m_DBConnectCollect[threadid]->addCommand(pCommand);
	}


	//----------------------------------------------------------------------------------
	bool DBService2::AsynExcuteProceduce(int32 ckey, const std::string& sql, DBCallBack cb, CustParament* userdata)
	{
		if (sql.empty())
		{
			if (userdata)
			{
				delete userdata;
				userdata = nullptr;
			}
			return false;
		}
		int32 threadid = getThreadID(ckey);

		DBCommandPtr pCommand = DBCommandPtr(new DBCommand(sql, cb, DCT_execProcedurce, userdata));

		return  m_DBConnectCollect[threadid]->addCommand(pCommand);

	}


	//----------------------------------------------------------------------------------
	bool DBService2::AsynExcuteProceduceQuery(int32 ckey, const std::string& sql, DBCallBack cb, CustParament* userdata, const std::string& getresultSql)
	{
		if (sql.empty())
		{
			if (userdata)
			{
				delete userdata;
				userdata = nullptr;
			}
			return false;
		}
		int32 threadid = getThreadID(ckey);

		DBCommandPtr pCommand = DBCommandPtr(new DBCommand(sql, cb, DCT_execProcedurceQuery, userdata, getresultSql));

		return  m_DBConnectCollect[threadid]->addCommand(pCommand);
	}


	//----------------------------------------------------------------------------------
	bool DBService2::SyncQuerySQL(const std::string& sql, MySQLQuery& query)
	{
		if (m_pDBInterface == NULL)
		{
			return false;
		}

		return m_pDBInterface->querySQL(sql.c_str(), query);
	}


	//----------------------------------------------------------------------------------
	int32 DBService2::SyncExcuteSQL(const std::string& sql)
	{
		if (m_pDBInterface == NULL)
		{
			return false;
		}

		return m_pDBInterface->execSQL(sql.c_str());
	}


	//----------------------------------------------------------------------------------
	bool DBService2::SyncExcuteProceduce(const std::string& sql)
	{
		if (m_pDBInterface == NULL)
		{
			return false;
		}

		return m_pDBInterface->execProcedurce(sql.c_str());
	}


	//----------------------------------------------------------------------------------
	bool DBService2::SyncExcuteProceduceQuery(const std::string& sql, MySQLQuery& query, const std::string& getresultSql)
	{
		if (m_pDBInterface == NULL)
		{
			return false;
		}

		bool b = m_pDBInterface->execProcedurce(sql.c_str());

		if (b == false)
		{
			return false;
		}

		if (getresultSql.empty() == false)
		{//有查询操作
			b = m_pDBInterface->querySQL(getresultSql, query);
			if (b == false)
			{
				return false;
			}
		}

		return true;
	}

	//-------------------------------------------------------------------------------------------
	MYSQL* DBService2::GetMySqlConnect()
	{
		if (m_pDBInterface == NULL)
			return NULL;

		return m_pDBInterface->getMysql();
	}

	//---------------------------------------------------------------------------------------------
	MYSQL* DBService2::GetMySqlConnect(int32 ckey)
	{
		if (m_pDBInterface == NULL)
			return NULL;

		int32 threadid = getThreadID(ckey);
		return m_DBConnectCollect[threadid]->getDBInterface()->getMysql();
	}
}
