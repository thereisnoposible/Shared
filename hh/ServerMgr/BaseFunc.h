#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

class Player
{
public:
	std::unordered_map<std::string, std::tr1::function<void(std::vector<std::string>&)>> _funcMap;
	Player();
protected:
	void registmessge();
	void registfunc();
	void CreateAccount(std::vector<std::string>& param);
	void AccountCheck(std::vector<std::string>& param);
	void CreatePlayer(std::vector<std::string>& param);
	void LoginGame(std::vector<std::string>& param);
	void Move(std::vector<std::string>& param);

	void CreateAccountResponse(PackPtr& pPack);
	void AccountCheckResponse(PackPtr& pPack);
	void CreatePlayerResponse(PackPtr& pPack);
	void LoginGameResponse(PackPtr& pPack);

    void AAAA(int,int,int);

};


