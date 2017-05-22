#pragma once


#pragma pack(1)
//不进行对齐
typedef struct tagPackHead
{
	tagPackHead()
	{
		memset(this,0,sizeof(tagPackHead));
	}

	int8 begflag;				//协议头开始标识,0x5A
	int16 version;				//协议版本
	int32 messageid;			//消息id
	int32 roleid;				//角色id
	uint16 datasize;				//协议体长度,包最大长度不超过64k
	int8 endflag;				// 协议头结束标识,0xA5
}PackHead,*PPackHead;

#pragma pack()


/**
* @brief		网络包封装
* @details		包头+包体
* @author		yuyi
* @date			2014/7/21 14:30
* @note			无
*/
class XClass NetPack
{
public:
	NetPack(void);
	NetPack(int32 messageid,const char* pdata,int32 len,int32 roleid);

	~NetPack(void);
public:
	/**
	* @brief		获取协议头缓存地址
	*/
	char* GetHeadBuff();


	/**
	* @brief		获取协议头长度
	*/
	int32 GetHeadSize();


	/**
	* @brief		验证协议头
	* @return		bool : true=有效 false=无效
	* @remarks
	*/
	bool VerifyHeader();


	/**
	* @brief		设置包体大小
	* @return		void
	* @remarks
	*/
	void InitDataBuff();


	/**
	* @brief		获取数据区域地址
	*/
	char* GetData();


	/**
	* @brief		获取协议体长度
	*/
	int32 GetDataSize();


	char* GetBuff();


	int32 GetBuffSize();


	/**
	* @brief		获取消息ID
	* @return		int32 : 消息ID
	*/
	int32 GetMessageID();


	/**
	* @brief		获取角色ID
	* @return		int32 : 角色ID
	* @remarks
	*/
	int32 GetRoleID();


	/**
	* @brief		设置包所属角色
	* @details
	* @param[in]	roleid[int32] : 包所属角色
	* @return		void
	* @remarks
	*/
	void SetRoleID(int32 roleid);


	/**
	* @brief		设置包的地址
	* @param[in]	addr[xstring&] : 包的来源地址
	* @return		void
	* @remarks
	*/
	void SetAddress(const xstring& addr);


	/**
	* @brief		获取包的来源地址
	* @return		xstring : ip:port
	* @remarks
	*/
	xstring GetAddress();

private:
	static const int16 PACK_VER;	/** 协议版本 */
	static const int8 PACK_HFLAG;	/** 开始标识 */
	static const int8 PACK_EFLAG;	/** 结束标识 */

	PackHead m_Head;				/** 协议头 */
	char* m_pBuff;					/** 协议体 */

	xstring m_address;				/** 网络包来源地址,ip+port */

};


typedef boost::shared_ptr<NetPack> NetPackPtr;
typedef xvector<NetPackPtr> NetPackCollect;

