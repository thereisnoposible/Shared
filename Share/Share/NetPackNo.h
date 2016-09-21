#pragma once
#pragma pack(1)
#include <stdint.h>
typedef struct tagPackHeadNo
{
	static const short PACK_VER = 0;		/** Э��汾 */
	static const char PACK_HFLAG = 0x5A;	/** ��ʼ��ʶ */
	static const char PACK_EFLAG = 0xa5;	/** ������ʶ */
	tagPackHeadNo()
	{
		memset(this, 0, sizeof(tagPackHead));
	}

	char begflag;				//Э��ͷ��ʼ��ʶ,0x5A
	short version;				//Э��汾
	long messageid;				//��Ϣid
	long roleid;				//��ɫid
	short datasize;				//Э���峤��,����󳤶Ȳ�����64k
	char endflag;				// Э��ͷ������ʶ,0xa5
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

	PackHeadNo m_Head;								/** Э��ͷ */
	std::string m_pBuff;							/** Э���� */

	std::string m_address;							/** �������Դ��ַ,ip+port */
};