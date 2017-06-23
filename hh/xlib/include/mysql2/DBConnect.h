#pragma once

#include "../include/define/define.h"
//#include "json/json.h"
#include "CustParament.h"
#include "DBInterface.h"
#include "CustParament.h"

#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

#include "../log/LogService.h"

namespace xlib
{
	struct DBCommand;
	typedef boost::shared_ptr<DBCommand> DBCommandPtr;
	typedef std::list<DBCommandPtr> DBCommandCollect;

	typedef boost::function<void(const  DBCommandPtr)> DBCallBack;

	enum DBCommandType
	{
		DCT_querySQL = 0,
		DCT_execSQL,
		DCT_execProcedurce,
		DCT_execProcedurceQuery,
	};

	struct DBCommand
	{
		DBCommand(const std::string& sql, DBCallBack cb, DBCommandType cmdtype, CustParament* userdata, const std::string& getResultSql = "")
			: m_Sql(sql), m_cb(cb), m_pUserData(userdata), m_CmdType(cmdtype), m_getResultSql(getResultSql)
		{

		}

		~DBCommand()
		{
			if (m_pUserData)
			{
				delete m_pUserData;
				m_pUserData = nullptr;
			}
		}


		std::string				m_Sql;				//��ѯsql���
		std::string				m_getResultSql;		//��ȡ�洢���̷��ؽ��sql
		DBCallBack			m_cb;					//�ص�����
		CustParament*		m_pUserData;			//�û�����
		DBCommandType		m_CmdType;				//��������
		bool				m_Result;				//�������,�ɹ� ʧ��
		std::string				m_Msg;				//ʧ��ԭ��
		MySQLQuery			m_query;				//���л�ȡ�ķ���ֵ��������
	};



	/**
	* @brief		���ݿ����Ӷ���
	* @details
	* @date			2014/7/28 9:35
	* @note			��
	*/
	class DBConnect
	{
		friend class DBService2;

	public:
		DBConnect(const char* host, const char* user, const char* passwd, const char* db,
			int32 port = 3306, const char* charset = "utf8", int32 flag = 0, LogService2* p = nullptr);
		~DBConnect(void);

	public:
		DBInterface* getDBInterface(){ return m_pDBInterface; }

	protected:
		/**
		* @brief		�����߳�
		*/
		void workThreadFun();


		/**
		* @brief		�ر�����
		*/
		void close();


		/**
		* @brief		����һ�����ݿ����
		* @param[in]	pCommand[DBCommand*] : ���ݿ������װ
		* @return		bool
		* @remarks
		*/
		bool addCommand(DBCommandPtr pCommand);


		/**
		* @brief		���ò�������ص�
		* @param[in]	cb[DBCallBack] : �ص�
		* @return		void
		* @remarks
		*/
		void setResultHandle(DBCallBack cb);


		void handleDBCommand(DBCommandPtr& pCommand);


	private:
		boost::thread*		m_pThread;				/** �����߳� */

		DBInterface*		m_pDBInterface;			/** MYSQL���ݿ�����ӿ� */

		bool				m_bQuit;				/** �Ƿ��˳� */

		boost::condition_variable	m_Condition;	/** �����������ȴ�������������� */

		DBCommandCollect       m_CommandCollect;	/** DBCommand���У��ȴ����� */

		boost::mutex           m_CommandMutex;		/** DB����Ᵽ������ */

		DBCallBack	m_ResultHandle;					/** ��������� */
	};


	typedef std::vector<DBConnect*> DBConnectCollect;
}