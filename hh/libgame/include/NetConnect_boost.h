/****************************************************************************
**       
** Copyright (C) 2014, All right reserved.
** 
** @file		NETCONNECT.H
** @brief		网络连接封装
**  
** Details	
**  
** @version	1.0
** @author		yuyi
** @date		2014/7/18 16:33
**  
** 修订说明：	最初版本 
** 
****************************************************************************/

#pragma once

#include "NetPack.h"


/**
* @brief		网络连接类
* @details		
* @author		yuyi
* @date			2014/7/18 16:33
* @note			无 
*/ 
class NetConnect : public boost::enable_shared_from_this<NetConnect>
{
	friend  class  NetService;

public:
	/**
	* @brief		析构函数
	* @remarks		不能外部释放
	*/
	~NetConnect(void);

	/**
	* @brief		设置连接地址
	* @param[in]	ipport[xstring] : “ip:端口”
	* @return		void
	* @remarks		
	*/
	void SetAddress(xstring addr);


	/**
	* @brief		获取连接的ipport
	* @return		xstring : "ip:port"
	* @remarks		
	*/
	xstring GetAddress() { return m_Address; }

	/**
	* @brief		获取socket对象
	* @details		
	* @return		boost::asio::ip::tcp::socket
	* @remarks		
	*/
	boost::asio::ip::tcp::socket& getSocket(){ return m_Socket; }

	/**
	* @brief		读取网络数据
	* @return		bool : true=成功 false=失败
	* @remarks		
	*/
	bool Read();


	/**
	* @brief		发送网络包
	* @param[in]	messageid[int32] : 消息ID
	* @param[in]	pdata[char*] : 发送缓存
	* @param[in]	length[int32] : 发送长度
	* @param[in]	roleid[int32] : 发送者角色ID，0代表系统消息
	* @return		bool : true=成功 false=失败
	* @remarks		
	*/
	bool Send(int32 messageid,const char* pdata,int32 length,uint32 roleid);


protected:
	/**
	* @brief		构造函数
	* @param[in]	
	* @remarks		不能外部构造
	*/
	NetConnect(boost::asio::io_service& ioservice,bool client=false);



	/**
	* @brief		关闭连接
	*/
	void close();


	/**
	* @brief		设置地址
	* @details		被动连接对象，从socket获取连接地址
	* @return		void
	* @remarks		
	*/
	void SetAddress();


	/**
	* @brief		处理协议头
	*/
	void handleHead(const boost::system::error_code& error, size_t len);


	/**
	* @brief		处理协议体	
	*/
	void handleBody(const boost::system::error_code& error, size_t len);


	/**
	* @brief		写入回调
	*/
	void handWrite(const boost::system::error_code& e);


	/**
	* @brief		处理发送
	*/
	void doSend();


private:

	NetPackPtr m_recvBuff;			/** 接收缓存 */

	NetPackPtr m_sendPack;			/** 发送缓存 */

	boost::asio::ip::tcp::socket  m_Socket;		/** 连接对象 */

	xstring m_Address;				/** 网络地址 */

	bool m_bDisconnect;				/** 是否已触发关闭操作 */


	NetPackCollect m_SendPackCollect;	/** 发送队列 */
	boost::mutex m_SendMutex;			/** 发送队列互斥保护对象 */

	bool m_bSending;					/** 是否正在发送 */

	bool _bClient;						/**< 是否是客户端方式 */
};


typedef boost::shared_ptr<NetConnect> NetConnectPtr;
typedef xmap<xstring,NetConnectPtr> NetConnectMap;		//key=ip:port|value=NetConnectPtr


typedef xvector<NetConnectPtr> NetConnectCollect;

typedef xvector<xstring> NetRecordCollect;


