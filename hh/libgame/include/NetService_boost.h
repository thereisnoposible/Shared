/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		NETSERVICE.H
** @brief		网络封装
**  
** Details	
**  
** @version	1.0
** @date		2014/7/18 13:36
**  
** 修订说明：	最初版本 
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
* @brief		网络服务
* @details		监听网络套接字，封装网络操作
* @date			2014/7/18 13:35
* @note			无 
*/ 
class XClass NetService : public Singleton<NetService>, public MessageHandle<NetPackPtr>
{
public:
	NetService(int32 iothreadnum);
	~NetService(void);

public:
	//释放器
	//class deletor 
	//{
	//public:
	//	void operator()(NetConnect* p)
	//	{
	//		SafeDelete(p);
	//	}
	//};

	/**
	* @brief		启动网络服务
	* @details		启动监听，等待连接
	* @param[in]	nPort[int32] : 端口号
	* @return		bool : true=成功 false=失败
	* @remarks		
	*/
	bool Start(int32 nPort);


	/**
	* @brief		接收到完整网络报
	* @param[in]	pPack[NetPackPtr] : 网络包对象
	* @return		void
	* @remarks		
	*/
	void ReceivePack(NetPackPtr pPack);


	/**
	* @brief		帧处理函数
	* @details		每帧调用
	* @return		void
	* @remarks		
	*/
	void Update();


	/**
	* @brief		连接其他服务器
	* @param[in]	ip[xstring] : 待连ip地址
	* @param[in]	port[int32] : 待连端口号
	* @return		
	* @remarks		
	*/
	bool Connect(const xstring& ip,int32 port);

	bool Connect2(const xstring& ip, int32 port);


	/**
	* @brief		增加一个观察者
	* @param[in]	pObserver[NetObserver*] : 观察者对象指针
	* @return		void
	* @remarks		
	*/
	void RegisterObserver(NetObserver* pObserver);


	/**
	* @brief		解注册一个观察者
	* @param[in]	pObserver[NetObserver*] : 观察者对象指针
	* @return		void
	* @remarks		
	*/
	void UnregisterObserver(NetObserver* pObserver);


	/**
	* @brief		删除一个观察者
	* @param[in]	pObserver[NetObserver*] : 观察者对象指针
	* @return		void
	* @remarks		
	*/
	void RemoveObserver(NetObserver* pObserver);


	/**
	* @brief		断开连接
	* @details		
	* @param[in]	addr[xstring] : ip地址及端口拼接字符串
	* @return		bool : true=成功 false=失败
	* @remarks		
	*/
	bool Disconnect(const xstring& addr);


	/**
	* @brief		处理已断开连接
	* @param[in]	ipport : ip地址及端口拼接字符串
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
	* @brief		接收连接
	* @return		void
	* @remarks		
	*/
	void accept();


	/**
	* @brief		处理收到网络连接
	* @details		
	* @param[in]	pConnect[NetConnectPtr] : NetConnect对象指针
	* @return		void
	* @remarks		
	*/
	void handleConnect(NetConnectPtr pConnect,const boost::system::error_code& e);

	void handleConnect2(NetConnectPtr pConnect, const boost::system::error_code& e);

private:
	io_service_pool		m_IOPool;					/** IO线程池 */

	boost::asio::ip::tcp::acceptor*		m_pAcceptor;			/** 监听服务 */

	boost::mutex			m_ConnectMutex;			/** 管理连接的互斥保护对象 */
	NetConnectMap			m_ConnectMap;			/** 所有连接以ip:port组成的map */
	NetRecordCollect		m_NewConnectCollect;	/** 新连接队列 */
	NetRecordCollect		m_CloseConnectCollect;	/** 断开连接列表 */

	NetPackCollect			m_PackCollect;			/** 未处理协议包集合 */
	boost::mutex			m_PackMutex;			/** 网络包互斥保护对象 */

	NetObserverCollect		m_NetObservers;	/** 网络观察者队列 */

	bool m_bAccept;
};

