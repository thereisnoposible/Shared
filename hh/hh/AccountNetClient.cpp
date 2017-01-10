#include "stdafx.h"
#include "Application.h"
#include "AccountNetClient.h"
#include "PlayerMgr.h"

AccountNetClient* Singleton<AccountNetClient>::single = nullptr;

AccountNetClient::AccountNetClient()
{
    m_LoginGameIncreament = 0;
}

void AccountNetClient::registMessage()
{
    RegisterMessage(GM_CREATE_ACCOUNT_RESPONSE, boost::bind(&AccountNetClient::CreateAccountResponse, this, _1));
    RegisterMessage(GM_ACCOUNT_CHECK_RESPONSE, boost::bind(&AccountNetClient::AccountCheckResponse, this, _1));
}

void AccountNetClient::Initial()
{
    ConnectTo(ACCOUNTADDR, ACCOUNTPORT);
}

void AccountNetClient::OnConnect(ConnectPtr& pConnect)
{
    NetClient::OnConnect(pConnect);
    SetInit();
}

void AccountNetClient::CreateAccount(ConnectPtr& pConnect, pm_createaccount& request)
{
    std::unordered_map<std::string, ConnectPtr>::iterator it = m_Creating.find(request.username());

    m_Creating.insert(std::make_pair(request.username(), pConnect));
    SendProtoBuf(GM_CREATE_ACCOUNT, request);
}

//-------------------------------------------------------------------------------------------
void AccountNetClient::CreateAccountResponse(PackPtr& pPack)
{
    pm_createaccount_response response;
    CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

    std::unordered_map<std::string, ConnectPtr>::iterator it = m_Creating.find(response.username());
    CHECKERRORANDRETURN(it != m_Creating.end());

    it->second->SendBuffer(pPack->getMessageId(), response, 0);
    m_Creating.erase(it);
}

//-------------------------------------------------------------------------------------------
void AccountNetClient::AccountCheck(ConnectPtr& pConnect, pm_account_check& request)
{
    request.set_increament(m_LoginGameIncreament);
    m_pLogin.insert(std::make_pair(m_LoginGameIncreament++, pConnect));

    SendProtoBuf(GM_ACCOUNT_CHECK, request);
}

//-------------------------------------------------------------------------------------------
void AccountNetClient::AccountCheckResponse(PackPtr& pPack)
{
    pm_account_check_response response;
    CHECKERRORANDRETURN(response.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

    std::unordered_map<unsigned int, ConnectPtr>::iterator it = m_pLogin.find(response.increament());
    CHECKERRORANDRETURN(it != m_pLogin.end());

    if (response.result() == 0)
    {
        std::unordered_map<ConnectPtr, PlayerManager::Session> m_AllPlayer = PlayerManager::getInstance().GetPlayerMap();
        std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>> m_AccPlayer = PlayerManager::getInstance().GetAccPlayerMap();

        std::unordered_map<ConnectPtr, PlayerManager::Session>::iterator sit = m_AllPlayer.find(it->second);
        if (sit == m_AllPlayer.end())
        {
            PlayerManager::Session player;
            player.acc_id = response.username();
            player.playerid = 0;
            m_AllPlayer[it->second] = player;
        }

        std::unordered_map<std::string, std::unordered_map<unsigned int, Player*>>::iterator accit = m_AccPlayer.find(response.username());
        if (accit != m_AccPlayer.end())
        {
            std::unordered_map<unsigned int, Player*>::iterator playerit = accit->second.begin();
            for (; playerit != accit->second.end(); ++playerit)
            {
                pm_player_sub_data* pData = response.add_playerdata();
                pData->set_id(playerit->second->GetPlayerData().id);
                pData->set_name(playerit->second->GetPlayerData().name);
            }
        }
        else
        {
            m_AccPlayer.insert(std::make_pair(response.username(), std::unordered_map<unsigned int, Player*>()));
        }
    }

    it->second->SendBuffer(pPack->getMessageId(), response, 0);
    m_pLogin.erase(it);
}