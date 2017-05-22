#include "pch.h"
#include "NetPack.h"


const int16 NetPack::PACK_VER = 0;	/** 协议版本 */
const int8 NetPack::PACK_HFLAG = (int8)0x5A;	/** 开始标识 */
const int8 NetPack::PACK_EFLAG = (int8)0xA5;	/** 结束标识 */

//------------------------------------------------------------------
NetPack::NetPack(void) 
	: m_pBuff(NULL)
{
	
}


//------------------------------------------------------------------
NetPack::NetPack(int32 messageid,const char* pdata,int32 len,int32 roleid)
{
	m_pBuff = new char[len+sizeof(PackHead)];
	memcpy(m_pBuff+sizeof(PackHead),pdata,len);

	PackHead* pHead = reinterpret_cast<PackHead*>(m_pBuff);
	pHead->datasize = len;
	pHead->version = PACK_VER;
	pHead->begflag = PACK_HFLAG;
	pHead->endflag = PACK_EFLAG;
	pHead->messageid = messageid;
	pHead->roleid = roleid;

	m_Head = *pHead;
}


//------------------------------------------------------------------
NetPack::~NetPack(void)
{
	if (m_pBuff != NULL)
	{
		delete[] m_pBuff;
		m_pBuff = NULL;
	}
}


//------------------------------------------------------------------
void NetPack::InitDataBuff()
{
	if (m_pBuff != NULL)
	{
		delete[] m_pBuff;
		m_pBuff = NULL;
	}

	m_pBuff = new char[m_Head.datasize+sizeof(PackHead)];

	PackHead* pHead = reinterpret_cast<PackHead*>(m_pBuff);
	*pHead = m_Head;	//协议头拷贝到缓存中，方便转发协议
}


char* NetPack::GetHeadBuff()
{
	return (char*)&m_Head;
}


//------------------------------------------------------------------
int32 NetPack::GetHeadSize()
{
	return sizeof(PackHead);
}


//------------------------------------------------------------------
bool NetPack::VerifyHeader()
{
	if(PACK_VER==m_Head.version && PACK_HFLAG==m_Head.begflag && PACK_EFLAG==m_Head.endflag)
		return true;

	return false;
}


//------------------------------------------------------------------
void NetPack::SetAddress(const xstring& addr)
{
	m_address = addr;
}


//------------------------------------------------------------------
char* NetPack::GetData()
{
	return m_pBuff+sizeof(PackHead);
}


//------------------------------------------------------------------
int32 NetPack::GetDataSize()
{
	PackHead* pHead = reinterpret_cast<PackHead*>(m_pBuff);
	return pHead->datasize;
}


char* NetPack::GetBuff()
{
	return m_pBuff;
}


int32 NetPack::GetBuffSize()
{
	PackHead* pHead = reinterpret_cast<PackHead*>(m_pBuff);
	return pHead->datasize + sizeof(PackHead);
}


//------------------------------------------------------------------
int32 NetPack::GetMessageID()
{
	PackHead* pHead = reinterpret_cast<PackHead*>(m_pBuff);

	return pHead->messageid;
}


//------------------------------------------------------------------
int32 NetPack::GetRoleID()
{
	PackHead* pHead = reinterpret_cast<PackHead*>(m_pBuff);

	return pHead->roleid;
}


//------------------------------------------------------------------
void NetPack::SetRoleID(int32 roleid)
{
	PackHead* pHead = reinterpret_cast<PackHead*>(m_pBuff);

	pHead->roleid = roleid;
}


//------------------------------------------------------------------
xstring NetPack::GetAddress()
{
	return m_address;
}