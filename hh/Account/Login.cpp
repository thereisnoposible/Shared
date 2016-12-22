#include "stdafx.h"
#include "Login.h"

#include "../new/proto/protobuf/login.pb.h"
//-------------------------------------------------------------------------------------------
LoginManager::LoginManager()
{
	registerMessage();
	m_pDBService = new DBService(1);
	if (!m_pDBService->Open("127.0.0.1", "root", "123456", "accdb"))
	{
		std::cout << "open accdb failed\n";
	}
	init();
}

//-------------------------------------------------------------------------------------------
LoginManager::~LoginManager()
{
	delete m_pDBService;
	m_pDBService = nullptr;
}

//-------------------------------------------------------------------------------------------
void LoginManager::registerMessage()
{
	NetService::getInstance().RegisterMessage(GM_CREATE_ACCOUNT, boost::bind(&LoginManager::CreateAccount, this, _1));
	NetService::getInstance().RegisterMessage(GM_ACCOUNT_CHECK, boost::bind(&LoginManager::AccountCheck, this, _1));
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
	DBResult result;
    m_pDBService->syncQuery(std::move(sql), result);

	while (!result.eof())
	{
		int id = result.getIntField("id");
		std::string name = result.getStringField("accname");
		std::string psw = result.getStringField("psw");
		if (id > _unique_acc)
			_unique_acc = id;

		_accMap.insert(std::make_pair(name, psw));

		result.nextRow();
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
	std::stringstream ss;

	ss << "insert into account(id,accname,psw) values(";
	ss << id << ",";
	ss << "'" << name << "'" << ",";
	ss << "'" << psw << "'" << ")";
	std::string sql = ss.str();
	//直接投递执行掉,后期补充安全机制
	m_pDBService->asynExcute(0, std::move(sql), NULLFUNC);
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