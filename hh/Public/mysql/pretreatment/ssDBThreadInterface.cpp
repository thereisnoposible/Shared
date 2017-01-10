#include "fcDBThreadInterface.h"


namespace FoldCat
{
namespace fcMySQL
{

fcDBThreadInterface::fcDBThreadInterface(fcMySQLClient& c)
	:m_mysql(c)
{
	m_wb.Set(FC_MAX_QUEUE_SIZE/100);
	for (int i=0; i<2; ++i)
	{
		m_queryQueue[i].Init(FC_MAX_QUEUE_SIZE);
		m_replyQueue[i].Init(FC_MAX_QUEUE_SIZE);
	}
}

fcDBThreadInterface::~fcDBThreadInterface(void)
{
}

u32 fcDBThreadInterface::ThreadMemberFunc()
{
	fcOrderContainer::Node_s* pNode = 0;
	DBMsg_s *pMsg = 0;
	m_timer.Init(15000);
	while (!IsRequestStop())
	{
		m_timer.Update();

		BOOST_MUTEX_LOCK(m_queryLock)
		if (m_queryQueue[0].Size())
		{
			m_queryQueue[1].CopyFrom(m_queryQueue[0]);
			m_queryQueue[0].Empty();
		}
		BOOST_MUTEX_UNLOCK

		pNode = m_queryQueue[1].Begin();
		while(pNode)
		{
			pMsg = reinterpret_cast<DBMsg_s*>(pNode->pData);
			//临时代码
			if (pMsg->msgID == 5015)
			{
				FC_ASSERT_NORMAL( pNode->size == 24);
			}
			DBMsgRoute(pMsg, pNode->pData+sizeof(DBMsg_s), pNode->size-sizeof(DBMsg_s));
			pNode = m_queryQueue[1].Next();
		}
		m_queryQueue[1].Empty();
		Sleep(1);
	}
	return 0;
}

bool fcDBThreadInterface::Start( const char *pIP, const char *pAccount, const char *pPW, const char *pDBName )
{
	if (!m_mysql.Init(pIP, pAccount, pPW, pDBName))
	{
		return false;
	}
	StartThread();
	return true;
}

fcOrderContainer& fcDBThreadInterface::GetReplyMsg()
{
	m_replyLock.Lock();
	if(m_replyQueue[1].Size())
	{
		m_replyQueue[0].CopyFrom(m_replyQueue[1]);
		m_replyQueue[1].Empty();
	}
	m_replyLock.Unlock();
	return m_replyQueue[0];
}

bool fcDBThreadInterface::AddQueryMsg( const char* msg, u32 size )
{
	bool bOK = false;
	BOOST_MUTEX_LOCK(m_queryLock)
		/****************临时代码**************/
		if (*((u16*)msg) == 5015 )
		{
			FC_ASSERT_NORMAL(size == 24);
		}
		/*************************************/
	bOK = m_queryQueue[0].Push(msg, size);
	BOOST_MUTEX_UNLOCK
	return bOK;
}

//void fcDBThreadInterface::UserLogin(DBMsg_s* pdbm, const char* pData,const u32 dataLen)
//{
//	int row = m_mysql.m_userLogin.ExecuteQueryStream(dataLen, pData);
//	pdbm->count = row;
//	m_wb.Reset();
//	m_wb.WriteData(pdbm, sizeof(DBMsg_s));
//	if (row>0)
//	{
//		fcOrderContainer& oc = m_mysql.m_userLogin.GetResultSet();
//		m_wb.WriteU32(oc.Count());
//		CReadByteBuffer rb;
//		int nOK = 0;
//		fcOrderContainer::iterator_t pNode = oc.Begin();
//		while(pNode)
//		{
//			rb.Set(pNode->pData, pNode->size);
//			rb.ReadI32(nOK);
//			if (nOK==-1)
//			{
//				pdbm->count = -1;
//				break;
//			}
//			m_wb.WriteData(rb.CurrentBuffer(), rb.RemainSize());
//			pNode = oc.Next();
//		}
//	}
//	m_replyLock.Lock();
//	m_replyQueue[1].Push(m_wb.Buffer(), m_wb.Size());
//	m_replyLock.Unlock();
//}

void fcDBThreadInterface::OnDBMsgNullRecord( fcMySQLQuery& query, DBMsg_s* pdbm, const char* pData, const u32 dataLen )
{
	int row = query.ExecuteQueryStream(dataLen, pData);
	pdbm->count = row;
	m_wb.Reset();
	m_wb.WriteData(pdbm, sizeof(DBMsg_s));
	if (row>0 && query.GetResultFlag()&DB_RESULT_INSERT_ID)
	{
		m_wb.WriteU32(query.GetInsertID());
	}
	else
	{
		m_wb.WriteU32(0);
	}
	m_replyLock.Lock();
	m_replyQueue[1].Push(m_wb.Buffer(), m_wb.Size());
	m_replyLock.Unlock();
}

void fcDBThreadInterface::OnDBMsgOneRecord( fcMySQLQuery& query, DBMsg_s* pdbm, const char* pData, const u32 dataLen )
{
	int row = query.ExecuteQueryStream(dataLen, pData);
	pdbm->count = row;
	m_wb.Reset();
	m_wb.WriteData(pdbm, sizeof(DBMsg_s));
	if (row==1)
	{
		m_wb.WriteData(query.GetResult().Buffer(), query.GetResult().Size());
	}
	m_replyLock.Lock();
	m_replyQueue[1].Push(m_wb.Buffer(), m_wb.Size());
	m_replyLock.Unlock();
}

void fcDBThreadInterface::OnDBMsgMultiRecord( fcMySQLQuery& query, DBMsg_s* pdbm, const char* pData, const u32 dataLen )
{
	int row = query.ExecuteQueryStream(dataLen, pData);
	pdbm->count = row;
	m_wb.Reset();
	m_wb.WriteData(pdbm, sizeof(DBMsg_s));
	if (row>0)
	{
		fcOrderContainer& oc = query.GetResultSet();
		m_wb.WriteU32(oc.Count());
		fcOrderContainer::iterator_t pNode = oc.Begin();
		while(pNode && pNode->size)
		{
			m_wb.WriteData(pNode->pData, pNode->size);
			pNode = oc.Next();
		}
	}
	m_replyLock.Lock();
	m_replyQueue[1].Push(m_wb.Buffer(), m_wb.Size());
	m_replyLock.Unlock();
}

}
}
