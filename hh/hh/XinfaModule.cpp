#include "stdafx.h"
#include "Application.h"
#include "Player.h"
#include "XinfaModule.h"

XinfaModule::XinfaModule(const std::string& name, Player* pOwner)
	:BaseModule(name, pOwner), m_Xinfa(NULL), tm_Handle(NULL)
{
	registerMessage();
}

XinfaModule::~XinfaModule()
{
	SAFE_DELETE(m_Xinfa);
}

bool XinfaModule::Init()
{
	return true;
}

void XinfaModule::registerMessage()
{
	m_pOwner->RegisterNetMsgHandle(GM_REQUEST_XINFA_RESPONSE, std::bind(&XinfaModule::requestXinfaResponse, this, std::placeholders::_1));

	m_pOwner->RegisterNetMsgHandle(GM_REQUEST_XINFA, std::bind(&XinfaModule::processRequestXinfa, this, std::placeholders::_1));
	m_pOwner->RegisterNetMsgHandle(GM_REQUEST_DAZUO, std::bind(&XinfaModule::processRequestDazuo, this, std::placeholders::_1));
	m_pOwner->RegisterNetMsgHandle(GM_REQUEST_TIAOXI, std::bind(&XinfaModule::processRequestTiaoxi, this, std::placeholders::_1));
}

void XinfaModule::unregisterMessage()
{
	
}

const Xinfa* XinfaModule::getXinfa(std::function<void()> callback)
{
	if (m_Xinfa == NULL)
	{
		mv_callback[CallBackTypeXINFA].push_back(callback);
		if (mv_callback[CallBackTypeXINFA].size() == 1)
		{
			pm_request_xinfa request;
			request.set_playerid(m_pOwner->GetPlayerID());

			Application::getInstance().GetDataBaseConnect().SendProtoBuf(GM_REQUEST_XINFA, request, m_pOwner->GetPlayerID());
		}
		return NULL;
	}

	return m_Xinfa;
}

void XinfaModule::processRequestXinfa(PackPtr& pPack)
{
	Xinfa* pTemp = const_cast<Xinfa*>(getXinfa(std::bind(&XinfaModule::processRequestXinfa, this, pPack)));
	if (pTemp == NULL)
		return;

	pm_request_xinfa_response response;
	response.set_result(0);
	pm_xinfa* pxinfa = new pm_xinfa;
	m_Xinfa->toPBMessage(*pxinfa);
	m_pOwner->SendProtoBuf(GM_REQUEST_XINFA_RESPONSE, response);
}

void XinfaModule::requestXinfaResponse(PackPtr& pPack)
{
	if (pPack->getAddr() != Application::getInstance().GetDataBaseConnect().getAddress() && m_Xinfa != NULL)
		return;
	pm_request_xinfa_response response;
	CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	
	m_Xinfa = new Xinfa();

	if (response.has_xinfa())
	{
		m_Xinfa->fromPBMessage(response.xinfa());
	}
	else
	{
		m_Xinfa->dbid = Application::getInstance().getDBID();
		m_Xinfa->playerid = m_pOwner->GetPlayerID();
		m_Xinfa->id = 1;
		m_Xinfa->level = 1;
		m_Xinfa->exp = 0;
	}

	for (uint32 i = 0; i < mv_callback[CallBackTypeXINFA].size(); i++)
	{
		mv_callback[CallBackTypeXINFA][i]();
	}
}

void XinfaModule::Dazuo()
{
	Xinfa* pTemp = const_cast<Xinfa*>(getXinfa(std::bind(&XinfaModule::Dazuo, this)));
	if (pTemp == NULL)
		return;

	const TypeXinfa* pTypeXinfa = TypeTable::getInstance().getTypeXinfa(pTemp->id);
	if (pTypeXinfa == NULL)
		return;

	int32 count = pTypeXinfa->add_ratio*pTemp->level + pTypeXinfa->add_count;
	count = count * m_pOwner->GetGengu() / 100;

	pTemp->exp += count;
}

void XinfaModule::processRequestDazuo(PackPtr& pPack)
{
	Xinfa* pTemp = const_cast<Xinfa*>(getXinfa(std::bind(&XinfaModule::processRequestDazuo, this, pPack)));
	if (pTemp == NULL)
		return;

	const TypeXinfa* pTypeXinfa = TypeTable::getInstance().getTypeXinfa(pTemp->id);
	if (pTypeXinfa == NULL)
		return;

	if (m_pOwner->GetPlayerStatus() != Player::psFree)
		return;

	if (tm_Handle != nullptr)
	{
		TimerManager::getInstance().RemoveTimer(tm_Handle);
	}

	m_pOwner->SetPlayerStatus(Player::psDazuo);

	tm_Handle = TimerManager::getInstance().AddIntervalTimer(60.0, std::bind(&XinfaModule::Dazuo, this));
}

void XinfaModule::processRequestTiaoxi(PackPtr& pPack)
{
	Xinfa* pTemp = const_cast<Xinfa*>(getXinfa(std::bind(&XinfaModule::processRequestTiaoxi, this, pPack)));
	if (pTemp == NULL)
		return;

	const TypeXinfa* pTypeXinfa = TypeTable::getInstance().getTypeXinfa(pTemp->id);
	if (pTypeXinfa == NULL)
		return;

	if (m_pOwner->GetPlayerStatus() != Player::psFree)
		return;

	if (tm_Handle != nullptr)
	{
		TimerManager::getInstance().RemoveTimer(tm_Handle);
	}

	m_pOwner->SetPlayerStatus(Player::psTiaoxi);
	tm_Handle = TimerManager::getInstance().AddIntervalTimer(60.0, std::bind(&XinfaModule::Tiaoxi, this));
}

void XinfaModule::Tiaoxi()
{
	Xinfa* pTemp = const_cast<Xinfa*>(getXinfa(std::bind(&XinfaModule::Tiaoxi, this)));
	if (pTemp == NULL)
		return;

	const TypeXinfa* pTypeXinfa = TypeTable::getInstance().getTypeXinfa(pTemp->id);
	if (pTypeXinfa == NULL)
		return;

	int32 count = pTypeXinfa->recovery_hp_ratio * pTemp->level + pTypeXinfa->recovery_hp_count;
	m_pOwner->AddHP(count);

	count = pTypeXinfa->recovery_mp_ratio * pTemp->level + pTypeXinfa->recovery_mp_count;

	pTemp->mp += count;
	if (pTemp->mp > pTemp->max_mp)
		pTemp->mp = pTemp->max_mp;
}