#pragma once


#pragma pack(1)
//�����ж���
typedef struct tagPackHead
{
	tagPackHead()
	{
		memset(this,0,sizeof(tagPackHead));
	}

	int8 begflag;				//Э��ͷ��ʼ��ʶ,0x5A
	int16 version;				//Э��汾
	int32 messageid;			//��Ϣid
	int32 roleid;				//��ɫid
	uint16 datasize;				//Э���峤��,����󳤶Ȳ�����64k
	int8 endflag;				// Э��ͷ������ʶ,0xA5
}PackHead,*PPackHead;

#pragma pack()


/**
* @brief		�������װ
* @details		��ͷ+����
* @author		yuyi
* @date			2014/7/21 14:30
* @note			��
*/
class XClass NetPack
{
public:
	NetPack(void);
	NetPack(int32 messageid,const char* pdata,int32 len,int32 roleid);

	~NetPack(void);
public:
	/**
	* @brief		��ȡЭ��ͷ�����ַ
	*/
	char* GetHeadBuff();


	/**
	* @brief		��ȡЭ��ͷ����
	*/
	int32 GetHeadSize();


	/**
	* @brief		��֤Э��ͷ
	* @return		bool : true=��Ч false=��Ч
	* @remarks
	*/
	bool VerifyHeader();


	/**
	* @brief		���ð����С
	* @return		void
	* @remarks
	*/
	void InitDataBuff();


	/**
	* @brief		��ȡ���������ַ
	*/
	char* GetData();


	/**
	* @brief		��ȡЭ���峤��
	*/
	int32 GetDataSize();


	char* GetBuff();


	int32 GetBuffSize();


	/**
	* @brief		��ȡ��ϢID
	* @return		int32 : ��ϢID
	*/
	int32 GetMessageID();


	/**
	* @brief		��ȡ��ɫID
	* @return		int32 : ��ɫID
	* @remarks
	*/
	int32 GetRoleID();


	/**
	* @brief		���ð�������ɫ
	* @details
	* @param[in]	roleid[int32] : ��������ɫ
	* @return		void
	* @remarks
	*/
	void SetRoleID(int32 roleid);


	/**
	* @brief		���ð��ĵ�ַ
	* @param[in]	addr[xstring&] : ������Դ��ַ
	* @return		void
	* @remarks
	*/
	void SetAddress(const xstring& addr);


	/**
	* @brief		��ȡ������Դ��ַ
	* @return		xstring : ip:port
	* @remarks
	*/
	xstring GetAddress();

private:
	static const int16 PACK_VER;	/** Э��汾 */
	static const int8 PACK_HFLAG;	/** ��ʼ��ʶ */
	static const int8 PACK_EFLAG;	/** ������ʶ */

	PackHead m_Head;				/** Э��ͷ */
	char* m_pBuff;					/** Э���� */

	xstring m_address;				/** �������Դ��ַ,ip+port */

};


typedef boost::shared_ptr<NetPack> NetPackPtr;
typedef xvector<NetPackPtr> NetPackCollect;

