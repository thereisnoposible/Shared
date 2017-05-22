#include "pch.h"
#include "DBConnect.h"
#include "DBInterface.h"
#include "DBService.h"

//--------------------------------------------------------------------
DBConnect::DBConnect(const char* host, const char* user, const char* passwd, const char* db,int32 port, const char* charset, int32 flag)
	: m_bQuit(false)
{
	m_pDBInterface=new DBInterface();

	bool b=m_pDBInterface->open(host,user,passwd,db,port,charset,flag);

	if(b)
	{//发起工作线程
		m_pThread=new boost::thread(boost::bind(&DBConnect::workThreadFun,this));
	}
	else
	{
		//抛出异常
		THROW("DBConnect error,Can not connect database!");
	}
}


//--------------------------------------------------------------------
DBConnect::~DBConnect(void)
{
	//关闭连接
	close();

	//等待线程退出
	m_pThread->join();

	//释放内存
	SafeDelete(m_pThread);

	SafeDelete(m_pDBInterface);
}


//--------------------------------------------------------------------
void DBConnect::workThreadFun()
{
	//循环处理
	while(!m_bQuit)
	{
		boost::mutex::scoped_lock oLock(m_CommandMutex);
		size_t size = m_CommandCollect.size();
		if(0 == size)
		{
			m_Condition.wait(oLock);
			continue;
		}

		//有数据,取出一条数据，并进行操作
		DBCommandPtr pCommand = m_CommandCollect.front();
		m_CommandCollect.pop_front();
		oLock.unlock();

		handleDBCommand(pCommand);

		//结果压回DBService，等待DBService的update处理
		m_ResultHandle(pCommand);
	}

	//未写完的记录，采用同步方式写入数据库!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	DBCommandCollect::iterator it = m_CommandCollect.begin();
	for (; it != m_CommandCollect.end(); it++)
	{
		handleDBCommand(*it);
	}
}

//-------------------------------------------------------------------------------------------
void DBConnect::handleDBCommand(DBCommandPtr& pCommand)
{
	bool b = false;

	int32 roweffect = 0;

	//设置为执行成功
	pCommand->m_Result = true;

	///执行sql命令并回调
	switch (pCommand->m_CmdType)
	{
	case DCT_querySQL:
		b = m_pDBInterface->querySQL(pCommand->m_Sql, pCommand->m_query);
		if (b == false)
		{
			pCommand->m_Result = false;
		}

		break;
	case DCT_execSQL:
		roweffect = m_pDBInterface->execSQL(pCommand->m_Sql);
		if (roweffect == -1)
		{
			pCommand->m_Result = false;
		}

		break;
	case DCT_execProcedurce:
		b = m_pDBInterface->execProcedurce(pCommand->m_Sql);
		if (b == false)
		{
			pCommand->m_Result = false;
		}

		break;
	case DCT_execProcedurceQuery:
		b = m_pDBInterface->execProcedurce(pCommand->m_Sql);
		if (b == false)
		{
			pCommand->m_Result = false;
			break;
		}

		if (pCommand->m_getResultSql.empty() == false)
		{//有查询操作
			b = m_pDBInterface->querySQL(pCommand->m_getResultSql, pCommand->m_query);
			if (b == false)
			{
				pCommand->m_Result = false;
				break;
			}
		}
		break;

	default:
		{
			   THROW("Error when excute sql!");
		}
	}
}


//-------------------------------------------------------------------
void DBConnect::close()
{
	m_bQuit = true;

	m_Condition.notify_all();
}


//-------------------------------------------------------------------
bool DBConnect::addCommand(DBCommandPtr pCommand)
{
	boost::mutex::scoped_lock oLock(m_CommandMutex);

	m_CommandCollect.push_back(pCommand);

	oLock.unlock();

	m_Condition.notify_all();

	return true;
}


//-------------------------------------------------------------------
void DBConnect::setResultHandle(DBCallBack cb)
{
	m_ResultHandle = cb;
}