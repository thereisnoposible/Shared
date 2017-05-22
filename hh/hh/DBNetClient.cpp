#include "stdafx.h"
#include "Application.h"
#include "DBNetClient.h"
#include "PlayerMgr.h"
#include "ObjectModule.h"

DBNetClient* Singleton<DBNetClient>::single = nullptr;

DBNetClient::DBNetClient(TimerManager* ptimer) :NetClient(ptimer)
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

	std::string buf;
	request.SerializePartialToString(&buf);

	pConnect->Send(GM_REQUEST_PLAYER, buf.c_str(), (int32)buf.length(), 0);
}

//----------------------------------------------------------------
void DBNetClient::SendProtoBuf(int32 messageid, const  ::google::protobuf::Message & proto)
{
	std::string buf;
	proto.SerializePartialToString(&buf);

	Send(messageid, buf.c_str(), (int32)buf.length());
}


void DBNetClient::PlayerResponse(PackPtr& pPack)
{
    pm_playerdata_db_response response;
    CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer(), pPack->getBufferSize()));

    PlayerManager::getInstance().loadPlayer(response);

    SetInit();
}

void DBNetClient::ObjectResponse(PackPtr& pPack)
{
	pm_object_data_response_db response;
	CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer(), pPack->getBufferSize()));

	Player* player = PlayerManager::getInstance().GetPlayer(response.playerid());
	if (!player)
		return;

	ObjectModule* pModule = dynamic_cast<ObjectModule*>(player->GetModule(OBJECT, true));
	pModule->OnDataResponse(response);
}