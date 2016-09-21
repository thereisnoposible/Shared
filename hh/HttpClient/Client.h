#pragma once
#include "connect_manager.h"
#include <unordered_map>
#include "TimerManager.h"
#include "game_struct.h"
#include <map>

class Client
{
public:
	Client();
	~Client();
	void run(double diff);
	void fire(std::string& cmd, std::string& param);
protected:
	void registmessage();

	void as_login(std::string& param);
	void as_login_ticket();
	void as_login_ticket_response(http::http_response& response);
	void as_login_response(http::http_response& response);
	void loggedin();
	void loggedin_response(http::http_response& response);
	void login_cb_response(http::http_response& response);
	void login_response(http::http_response& response);
	void init();
	void request_init_data();
	void request_init_data_response(http::http_response& response);
	void get_fight_map();
	void get_fight_map_response(http::http_response& response);
	void get_fight_map_sub(int cid);
	void get_fight_map_sub_response(int cid,http::http_response& response);
	void playgame_response(int map, http::http_response& response);

	void get_challenge();
	void get_challenge_response(http::http_response& response);
	void challenge_response(http::http_response& response);

	void request_player_data();
	void request_player_data_response(http::http_response& response);

	void set_mode(std::string& param);
	void fight(int map);

	std::string parse(std::string& temp);
	
	void run_fight();

	void request_eat();
	void request_eat_response(http::http_response& response);

	void getHero();
	void getHero_response(http::http_response& response);
private:
	http::connect_manager* m_manager;
	std::unordered_map<std::string, std::function<void(std::string&)>> m_message;

	std::string cookie;
	std::string token;

	std::string expire;
	std::string sig;
	std::string plat_id;

	std::string deviceid = "B5028106-12E3-483D-99BC-3E180138F779";

	int endurance;	//ƒÕ¡¶
	int pow;		//ÃÂ¡¶

	TimerManager* m_pTimerManager;	
	int fight_mode;

	std::map<int, Chapter> m_pChapter;

	TMHANDLE m_fight;
	TMHANDLE m_challenge;
	TMHANDLE m_playerdata;
	TMHANDLE m_hero;

	TMHANDLE m_eat1;
	TMHANDLE m_eat2;
	TMHANDLE m_eat3;
};