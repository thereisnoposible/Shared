#pragma once 
#include "Initialer.h"
class DBNetClient :public NetClient, public Singleton<DBNetClient>, public Initialer
{
public:
	DBNetClient(TimerManager* ptimer);
    void registMessage();
    void Initial();
    void OnConnect(ConnectPtr&);

	void SendProtoBuf(int32 messageid, const ::google::protobuf::Message &proto);

protected:
    void PlayerResponse(PackPtr& pPack);
	void ObjectResponse(PackPtr& pPack);
private:
};