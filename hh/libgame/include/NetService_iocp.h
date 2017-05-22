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
	* @brief		��������������
	* @param[in]	ip[xstring] : ����ip��ַ
	* @param[in]	port[int32] : �����˿ں�
	*/
	bool Connect(const xstring& ip, int32 port);
	bool Connect2(const xstring& ip, int32 port);

	PIOContext AllocateContext();
	void ReleaseContext(PIOContext pContext);

	/**
	* @brief		���յ�һ���µ�Э���
	*/
	void ReceivePack(NetPackPtr& pPack);

	/**
	* @brief		֡������
	* @details		ÿ֡����
	* @return		void
	* @remarks
	*/
	void Update();

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

	bool Send(int32 messageid, const char* pdata, int32 length, const xstring& addr, int32 roleid = 0);


	template<typename T>
	bool Send(uint32 mesageid, const T& t, const xstring& addr, int32 roleid = 0)
	{
		return Send(mesageid, (const char *)&t, sizeof(T), addr, roleid);
	}

	bool SendProtoBuf(int32 messageid, const  ::google::protobuf::Message & proto, const xstring& addr, int32 roleid=0);

protected:
	/**
	* @brief		�����߳�
	*/
	void ListenThreadProc();

	/**
	* @brief		IO�����߳�
	*/
	void IOThreadProc();

	/**
	* @brief		����IO�߳�
	*/
	bool SetupIOWorkers();

	/**
	* @brief		���յ��µ�socket
	*/
	void onNewSocket(NetConnectPtr& pConnect);

	void onDisconnectSocket(NetConnectPtr& pConnect);

	/**
	* @brief		��¼��������
	*/
	void recordClientConnect(NetConnectPtr& pConnect);

	/**
	* @brief		ͨ��SOCKET�ҵ�Connect����
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

	IOContextCollect _IOContextCollect;		/**< �ڴ�� */

	boost::mutex _contextMutex;				/**< �ڴ�ؿռ以����� */

	boost::mutex m_PackMutex;				/**< ������� */
	NetPackCollect			m_PackCollect;			/** δ����Э������� */

	boost::mutex _connectMutex;
	ConnectIDMap _allConnectIDMap;			/**< ����ID������ */
	ConnectAddrMap _allConnectAddrMap;		/**< ���ӵ�ַ������ */

	NetObserverCollect		m_NetObservers;	/** ����۲��߶��� */

	typedef xvector<xstring> NetRecordCollect;
	NetRecordCollect		m_NewConnectCollect;	/** �����Ӷ��� */
	NetRecordCollect		m_CloseConnectCollect;	/** �Ͽ������б� */
};

