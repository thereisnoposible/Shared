#include "stdafx.h"
#include "Application.h"
#include "Login.h"

#include "../new/proto/protobuf/login.pb.h"
//-------------------------------------------------------------------------------------------
LoginManager::LoginManager()
{
	registerMessage();
	m_pDBService = new MysqlStmt;
	if (!m_pDBService->Open("127.0.0.1", "root", "123456", "accdb"))
	{
		std::cout << "open accdb failed\n";
	}
	init();
}

//-------------------------------------------------------------------------------------------
LoginManager::~LoginManager()
{
    m_pDBService->Close();

	delete m_pDBService;
	m_pDBService = nullptr;
}

//-------------------------------------------------------------------------------------------
void LoginManager::Update()
{
	m_pDBService->Update();
}

//-------------------------------------------------------------------------------------------
void LoginManager::registerMessage()
{
    sNetService.RegisterMessage(GM_CREATE_ACCOUNT, boost::bind(&LoginManager::CreateAccount, this, _1));
    sNetService.RegisterMessage(GM_ACCOUNT_CHECK, boost::bind(&LoginManager::AccountCheck, this, _1));
}


//-------------------------------------------------------------------------------------------
void LoginManager::init()
{
	loadAcc();
}

//-------------------------------------------------------------------------------------------
void LoginManager::loadAcc()
{
	_unique_acc = 0;

    std::string sql = "select * from account";
    StmtSyncResult result;
    m_pDBService->SyncExecuteQueryVariable(sql, nullptr, result);

	while (!result.pResult.eof())
	{
        int id = *(int*)result.pResult.result.GetData("id")->buffer;
        std::string name = (char*)result.pResult.result.GetData("accname")->buffer;
        std::string psw = (char*)result.pResult.result.GetData("psw")->buffer;
		if (id > _unique_acc)
			_unique_acc = id;

		_accMap.insert(std::make_pair(name, psw));

        result.pResult.nextRow();
	}
	_unique_acc += 1;
}

//-------------------------------------------------------------------------------------------
void LoginManager::loadPlayer()
{

}

//-------------------------------------------------------------------------------------------
void LoginManager::notifyInsertAccount(int id, std::string name, std::string psw)
{
    std::string sql = "insert into account(id,accname,psw) values(?id,?accname,?psw)";
    StmtBindData* pParam = m_pDBService->PrepareQuery(sql.c_str());

    pParam->SetInt32(0, id);
    pParam->SetString(1, name);
    pParam->SetString(2, psw);

	//直接投递执行掉,后期补充安全机制
    m_pDBService->AsynExecuteQueryVariable(sql, pParam, nullptr);
}

//-------------------------------------------------------------------------------------------
void LoginManager::CreateAccount(PackPtr& pPack)
{
	pm_createaccount request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));

	int result = 0;
	for (int k = 0; k < 1; k++)
	{
		std::unordered_map<std::string, std::string>::iterator it = _accMap.find(request.username());
		if (it != _accMap.end())
		{
			result = 1;
			break;
		}
		_accMap.insert(std::make_pair(request.username(), request.pwd()));
		notifyInsertAccount(_unique_acc++, request.username(), request.pwd());
	}

	pm_createaccount_response response;
	response.set_result(result);
	response.set_username(request.username());
	pPack->m_Connect->SendBuffer(GM_CREATE_ACCOUNT_RESPONSE, response, 0);
}

//-------------------------------------------------------------------------------------------
void LoginManager::AccountCheck(PackPtr& pPack)
{
	pm_account_check request;
	CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));
	pm_account_check_response response;
	int result = 0;
	response.set_increament(request.increament());
	response.set_username(request.username());
	for (int k = 0; k < 1; k++)
	{
		std::unordered_map<std::string, std::string>::iterator it = _accMap.find(request.username());
		if (it == _accMap.end())
		{
			result = 1;
			break;
		}
		if (request.pwd() != it->second)
		{
			result = 2;
			break;
		}
	}
	
	response.set_result(result);
	pPack->m_Connect->SendBuffer(GM_ACCOUNT_CHECK_RESPONSE, response, 0);
}