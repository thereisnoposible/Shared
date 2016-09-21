#pragma once
#pragma pack(1)
#include <stdint.h>
#include <memory>
#include "Helper.h"
struct NetConnect;
typedef struct tagPackHead
{
	static const short PACK_VER = 1;				/** 协议版本 */
	static const unsigned char PACK_HFLAG = 0x5A;	/** 开始标识 */
	static const unsigned char PACK_EFLAG = 0xA5;	/** 结束标识 */
	tagPackHead()
	{
		memset(this, 0, sizeof(tagPackHead));
	}

	unsigned char begflag;		//协议头开始标识,0x5A
	short version;				//协议版本
	long messageid;				//消息id
	long roleid;				//角色id
	unsigned short datasize;		//协议体长度
	unsigned char endflag;		// 协议头结束标识,0x5B
}PackHead, *PPackHead;

#pragma pack()
#include <string>

class NetPack
{
public:
	NetPack(){};
	NetPack(long messageid, const char* pdata, long len, long roleid,int type)
	{
		m_Head.begflag = PackHead::PACK_HFLAG;
		m_Head.endflag = PackHead::PACK_EFLAG;
		m_Head.version = PackHead::PACK_VER;
		m_Head.messageid = messageid;
		m_Head.roleid = roleid;
		m_Head.datasize = static_cast<unsigned short>(len);
		//if (type == 2)
		{
			m_pBuff.assign(reinterpret_cast<char*>(&m_Head), sizeof(PackHead));
		}
		std::string temp;
		temp.assign(pdata, len);
		//temp = Helper::Gb2312ToUTF8(temp);
		m_pBuff.append(temp);
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

	PackHead m_Head;								/** 协议头 */
	std::string m_pBuff;							/** 协议体 */
	std::string m_address;							/** 网络包来源地址,ip+port */
	std::shared_ptr<NetConnect> m_Connect;		
};