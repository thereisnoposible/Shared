#ifndef __FC_DB_THREAD_INTERFACE_H__
#define __FC_DB_THREAD_INTERFACE_H__
#include "../thread/fcThread.h"
#include "../thread/fcLock.h"
#include "../container/fcOrderContainer.h"
#include "../byteBuffer/fcWriteByteBuffer.h"
#include "../tools/fcTime.h"
#include "fcMySQL.h"

#define FC_MAX_QUEUE_SIZE	10485760	//10mb

namespace FoldCat
{
namespace fcMySQL
{

struct DBMsg_s
{
	u16		msgID;		//查询ID
	i32		count;		//查询结果记录数
	void*	sender;		//参数指针
};

class fcDBThreadInterface : public fcThread
{
public:
	fcDBThreadInterface(fcMySQLClient& c);
	~fcDBThreadInterface(void);
private:
	u32 ThreadMemberFunc();
public:
	//启动数据库线程
	bool Start(const char *pIP, const char *pAccount, const char *pPW, const char *pDBName);
	//增加一个查询消息
	bool AddQueryMsg( const char *msg, u32 size );
	//获取所有查询结果
	fcOrderContainer& GetReplyMsg();
	//获取线程帧速
	u32 GetFreameTime() {return m_timer.m_msPerFrame;}
protected:
	//查询路由，由子类实现
	virtual void DBMsgRoute(DBMsg_s* pdbm, const char* pData, const u32 dataLen)=0;
	//执行查询的函数
	void OnDBMsgNullRecord(fcMySQLQuery& query, DBMsg_s* pdbm, const char* pData, const u32 dataLen);
	void OnDBMsgOneRecord(fcMySQLQuery& query, DBMsg_s* pdbm, const char* pData, const u32 dataLen);
	void OnDBMsgMultiRecord(fcMySQLQuery& query, DBMsg_s* pdbm, const char* pData, const u32 dataLen);

private:
	fcMySQLClient	&		m_mysql;
	fcOrderContainer		m_queryQueue[2];
	fcOrderContainer		m_replyQueue[2];
	fcLock					m_queryLock;
	fcLock					m_replyLock;
	fcWriteByteBuffer		m_wb;
	fcFrameTimer			m_timer;
};

}
}
#endif
