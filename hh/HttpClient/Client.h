#pragma once
#include "connect_manager.h"
#include <unordered_map>
#include "timer/TimerManager.h"
#include "game_struct.h"
#include "timer/TimeWheel.h"
#include <map>
#include <set>

#include "pinger.h"

using namespace xlib;

namespace Json
{
	class Value;
}

enum friendStep
{
	INIT = 0,
	DELETE_FRIEND,
	DELETE_FRIEND_RESPONSE,
	ADD_FRIEND,
	ADD_FRIEND_RESPONSE,
	GET_NEW,
	GET_NEW_RESPONSE,
	ACCEPT_ADD_FRIEND,
	ACCEPT_ADD_FRIEND_RESPONSE,
	SEND_POWER,
	SEND_POWER_RESPONSE,
	ACCEPT_POWER,
	ACCEPT_POWER_RESPONSE = INIT,

	REMOVE_TIMER = -1,
    CHECKSTEP = -2,
};

class Client
{
public:
	Client(std::string& plat);
	~Client();
	void run(double diff);
	void fire(std::string& cmd, std::string& param);

	bool webreward_response(http::http_response& response, std::string& addr, std::string& port, std::chrono::steady_clock::time_point& tim);
protected:
	void registmessage();

	void as_login(std::string& param);
	void as_login_ticket();
	bool as_login_ticket_response(http::http_response& response);
	bool as_login_response(http::http_response& response);
	void loggedin();
	bool loggedin_response(http::http_response& response);
	bool login_cb_response(http::http_response& response);
	bool login_response(http::http_response& response);
	void init();
	void request_init_data();
	void request_init_data_response(Json::Value& value);
	void get_fight_map();
	void get_fight_map_response(Json::Value& value);
	void get_fight_map_sub(int cid);
	void get_fight_map_sub_response(int cid, Json::Value& value);
	void playgame_response(int map, Json::Value& value);

	void get_challenge();
	void get_challenge_response(Json::Value& value);
	void challenge_response(Json::Value& value);

	void request_player_data();
	void request_player_data_response(Json::Value& value);

	void set_mode(std::string& param);
    void set_fight(std::string& param);
	void set_ip(std::string& param);
	void fight(int map);

	std::string parse(std::string& temp);
	
	void run_fight();

	void request_eat();
	void request_eat_response(Json::Value& value);

	void getHero();
	void getHero_response(Json::Value& value);

	void startTower();
	void startTower_response(Json::Value& value);
	void startTowerFight();
	void startTowerFight_response(Json::Value& value);

	void signin();
	void onlineGift();
	void buyPow();
	void CalendarSign();
	void CardDraw(int type);

    void CupSignUp();
    void ManorGet();
    void ManorGet_response(Json::Value& value);
    void ManortaskReward(int wid);
    void ManordoTask(int task, int wid);

	void UseItem(int id);
	void Wish();

	void luckyTurnHappyReset(int aid);
	void luckyTurnHappyAward(int id, int aid);

	void loulanPray();
	void prayReward();

    void ManordojoReward(int pid);
    void ManordojoTrain(int pid, int wid);

    void ManorfishReward(int wid);
    void Manorfish(int bid, int pid, int wid);

    void ManororderReward(int wid);
    void ManordoOrder(int oid, int wid);

	void ManorRespone(Json::Value& value, int type, int manor, int wid);

    void Activeness(int point);

    void worldBoss();
    void worldBoss_response(Json::Value& value);
    void startWorldBoss();
    void startWorldBoss_response(Json::Value& value);
    void playWorldBoss_response(Json::Value& value);

    void guildSign();

    void GuildBoss();
    void GuildBoss_response(Json::Value& value);
    void GuildBossAttack();
    void GuildBossAttack_response(Json::Value& value);

    void shared();
    void shared_response(Json::Value& value);

	void startSail();
	void startSail_response(Json::Value& value);
	void boat();
	void boat_response(Json::Value& value);
	void sailing();
	void sailing_response(Json::Value& value);

	void sailing_type2();
	void sailing_type3();
	void sailing_type5();
	void sailing_type6();

	void post(http::http_request, std::function<void(Json::Value&)>);
	bool post_callback(http::http_response, std::function<void(Json::Value&)>);

	void checkStep();
	void delete_friend();
	void delete_friend_response(Json::Value& value);
	void add_friend();
	void add_friend_response(Json::Value& value);
	void get_new();
	void get_new_response(Json::Value& value);
	void accept_add_friend();
	
	void accept_add_friend_response(Json::Value& value);
	void send_power();
	void send_power_response(Json::Value& value);
	void accept_power();
	void accept_power_response(Json::Value& value);

    std::chrono::steady_clock::time_point fCurr;

	static int ip1;
	static int ip2;
	static int ip3;
	static int ip4;
private:
	http::connect_manager* m_manager;
	std::unordered_map<std::string, std::function<void(std::string&)>> m_message;

	std::string platform_;

	std::string cookie;
	std::string token;

	std::string expire;
	std::string sig;
	std::string plat_id;

	std::string ios_deviceid = "B5028106-12E3-483D-99BC-3E180138F779";
	std::string android_deviceid = "127468846146468";
    std::string version = "1.0.9";

	int endurance;	//ƒÕ¡¶
	int pow;		//ÃÂ¡¶

	TimerManager* m_pTimerManager;	
    timewheel::TimeWheel* m_pTimeWheel;
	int fight_mode;

	std::map<int, Chapter> m_pChapter;

    TMHANDLE m_step;
    TMHANDLE m_fight;
    TMHANDLE m_challenge;
    TMHANDLE m_hero;
    TMHANDLE m_tower;
    TMHANDLE m_sailing;

	int map_high;
	int map_low;

	static friendStep step_;

    int fight_high = 17039;// 15920;
    int fight_low = 13941;// 13025;

    std::map<int, int> maf;

    std::set<int> m_wid;
    std::vector<int> m_wid_vec;

	//pinger* m_pinger;
	boost::asio::io_service io_service;
};