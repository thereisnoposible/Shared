#pragma once
#pragma pack(1)
#include <stdint.h>
typedef struct tagPackHeadNo
{
	static const short PACK_VER = 0;		/** 协议版本 */
	static const char PACK_HFLAG = 0x5A;	/** 开始标识 */
	static const char PACK_EFLAG = 0xa5;	/** 结束标识 */
	tagPackHeadNo()
	{
		memset(this, 0, sizeof(tagPackHead));
	}

	char begflag;				//协议头开始标识,0x5A
	short version;				//协议版本
	long messageid;				//消息id
	long roleid;				//角色id
	short datasize;				//协议体长度,包最大长度不超过64k
	char endflag;				// 协议头结束标识,0xa5
}PackHeadNo, *PPackHeadNo;

#pragma pack()
#include <string>

class NetPackNo
{
public:
	NetPackNo(){};
	NetPackNo(long messageid, const char* pdata, long len, long roleid)
	{
		m_Head.begflag = PackHeadNo::PACK_HFLAG;
		m_Head.endflag = PackHeadNo::PACK_EFLAG;
		m_Head.version = PackHeadNo::PACK_VER;
		m_Head.messageid = messageid;
		m_Head.roleid = roleid;
		m_Head.datasize = static_cast<short>(len);
		m_pBuff = pdata;		
	}

	std::string getBuffer()
	{
		return m_pBuff;
	}

	int getBufferSize()
	{
		return m_Head.datasize;
	}

	int getRoid()
	{
		return m_Head.roleid;
	}

	int getMessageId()
	{
		return m_Head.messageid;
	}

	std::string getAddr()
	{
		return m_address;
	}

	void setAddr(std::string addr)
	{
		m_address = addr;
	}

	PackHeadNo m_Head;								/** 协议头 */
	std::string m_pBuff;							/** 协议体 */

	std::string m_address;							/** 网络包来源地址,ip+port */
};