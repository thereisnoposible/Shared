#pragma once

#include "IOContext.h"
#include "NetPack.h"

enum RecvStatus
{
	recvHead = 0,		/**< ��ȡЭ��ͷ */
	recvBody,			/**< ��ȡЭ���� */
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
	* @brief		�������ӵ�ַ
	*/
	void SetAddress(const xstring& addr);

	/**
	* @brief		��õ�ַ
	*/
	xstring& GetAddress();

	/**
	* @brief		��������׽���
	*/
	SOCKET GetSocket();

	/**
	* @brief		���������
	* @param[in]	messageid[int32] : ��ϢID
	* @param[in]	pdata[char*] : ���ͻ���
	* @param[in]	length[int32] : ���ͳ���
	* @param[in]	roleid[int32] : �����߽�ɫID��0����ϵͳ��Ϣ
	* @return		bool : true=�ɹ� false=ʧ��
	* @remarks
	*/
	bool Send(int32 messageid, const char* pdata, int32 length, uint32 roleid);

	void close();

protected:
	void onRead(char* pBuff, int32 len);
	void onWrite();

	/**
	* @brief		���ʹ���
	*/
	void doSend();

private:
	xstring _addr;
	SOCKET _socket;

	

	boost::mutex m_SendMutex;			/** ���Ͷ��л��Ᵽ������ */
	NetPackCollect m_SendPackCollect;	/** ���Ͷ��� */

	bool m_bSending;					/** �Ƿ����ڷ��� */

	NetPackPtr _readBuff;

	RecvStatus _channelStatus;

	int32 _needLen;					/**< ��Ҫ��ȡ�ĳ��� */
	int32 _recvLen;						/**< �Ѷ����� */
};

typedef boost::shared_ptr<NetConnect> NetConnectPtr;
typedef xmap<xstring, NetConnectPtr> ConnectAddrMap;
typedef xmap<SOCKET, NetConnectPtr> ConnectIDMap;