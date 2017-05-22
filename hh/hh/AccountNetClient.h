#pragma once 
#include "Initialer.h"
class AccountNetClient :public NetClient, public Singleton<AccountNetClient>, public Initialer
{
public:
	AccountNetClient(TimerManager* ptimer);
    void registMessage();
    void Initial();
    void OnConnect(ConnectPtr&);

    void CreateAccount(ConnectPtr& pConnect, pm_createaccount& request);
    void CreateAccountResponse(PackPtr& pPack);

    void AccountCheck(ConnectPtr& pConnect, pm_account_check& request);
    void AccountCheckResponse(PackPtr& pPack);

	void SendProtoBuf(int32 messageid, const ::google::protobuf::Message &proto);

private:
    std::unordered_map<std::string, ConnectPtr> m_Creating;
    std::unordered_map<unsigned int, ConnectPtr> m_pLogin;

    unsigned int m_LoginGameIncreament;
};