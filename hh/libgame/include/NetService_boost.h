/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		NETSERVICE.H
** @brief		�����װ
**  
** Details	
**  
** @version	1.0
** @date		2014/7/18 13:36
**  
** �޶�˵����	����汾 
** 
****************************************************************************/


#pragma once


#include "Singleton.h"
#include "io_service_pool.h"
#include "NetConnect.h"
#include "NetPack.h"
#include "MessageHandle.h"
#include "google/protobuf/message.h"



interface NetObserver
{
	virtual void onConnect(const xstring& addr) = 0;
	virtual void onDisconnect(const xstring& addr) = 0;
};


typedef xset<NetObserver*> NetObserverCollect;


/**
* @brief		�������
* @details		���������׽��֣���װ�������
* @date			2014/7/18 13:35
* @note			�� 
*/ 
class XClass NetService : public Singleton<NetService>, public MessageHandle<NetPackPtr>
{
public:
	NetService(int32 iothreadnum);
	~NetService(void);

public:
	//�ͷ���
	//class deletor 
	//{
	//public:
	//	void operator()(NetConnect* p)
	//	{
	//		SafeDelete(p);
	//	}
	//};

	/**
	* @brief		�����������
	* @details		�����������ȴ�����
	* @param[in]	nPort[int32] : �˿ں�
	* @return		bool : true=�ɹ� false=ʧ��
	* @remarks		
	*/
	bool Start(int32 nPort);


	/**
	* @brief		���յ��������籨
	* @param[in]	pPack[NetPackPtr] : ���������
	* @return		void
	* @remarks		
	*/
	void ReceivePack(NetPackPtr pPack);


	/**
	* @brief		֡������
	* @details		ÿ֡����
	* @return		void
	* @remarks		
	*/
	void Update();


	/**
	* @brief		��������������
	* @param[in]	ip[xstring] : ����ip��ַ
	* @param[in]	port[int32] : �����˿ں�
	* @return		
	* @remarks		
	*/
	bool Connect(const xstring& ip,int32 port);

	bool Connect2(const xstring& ip, int32 port);


	/**
	* @brief		����һ���۲���
	* @param[in]	pObserver[NetObserver*] : �۲��߶���ָ��
	* @return		void
	* @remarks		
	*/
	void RegisterObserver(NetObserver* pObserver);


	/**
	* @brief		��ע��һ���۲���
	* @param[in]	pObserver[NetObserver*] : �۲��߶���ָ��
	* @return		void
	* @remarks		
	*/
	void UnregisterObserver(NetObserver* pObserver);


	/**
	* @brief		ɾ��һ���۲���
	* @param[in]	pObserver[NetObserver*] : �۲��߶���ָ��
	* @return		void
	* @remarks		
	*/
	void RemoveObserver(NetObserver* pObserver);


	/**
	* @brief		�Ͽ�����
	* @details		
	* @param[in]	addr[xstring] : ip��ַ���˿�ƴ���ַ���
	* @return		bool : true=�ɹ� false=ʧ��
	* @remarks		
	*/
	bool Disconnect(const xstring& addr);


	/**
	* @brief		�����ѶϿ�����
	* @param[in]	ipport : ip��ַ���˿�ƴ���ַ���
	* @return		void
	* @remarks		
	*/
	void OnDisconnect(const xstring& addr);


	bool Send(int32 messageid,const char* pdata,int32 length,const xstring& addr,int32 roleid = 0);


	template<typename T>
	bool Send(uint32 mesageid ,const T& t,const xstring& addr, int32 roleid = 0)
	{
		return Send(mesageid,(const char *)&t,sizeof(T),addr,roleid);
	}


	bool SendProtoBuf(int32 messageid ,const ::google::protobuf::Message& pdata,const xstring& addr,int32 roleid = 0);


protected:
	/**
	* @brief		��������
	* @return		void
	* @remarks		
	*/
	void accept();


	/**
	* @brief		�����յ���������
	* @details		
	* @param[in]	pConnect[NetConnectPtr] : NetConnect����ָ��
	* @return		void
	* @remarks		
	*/
	void handleConnect(NetConnectPtr pConnect,const boost::system::error_code& e);

	void handleConnect2(NetConnectPtr pConnect, const boost::system::error_code& e);

private:
	io_service_pool		m_IOPool;					/** IO�̳߳� */

	boost::asio::ip::tcp::acceptor*		m_pAcceptor;			/** �������� */

	boost::mutex			m_ConnectMutex;			/** �������ӵĻ��Ᵽ������ */
	NetConnectMap			m_ConnectMap;			/** ����������ip:port��ɵ�map */
	NetRecordCollect		m_NewConnectCollect;	/** �����Ӷ��� */
	NetRecordCollect		m_CloseConnectCollect;	/** �Ͽ������б� */

	NetPackCollect			m_PackCollect;			/** δ����Э������� */
	boost::mutex			m_PackMutex;			/** ��������Ᵽ������ */

	NetObserverCollect		m_NetObservers;	/** ����۲��߶��� */

	bool m_bAccept;
};

