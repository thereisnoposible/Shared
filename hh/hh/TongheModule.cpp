#include "stdafx.h"
#include "TongheModule.h"
#include "Player.h"
#include "WugongModule.h"
#include "MapManager.h"
#include "Cell.h"

#include "../new/proto/protobuf/wugong.pb.h"
#include "../new/proto/protobuf/tonghe.pb.h"

//-------------------------------------------------------------------------------------------
TongheModule::TongheModule(std::string modulename, Player* pOwner) : BaseModule(modulename, pOwner)
{
	registmessage();
}

//-------------------------------------------------------------------------------------------
TongheModule::~TongheModule()
{

}

//-------------------------------------------------------------------------------------------
void TongheModule::registmessage()
{
	m_pOwner->RegisterNetMsgHandle(GM_REQUEST_MOVE, boost::bind(&TongheModule::processMove, this, _1));

}

//-------------------------------------------------------------------------------------------
void TongheModule::unregistmessage()
{

}

//-------------------------------------------------------------------------------------------
bool TongheModule::Init()
{
	setInitOk();
	return true;
}

//-------------------------------------------------------------------------------------------
void TongheModule::xuexi(PackPtr& pPack)
{
	pm_request_xuexi request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	pm_xuexi_response response;
	response.set_result(0);
	for (int k = 0; k < 1; k++)
	{
		int wugongid = request.wugongid();
		const WugongType* pType = TypeTable::getInstance().GetWugongType(wugongid);
		if (pType == nullptr)
			break;

		WugongModule* pModule = dynamic_cast<WugongModule*>(m_pOwner->GetModule(WUGONG));
		int level = pModule->GetWugongLevel(wugongid);
		if (level == -1)
		{
			Wugong item;
			item.id = wugongid;
			item.type = pType->type;
			item.level = 0;
			item.exp = 0;
			pModule->Learning(item);
		}
		level = pModule->GetWugongLevel(wugongid);
		const WugongLevelUpType* pWuLevelType = TypeTable::getInstance().GetWugongLevelUpType(pType->quality, level);
		if (pWuLevelType == nullptr)
			break;
		if (pWuLevelType->needxuedian > m_pOwner->GetXuedian())
			break;

		m_pOwner->Addxuedian(-pWuLevelType->needxuedian);
		pModule->AddWugongLevel(wugongid);
	}

	m_pOwner->SendProtoBuf(1, response);

}


//-------------------------------------------------------------------------------------------
void TongheModule::processMove(PackPtr& pPack)
{
	pm_request_move request;
	int x;
	int y;
	if (request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()))
	{
		x = request.movetox();
		y = request.movetoy();
	}
	else
	{
		Json::Reader read;
		Json::Value value;
		read.parse(pPack->m_pBuff, value, false);

		if (value.isMember("x"))
			x = value["x"].asInt();
		if (value.isMember("y"))
			y = value["y"].asInt();
	}

	//m_pOwner->StartMove((unsigned int)x, (unsigned int)y);
	Cell* pCell = MapManager::getInstance().GetCell(m_pOwner->GetCellID());
	m_pOwner->dstpos.x = x;
	m_pOwner->dstpos.y = y;
	if (pCell != NULL)
	{
		pCell->OnRequestMove(m_pOwner);
	}

	MapManager::getInstance().BrocastPlayerMessage(m_pOwner, &Player::OnPlayerMove, m_pOwner);
}