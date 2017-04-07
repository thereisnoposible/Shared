#include "stdafx.h"
#include "Application.h"
#include "DBNetClient.h"
#include "PlayerMgr.h"

DBNetClient* Singleton<DBNetClient>::single = nullptr;

DBNetClient::DBNetClient()
{

}

void DBNetClient::registMessage()
{
    RegisterMessage(GM_REQUEST_PLAYER_RESPONSE, boost::bind(&DBNetClient::PlayerResponse, this, _1));
	RegisterMessage(GM_OBJECT_DATA_RESPONSE_DB, boost::bind(&DBNetClient::ObjectResponse, this, _1));
}

void DBNetClient::Initial()
{
    ConnectTo(DATEBASEADDR, DATEBASEPORT);
}

void DBNetClient::OnConnect(ConnectPtr& pConnect)
{
    NetClient::OnConnect(pConnect);
    pm_request_playerdata_db request;
    request.set_nothing(0);
    pConnect->SendBuffer(GM_REQUEST_PLAYER, request, 0);
}

void DBNetClient::PlayerResponse(PackPtr& pPack)
{
    pm_playerdata_db_response response;
    CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

    PlayerManager::getInstance().loadPlayer(response);

    SetInit();
}

void DBNetClient::ObjectResponse(PackPtr& pPack)
{
	pm_playerdata_db_response response;
	CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	PlayerManager::getInstance().loadPlayer(response);
}