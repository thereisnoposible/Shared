#pragma once

#include "IOContext.h"
#include "NetPack.h"

enum RecvStatus
{
	recvHead = 0,		/**< 读取协议头 */
	recvBody,			/**< 读取协议体 */
};

class NetService;

class NetConnect
{
public:
	friend class NetService;

	NetConnect(SOCKET sock);

	~NetConnect();

public:
	void StartRead();

	/**
	* @brief		设置连接地址
	*/
	void SetAddress(const xstring& addr);

	/**
	* @brief		获得地址
	*/
	xstring& GetAddress();

	/**
	* @brief		获得连接套接字
	*/
	SOCKET GetSocket();

	/**
	* @brief		发送网络包
	* @param[in]	messageid[int32] : 消息ID
	* @param[in]	pdata[char*] : 发送缓存
	* @param[in]	length[int32] : 发送长度
	* @param[in]	roleid[int32] : 发送者角色ID，0代表系统消息
	* @return		bool : true=成功 false=失败
	* @remarks
	*/
	bool Send(int32 messageid, const char* pdata, int32 length, uint32 roleid);

	void close();

protected:
	void onRead(char* pBuff, int32 len);
	void onWrite();

	/**
	* @brief		发送处理
	*/
	void doSend();

private:
	xstring _addr;
	SOCKET _socket;

	

	boost::mutex m_SendMutex;			/** 发送队列互斥保护对象 */
	NetPackCollect m_SendPackCollect;	/** 发送队列 */

	bool m_bSending;					/** 是否正在发送 */

	NetPackPtr _readBuff;

	RecvStatus _channelStatus;

	int32 _needLen;					/**< 需要读取的长度 */
	int32 _recvLen;						/**< 已读长度 */
};

typedef boost::shared_ptr<NetConnect> NetConnectPtr;
typedef xmap<xstring, NetConnectPtr> ConnectAddrMap;
typedef xmap<SOCKET, NetConnectPtr> ConnectIDMap;