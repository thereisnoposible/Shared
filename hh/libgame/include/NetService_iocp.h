#pragma once

#include "IOContext.h"
#include "NetConnect.h"
#include "MessageHandle.h"

#include "google/protobuf/message.h"

interface NetObserver
{
	virtual void onConnect(const xstring& addr) = 0;
	virtual void onDisconnect(const xstring& addr) = 0;
};

typedef xset<NetObserver*> NetObserverCollect;

class XClass NetService : public Singleton<NetService>, public MessageHandle<NetPackPtr>
{
public:
	NetService(int32 iothreadnum);
	~NetService();

public:
	bool Start(int32 nPort);

	/**
	* @brief		连接其他服务器
	* @param[in]	ip[xstring] : 待连ip地址
	* @param[in]	port[int32] : 待连端口号
	*/
	bool Connect(const xstring& ip, int32 port);
	bool Connect2(const xstring& ip, int32 port);

	PIOContext AllocateContext();
	void ReleaseContext(PIOContext pContext);

	/**
	* @brief		接收到一个新的协议包
	*/
	void ReceivePack(NetPackPtr& pPack);

	/**
	* @brief		帧处理函数
	* @details		每帧调用
	* @return		void
	* @remarks
	*/
	void Update();

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

	bool Send(int32 messageid, const char* pdata, int32 length, const xstring& addr, int32 roleid = 0);


	template<typename T>
	bool Send(uint32 mesageid, const T& t, const xstring& addr, int32 roleid = 0)
	{
		return Send(mesageid, (const char *)&t, sizeof(T), addr, roleid);
	}

	bool SendProtoBuf(int32 messageid, const  ::google::protobuf::Message & proto, const xstring& addr, int32 roleid=0);

protected:
	/**
	* @brief		监听线程
	*/
	void ListenThreadProc();

	/**
	* @brief		IO工作线程
	*/
	void IOThreadProc();

	/**
	* @brief		创建IO线程
	*/
	bool SetupIOWorkers();

	/**
	* @brief		接收到新的socket
	*/
	void onNewSocket(NetConnectPtr& pConnect);

	void onDisconnectSocket(NetConnectPtr& pConnect);

	/**
	* @brief		记录主动连接
	*/
	void recordClientConnect(NetConnectPtr& pConnect);

	/**
	* @brief		通过SOCKET找到Connect对象
	*/
	NetConnectPtr getConnectBy(SOCKET sock);

	void setupListenWorker(int32 port);
private:
	int32 _io_thread_num;

	HANDLE _hIOCompletionPort;

	std::vector<boost::shared_ptr<boost::thread> > _threads;

	SOCKET _sockListen ;

	HANDLE _hEvent;

	bool _bShutDown;

	IOContextCollect _IOContextCollect;		/**< 内存池 */

	boost::mutex _contextMutex;				/**< 内存池空间互斥对象 */

	boost::mutex m_PackMutex;				/**< 网络包锁 */
	NetPackCollect			m_PackCollect;			/** 未处理协议包集合 */

	boost::mutex _connectMutex;
	ConnectIDMap _allConnectIDMap;			/**< 连接ID索引表 */
	ConnectAddrMap _allConnectAddrMap;		/**< 连接地址索引表 */

	NetObserverCollect		m_NetObservers;	/** 网络观察者队列 */

	typedef xvector<xstring> NetRecordCollect;
	NetRecordCollect		m_NewConnectCollect;	/** 新连接队列 */
	NetRecordCollect		m_CloseConnectCollect;	/** 断开连接列表 */
};

