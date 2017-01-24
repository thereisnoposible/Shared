#pragma once

#include <boost/multi_index_container.hpp>  
#include <boost/multi_index/ordered_index.hpp>  
#include <boost/multi_index/identity.hpp>  
#include <boost/multi_index/member.hpp>  
#include <boost/lambda/lambda.hpp>  

class LoginManager
{
public:

	LoginManager();
	~LoginManager();
protected:
	void registerMessage();

	void init();
	void loadAcc();
	void loadPlayer();

	void CreateAccount(PackPtr& pPack);
	void AccountCheck(PackPtr& pPack);

	void notifyInsertAccount(int id, std::string name, std::string psw);

private:
	int _unique_acc;
	int _unique_player;

	std::unordered_map<std::string, std::string> _accMap;

	MysqlStmt* m_pDBService;
};