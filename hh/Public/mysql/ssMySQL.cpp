#include "fcMySQL.h"
#include <cstdio>
#include "../fcAssert.h"


namespace FoldCat
{
namespace fcMySQL
{

fcMySQLClient::fcMySQLClient(void)
{
	m_pConnect = 0;
	memset(m_IP, 0, sizeof(m_IP));
	memset(m_account, 0, sizeof(m_account));
	memset(m_password, 0, sizeof(m_password));
	memset(m_name, 0, sizeof(m_name));
	memset(m_character, 0, sizeof(m_character));
	m_log.Init("./msql_log.log");
	m_isConnected = false;
}

fcMySQLClient::~fcMySQLClient(void)
{
	//Exit();
}

//初始化连接
bool fcMySQLClient::Init(const char *pIP, const char *pAccount, const char *pPW, const char *pDBName, const char *pCharacter)
{
	strcpy(m_IP, pIP);
	strcpy(m_account, pAccount);
	strcpy(m_password, pPW);
	strcpy(m_name, pDBName);
	strcpy(m_character, pCharacter);
	m_isConnected = Connect();
	if (!m_isConnected)
		return false;
	m_isConnected = InitQuery();
	return m_isConnected;
}
//结束连接
void fcMySQLClient::Exit()
{
	ExitQuery();
	if (m_pConnect)
	{
		mysql_close(m_pConnect);
		m_pConnect = 0;
	}
	m_isConnected = false;
}
//设置字符集
void fcMySQLClient::SetCharacter(const char *pc)
{
	//mysql_query(m_pConnect, "set names gbk");
	mysql_query(m_pConnect, pc);
	strcpy(m_character, pc);
}

bool fcMySQLClient::Reconnect()
{
	if (!m_isConnected)
	{
		Exit();
		m_isConnected = Connect();
		if (!m_isConnected)
		{
			SaveLog("mysql reconnect failed\n");
			return m_isConnected;
		}
		m_isConnected = InitQuery();
		if (!m_isConnected)
		{
			SaveLog("mysql reconnect InitQuery failed\n");
			return m_isConnected;
		}
		return m_isConnected;
	}
	return true;
}

void fcMySQLClient::SaveLog( const char* str, ...)
{
	char tmpStr[256] = {0};
	va_list arglist;
	va_start(arglist, str);
	vsprintf(tmpStr, str, arglist);
	va_end(arglist);
	printf(tmpStr);
	m_log.WriteSomeAndtime(tmpStr);
}

bool fcMySQLClient::Connect()
{
	if (!m_pConnect)
	{
		//初始化
		m_pConnect = mysql_init(NULL);
		if (!m_pConnect)
		{
			SaveLog("mysql_init error! %u: %s\n", mysql_errno(m_pConnect), mysql_error(m_pConnect));
			return false;
		}
		my_bool mybool = 1;
		mysql_options(m_pConnect, MYSQL_OPT_RECONNECT, &mybool);
		//连接
		if (!mysql_real_connect(m_pConnect, m_IP, m_account, m_password, m_name, 3306, NULL, CLIENT_MULTI_STATEMENTS|CLIENT_INTERACTIVE))
		{
			SaveLog("mysql_real_connect error! %u: %s\n", mysql_errno(m_pConnect), mysql_error(m_pConnect));
			mysql_close(m_pConnect);
			return false;
		}
		//选择db
		//if (mysql_select_db(m_pConnect, pDBName))
		//{
		//	printf("Error %u: %s\n", mysql_errno(m_pConnect), mysql_error(m_pConnect));
		//	mysql_close(m_pConnect);
		//	return false;
		//}
		mysql_query(m_pConnect, m_character);
		printf("...MySQL server version: %s\n", mysql_get_server_info(m_pConnect));
		printf("...MySQL client version: %s\n", mysql_get_client_info());
		mysql_query(m_pConnect, "set wait_timeout = 432200");
		return true;
	}
	return false;
}

}
}
