/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		NETCONNECT.H
** @brief		�������ӷ�װ
**  
** Details	
**  
** @version	1.0
** @author		yuyi
** @date		2014/7/18 16:33
**  
** �޶�˵����	����汾 
** 
****************************************************************************/

#pragma once

#include "NetPack.h"


/**
* @brief		����������
* @details		
* @author		yuyi
* @date			2014/7/18 16:33
* @note			�� 
*/ 
class NetConnect : public boost::enable_shared_from_this<NetConnect>
{
	friend  class  NetService;

public:
	/**
	* @brief		��������
	* @remarks		�����ⲿ�ͷ�
	*/
	~NetConnect(void);

	/**
	* @brief		�������ӵ�ַ
	* @param[in]	ipport[xstring] : ��ip:�˿ڡ�
	* @return		void
	* @remarks		
	*/
	void SetAddress(xstring addr);


	/**
	* @brief		��ȡ���ӵ�ipport
	* @return		xstring : "ip:port"
	* @remarks		
	*/
	xstring GetAddress() { return m_Address; }

	/**
	* @brief		��ȡsocket����
	* @details		
	* @return		boost::asio::ip::tcp::socket
	* @remarks		
	*/
	boost::asio::ip::tcp::socket& getSocket(){ return m_Socket; }

	/**
	* @brief		��ȡ��������
	* @return		bool : true=�ɹ� false=ʧ��
	* @remarks		
	*/
	bool Read();


	/**
	* @brief		���������
	* @param[in]	messageid[int32] : ��ϢID
	* @param[in]	pdata[char*] : ���ͻ���
	* @param[in]	length[int32] : ���ͳ���
	* @param[in]	roleid[int32] : �����߽�ɫID��0����ϵͳ��Ϣ
	* @return		bool : true=�ɹ� false=ʧ��
	* @remarks		
	*/
	bool Send(int32 messageid,const char* pdata,int32 length,uint32 roleid);


protected:
	/**
	* @brief		���캯��
	* @param[in]	
	* @remarks		�����ⲿ����
	*/
	NetConnect(boost::asio::io_service& ioservice,bool client=false);



	/**
	* @brief		�ر�����
	*/
	void close();


	/**
	* @brief		���õ�ַ
	* @details		�������Ӷ��󣬴�socket��ȡ���ӵ�ַ
	* @return		void
	* @remarks		
	*/
	void SetAddress();


	/**
	* @brief		����Э��ͷ
	*/
	void handleHead(const boost::system::error_code& error, size_t len);


	/**
	* @brief		����Э����	
	*/
	void handleBody(const boost::system::error_code& error, size_t len);


	/**
	* @brief		д��ص�
	*/
	void handWrite(const boost::system::error_code& e);


	/**
	* @brief		������
	*/
	void doSend();


private:

	NetPackPtr m_recvBuff;			/** ���ջ��� */

	NetPackPtr m_sendPack;			/** ���ͻ��� */

	boost::asio::ip::tcp::socket  m_Socket;		/** ���Ӷ��� */

	xstring m_Address;				/** �����ַ */

	bool m_bDisconnect;				/** �Ƿ��Ѵ����رղ��� */


	NetPackCollect m_SendPackCollect;	/** ���Ͷ��� */
	boost::mutex m_SendMutex;			/** ���Ͷ��л��Ᵽ������ */

	bool m_bSending;					/** �Ƿ����ڷ��� */

	bool _bClient;						/**< �Ƿ��ǿͻ��˷�ʽ */
};


typedef boost::shared_ptr<NetConnect> NetConnectPtr;
typedef xmap<xstring,NetConnectPtr> NetConnectMap;		//key=ip:port|value=NetConnectPtr


typedef xvector<NetConnectPtr> NetConnectCollect;

typedef xvector<xstring> NetRecordCollect;


