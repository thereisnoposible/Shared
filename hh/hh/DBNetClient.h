#pragma once 
#include "Initialer.h"
class DBNetClient :public NetClient, public Singleton<DBNetClient>, public Initialer
{
public:
    DBNetClient();
    void registMessage();
    void Initial();
    void OnConnect(ConnectPtr&);

protected:
    void PlayerResponse(PackPtr& pPack);
private:
};