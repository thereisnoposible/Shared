#ifndef __MYSQL_H__
#define __MYSQL_H__
#include "fcMySQLDefin.h"
#include "../tools/fcWriteFile.h"

namespace FoldCat
{
namespace fcMySQL
{

class fcMySQLClient
{
public:
	fcMySQLClient(void);
	virtual ~fcMySQLClient(void);
	//初始化连接
	bool Init(const char *pIP, const char *pAccount, const char *pPW, const char *pDBName, const char *pCharacter="set names utf8");
	//结束连接
	void Exit();
	//设置字符集
	void SetCharacter(const char *pc="set names utf8");
	//重新连接
	bool Reconnect();
	//写日志
	void SaveLog(const char* str, ...);
protected:
	//准备查询语句
	virtual bool InitQuery()=0;
	//退出查询
	virtual void ExitQuery()=0;
	//连接数据库
	bool Connect();

	MYSQL*		m_pConnect;
	char		m_IP[20];
	char		m_account[64];
	char		m_password[64];
	char		m_name[64];
	char		m_character[32];
	fcWriteFile	m_log;
	bool		m_isConnected;
};

}
}
#endif
