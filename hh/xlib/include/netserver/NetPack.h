#pragma once
#pragma pack(1)
#include <stdint.h>
#include <memory>
#include <boost/shared_ptr.hpp>
#include "../include/helper/Helper.h"
#include <string>

#pragma warning (push)

#pragma warning (disable: 4251)

namespace xlib
{
	struct NetConnect;
	class WebSocketProtocol;

	typedef struct XDLL tagPackHead
	{
		static const short PACK_VER = 1;				/** Э��汾 */
		static const unsigned char PACK_HFLAG = 0x5A;	/** ��ʼ��ʶ */
		static const unsigned char PACK_EFLAG = 0xA5;	/** ������ʶ */
		tagPackHead()
		{
			memset(this, 0, sizeof(tagPackHead));
		}

		unsigned char begflag;		//Э��ͷ��ʼ��ʶ,0x5A
		short version;				//Э��汾
		long messageid;				//��Ϣid
		long roleid;				//��ɫid
		unsigned short datasize;		//Э���峤��
		unsigned char endflag;		// Э��ͷ������ʶ,0x5B
	}PackHead, *PPackHead;

#pragma pack()

	class XDLL NetPack
	{
	public:
		friend class WebSocketProtocol;
		friend struct NetConnect;

		NetPack(){};
		NetPack(long messageid, const char* pdata, long len, long roleid, int type)
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

		const char* getBuffer()
		{
			return m_pBuff.c_str();
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

		const char* getAddr()
		{
			return m_address.c_str();
		}

		void setAddr(const char* addr)
		{
			m_address = addr;
		}

		boost::shared_ptr<NetConnect> GetConnect()
		{
			return m_Connect;
		}

	private:
		PackHead m_Head;								/** Э��ͷ */
		std::string m_pBuff;							/** Э���� */
		std::string m_address;							/** �������Դ��ַ,ip+port */
		boost::shared_ptr<NetConnect> m_Connect;
	};
}

#pragma warning (pop)