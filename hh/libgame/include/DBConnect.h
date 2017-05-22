#pragma once

#include "json/json.h"
#include "CustParament.h"
#include "DBInterface.h"
#include "CustParament.h"

struct DBCommand;
typedef boost::shared_ptr<DBCommand> DBCommandPtr;
typedef xlist<DBCommandPtr> DBCommandCollect;

typedef boost::function<void (const  DBCommandPtr  )> DBCallBack;

enum DBCommandType
{
	DCT_querySQL=0,
	DCT_execSQL,
	DCT_execProcedurce,
	DCT_execProcedurceQuery,
};




struct DBCommand
{
	DBCommand(const xstring& sql,DBCallBack cb,DBCommandType cmdtype, CustParament* userdata,const xstring& getResultSql="")
		: m_Sql(sql),m_cb(cb),m_pUserData(userdata),m_CmdType(cmdtype),m_getResultSql(getResultSql)
	{

	}

	~DBCommand()
	{
		SafeDelete(m_pUserData);
	}


	xstring				m_Sql;				//查询sql语句
	xstring				m_getResultSql;		//获取存储过程返回结果sql
	DBCallBack			m_cb;				//回调函数
	CustParament*		m_pUserData;		//用户数据
	DBCommandType		m_CmdType;			//命令类型
	bool				m_Result;			//操作结果,成功 失败
	xstring				m_Msg;				//失败原因
	MySQLQuery			m_query;			//所有获取的返回值都在这里
};



/**
* @brief		数据库连接对象
* @details		
* @date			2014/7/28 9:35
* @note			无 
*/ 
class DBConnect
{
	friend class DBService;

public:
	DBConnect(const char* host, const char* user, const char* passwd, const char* db,int32 port = 3306, const char* charset="utf8", int32 flag = 0);
	~DBConnect(void);

public:
	DBInterface* getDBInterface(){ return m_pDBInterface; }

protected:
	/**
	* @brief		工作线程
	*/
	void workThreadFun();


	/**
	* @brief		关闭连接
	*/
	void close();


	/**
	* @brief		增加一个数据库操作
	* @param[in]	pCommand[DBCommand*] : 数据库操作封装
	* @return		bool
	* @remarks		
	*/
	bool addCommand(DBCommandPtr pCommand);


	/**
	* @brief		设置操作结果回调
	* @param[in]	cb[DBCallBack] : 回调
	* @return		void
	* @remarks		
	*/
	void setResultHandle(DBCallBack cb);


	void handleDBCommand(DBCommandPtr& pCommand);


private:
	boost::thread*		m_pThread;				/** 工作线程 */

	DBInterface*		m_pDBInterface;			/** MYSQL数据库操作接口 */

	bool				m_bQuit;				/** 是否退出 */

	boost::condition_variable	m_Condition;	/** 条件变量，等待处理队列有数据 */

	DBCommandCollect       m_CommandCollect;	/** DBCommand队列，等待处理 */

	boost::mutex           m_CommandMutex;		/** DB命令互斥保护对象 */

	DBCallBack	m_ResultHandle;					/** 结果处理函数 */
};


typedef xvector<DBConnect*> DBConnectCollect;