#include "Client.h"
#include "json/json.h"
#include "Helper.h"
#include "json/json_reader.cpp"
#include "json/json_value.cpp"
#include "json/json_writer.cpp"
#include "TimerManager.cpp"

enum code
{
	Success,			//成功
	NOON_POWER = 1101,	//无体力
	NOON_ACCESS = 1104,	//之前小节未战斗
	NOON_WAIT = 1210,	//操作太频繁
	NOON_DEDURENCE = 2210,	//耐力不足
	NEED_PERMISSION = 9000,	//许可证
};

std::unordered_map<int, std::string> codestr;

void Client::registmessage()
{
	m_message["as_login"] = std::bind(&Client::as_login, this, std::placeholders::_1);
	m_message["set_mode"] = std::bind(&Client::set_mode, this, std::placeholders::_1);

	codestr[NOON_POWER] = "无体力";
	codestr[NOON_ACCESS] = "之前小节未战斗";
	codestr[NOON_WAIT] = "操作太频繁";
	codestr[NOON_DEDURENCE] = "耐力不足";
}

void test(int i)
{
	std::cout << Helper::Timet2String(time(NULL)) << ": " << i << "\n";
}

Client::Client()
{
	m_manager = new http::connect_manager();
	registmessage();

	m_pTimerManager = new TimerManager;

	//m_fight = m_pTimerManager->AddIntervalTimer(30, std::bind(&Client::run_fight, this));

	fight_mode = 1;

	m_playerdata = m_pTimerManager->AddIntervalTimer(6 * 30, std::bind(&Client::request_player_data, this));

	m_eat1 = m_pTimerManager->AddTriggerTimer(12, 1, 0, std::bind(&Client::request_eat, this));
	m_eat2 = m_pTimerManager->AddTriggerTimer(17, 1, 0, std::bind(&Client::request_eat, this));
	m_eat3 = m_pTimerManager->AddTriggerTimer(21, 1, 0, std::bind(&Client::request_eat, this));

	m_hero = NULL;
}

Client::~Client()
{
	if (m_fight != NULL)
	{
		m_pTimerManager->RemoveTimer(m_fight);
	}
	if (m_challenge != NULL)
	{
		m_pTimerManager->RemoveTimer(m_challenge);
	}
	if (m_playerdata != NULL)
	{
		m_pTimerManager->RemoveTimer(m_playerdata);
	}
	if (m_eat1 != NULL)
	{
		m_pTimerManager->RemoveTimer(m_eat1);
	}
	if (m_eat2 != NULL)
	{
		m_pTimerManager->RemoveTimer(m_eat2);
	}
	if (m_eat3 != NULL)
	{
		m_pTimerManager->RemoveTimer(m_eat3);
	}
	delete m_manager;
	delete m_pTimerManager;
}

void Client::run(double diff)
{
	m_manager->update();

	m_pTimerManager->Update(diff);
}

void Client::run_fight()
{
	if (m_fight != NULL)
	{
		m_pTimerManager->RemoveTimer(m_fight);
		m_fight = NULL;
	}
	//通关
	if (fight_mode == 1)
	{
		std::map<int, Chapter>::reverse_iterator it = m_pChapter.rbegin();
		std::map<int, Explore>::reverse_iterator exit = it->second.explores.rbegin();
		int mid = it->second.cid * 100;
		if (exit != it->second.explores.rend())
		{
			mid = exit->second.mid;
			if (exit->second.score == 0)
				mid -= 1;
		}
		mid += 1;
		fight(mid);	
	}
	//3星
	if (fight_mode == 2)
	{
		std::map<int, Chapter>::iterator it = m_pChapter.begin();
		for (; it != m_pChapter.end(); ++it)
		{
			std::map<int, Explore>::iterator exit = it->second.explores.begin();
			for (; exit != it->second.explores.end(); ++exit)
			{
				if (exit->second.score != 3 && (exit->second.times-exit->second.todayTimes) > 0)
				{
					fight(exit->second.mid);
					return;
				}
			}
		}
	}
}

void Client::fire(std::string& cmd, std::string& param)
{
	std::unordered_map<std::string, std::function<void(std::string&)>>::iterator it = m_message.find(cmd);
	if (it != m_message.end())
	{
		it->second(param);
	}
}

void Client::as_login(std::string& param)
{
	as_login_ticket();
}

void Client::as_login_ticket()
{
	http::http_request request;
	request.type = HTTP_POST;
	request.url = "/sdk/as_login.php";
	request.host = "auth.thedream.cc";
	request.head["Content-Type"] = "application/x-www-form-urlencoded";
	//request.head["User-Agent"] = "Dalvik/1.6.0 (IOS; U; iphone 6.1)";
	request.head["User-Agent"] = "PL.APPSTORE.CN/223 CFNetwork/758.2.8 Darwin/15.0.0";
	request.head["Connection"] = "Keep-Alive";
	request.head["Keep-Alive"] = "timeout= 5,max = 0";
	request.head["Accept-Encoding"] = "gzip, deflate";

	request.body = "method=ticket&app_id=66";
	std::stringstream ss;
	ss << request.body.size();
	request.head["Content-Length"] = ss.str();

	m_manager->post(request, std::bind(&Client::as_login_ticket_response, this, std::placeholders::_1));
}

void Client::as_login_ticket_response(http::http_response& response)
{
	if (response.code != 200)
		abort();

	Json::Reader reader;
	Json::Value value;
	reader.parse(response.body, value, false);

	http::http_request request;
	request.type = HTTP_POST;
	request.url = "/sdk/as_login.php";
	request.host = "auth.thedream.cc";
	request.head["Content-Type"] = "application/x-www-form-urlencoded; charset = UTF-8";
	request.head["User-Agent"] = "Dalvik/1.6.0 (IOS; U; iphone 6.1)";
	request.head["Connection"] = "Keep-Alive";
	request.head["Keep-Alive"] = "timeout = 5, max = 1";
	request.head["Accept-Encoding"] = "gzip";

	std::stringstream ss;

	request.body = "method=login&app_id=66&upw=e3dd4b27ec803a0c489b448c0740e197523a294e&uid=a65508333&ticket=";
	request.body += value["ticket"].asString();
	request.body += "&ch_id=100000&os=2";

	ss << request.body.size();
	request.head["Content-Length"] = ss.str();

	m_manager->post(request, std::bind(&Client::as_login_response, this, std::placeholders::_1));
}

void Client::as_login_response(http::http_response& response)
{
	if (response.code != 200)
		abort();
	Json::Reader reader;
	Json::Value value;
	reader.parse(response.body, value, false);

	token = value["token"].asString();

	loggedin();
}

void Client::loggedin()
{
	http::http_request request;
	request.type = HTTP_POST;
	request.url = "/receive/track/loggedin";
	request.host = "log.reyun.com";

	request.head["Content-Type"] = "application/json";
	request.head["Connection"] = "Keep-Alive";
	request.head["Accept-Encoding"] = "gzip";
	request.head["User-Agent"] = "android-async-http/1.4.1 (http://loopj.com/android-async-http)";

	Json::FastWriter writer;
	Json::Value value;
	Json::Value content;
	content["channelid"] = "_default_";
	content["mac"] = "02:00:00:00:00:00";
	content["idfa"] = deviceid;
	content["ryos"] = "ios";
	content["lats"] = "1";
	content["ryosversion"] = "9.2.1";
	content["openudid"] = "";
	content["ry_sdk_name"] = "ios_channel";
	content["rydevicetype"] = "iPhone";
	content["deviceid"] = deviceid;
	content["ry_sdk_ver"] = "2.0.0";	
	content["idfv"] = "BFD680A7-1CDA-4940-B874-ACF9E40FE4FC";
	
	

	value["context"] = content;
	value["appid"] = "f7e3fd6fe2a69e4b70660ed30c6d6486";
	value["what"] = "loggedin";
	value["when"] = Helper::Timet2String(time(NULL));
	value["who"] = "a65508333";

	request.body = writer.write(value);
	//request.pend_flag = "0\r\n\r\n";
	m_manager->post(request, std::bind(&Client::loggedin_response, this, std::placeholders::_1));

}
void Client::loggedin_response(http::http_response& response)
{
	if (response.code != 200)
		abort();
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20043/port/dream/login_cb.php?u=&c=";
	request.url += deviceid;
	request.url += "&et=iPhone6,2_Darwin_15.0.0&t=";
	request.url += token;
	request.url += "&uid=&plat_lj=&plat_lj_uid=&transfer=&v=1.0.7&plat=100000";
	request.host = "pl-game.thedream.cc";
	request.head["Accept"] = "*/*";
	
	request.pend_flag = "0\r\n\r\n";
	m_manager->post(request, std::bind(&Client::login_cb_response, this, std::placeholders::_1));
}

void Client::login_cb_response(http::http_response& response)
{
	if (response.code != 200)
		abort();
	Json::Reader reader;
	Json::Value value;
	int pos = response.body.find("{");
	int rpos = response.body.rfind("}");

	if (pos == std::string::npos || rpos == std::string::npos)
		abort();
	std::string body = response.body.substr(pos, rpos - pos + 1);
	reader.parse(body, value, false);

	Json::Value data;
	data = value["data"];
	Json::Value plat_auth = data["plat_auth"];
	expire = plat_auth["expire"].asString();
	sig = plat_auth["sig"].asString();
	plat_id = data["plat_id"].asString();

	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/dream/login.php?u=";
	request.url += plat_id;
	request.url += "&p=&c=";
	request.url += deviceid;
	request.url += "&et=iPhone6,C2_Darwin_15.0.0&t=";
	request.url += token;
	request.url += "&is_guest=0&e=";
	request.url += expire;
	request.url += "&s=";
	request.url += sig;
	request.url += "&plat_lj=&v=1.0.7&&sys=ios&plat=100000";
	request.host = "pl-game.thedream.cc";
	request.head["Accept"] = "*/*";
	request.pend_flag = "0\r\n\r\n";

	m_manager->post(request, std::bind(&Client::login_response, this, std::placeholders::_1));
}

void Client::login_response(http::http_response& response)
{
	if (response.code != 200)
		abort();
	cookie = response.head["Set-Cookie"];
	int pos = cookie.find(";");
	cookie = cookie.substr(0, pos);

	init();
}

std::string Client::parse(std::string& temp)
{
	std::vector<std::string> vec;
	std::string body;
	Helper::SplitString(temp, "\r\n", vec);
	for (int i = 0; i < (int)vec.size(); i++)
	{
		if (i % 2 == 1)
		{
			body += vec[i];
		}
	}
	return body;
}

void Client::request_player_data()
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = " /s20042/port/interface.php?s=User&m=get&a={}&v=1.0.7&sys=ios";
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	m_manager->post(request, std::bind(&Client::request_player_data_response, this, std::placeholders::_1));
}
void Client::request_player_data_response(http::http_response& response)
{
	if (response.code != 200)
		abort();

	std::string& body = parse(response.body);

	Json::Reader reader;
	Json::Value value;
	reader.parse(body, value);

	if (value.isMember("code"))
	{
		if (Helper::StringToInt(value["code"].asString()) == NEED_PERMISSION)
		{
			as_login_ticket();
			return;
		}
	}

	if (value.isMember("data"))
	{
		endurance = value["data"]["user"]["endurance"].asInt();
		pow = value["data"]["user"]["pow"].asInt();
	}

	if (endurance > 0)
	{
		get_challenge();
	}
	if (pow > 0)
	{
		run_fight();
	}
}

void Client::init()
{
	request_init_data();
	get_fight_map();
	request_eat();
	getHero();
}

void Client::request_init_data()
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=System&m=init&a=%7B%7D&v=1.0.7&sys=ios";
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	m_manager->post(request, std::bind(&Client::request_init_data_response, this, std::placeholders::_1));
}

void Client::request_init_data_response(http::http_response& response)
{
	if (response.code != 200)
		abort();

	std::string& body = parse(response.body);

	Json::Reader reader;
	Json::Value value;
	reader.parse(body, value);
	if (value.isMember("data"))
	{
		endurance = value["data"]["user"]["endurance"].asInt();
		pow = value["data"]["user"]["pow"].asInt();
	}

	//fight(map_high * 100 + map_low);
}

void Client::get_fight_map()
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Explore&m=getC&a={}&v=1.0.7&sys=ios";
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	m_manager->post(request, std::bind(&Client::get_fight_map_response, this, std::placeholders::_1));
}

void Client::get_fight_map_response(http::http_response& response)
{
	if (response.code != 200)
		abort();

	m_pChapter.clear();

	std::string& body = parse(response.body);

	Json::Reader reader;
	Json::Value value;
	reader.parse(body, value);

	int map_high = 0;

	Json::Value chapters = value["data"]["chapters"];
	for (int i = 0; i < (int)chapters.size(); i++)
	{
		if (chapters[i]["cid"].asInt()>10000)
			continue;

		Chapter temp;
		temp.cid = chapters[i]["cid"].asInt();
		temp.pres = chapters[i]["pres"].asInt();
		temp.af = chapters[i]["af"].asInt();
		temp.grade_award = chapters[i]["grade_award"].asInt();
		temp.great_num = chapters[i]["great_num"].asInt();
		temp.video_award = chapters[i]["video_award"].asInt();
		temp.activity_award = chapters[i]["activity_award"].asInt();
		temp.id = chapters[i]["id"].asInt();

		m_pChapter.insert(std::make_pair(temp.cid, temp));

		if (map_high < temp.cid)
			map_high = temp.cid;

		get_fight_map_sub(temp.cid);
	}
}

void Client::get_fight_map_sub(int cid)
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Explore&m=get&a={\"cid\":";
	request.url += Helper::IntToString(cid);
	request.url += "}&v=1.0.7&sys=ios";
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	m_manager->post(request, std::bind(&Client::get_fight_map_sub_response, this, cid, std::placeholders::_1));
}

void Client::get_fight_map_sub_response(int cid, http::http_response& response)
{
	if (response.code != 200)
		abort();

	std::string& body = parse(response.body);

	Json::Reader reader;
	Json::Value value;
	reader.parse(body, value);

	Json::Value explores = value["data"]["explores"];
	Chapter& chapter = m_pChapter[cid];

	for (int i = 0; i < (int)explores.size(); i++)
	{
		Explore temp;
		temp.id = explores[i]["id"].asInt();
		temp.mid = explores[i]["mid"].asInt();
		temp.score = explores[i]["score"].asInt();
		temp.times = explores[i]["times"].asInt();
		temp.todayTimes = explores[i]["todayTimes"].asInt();
		temp.lastBattleTs = explores[i]["lastBattleTs"].asInt();
		temp.lastExploreTs = explores[i]["lastExploreTs"].asInt();

		chapter.explores.insert(std::make_pair(temp.mid, temp));
	}

	Chapter& item = m_pChapter.rbegin()->second;
	if (chapter.cid == m_pChapter.rbegin()->second.cid && item.explores.size() == 10)
	{
		Chapter temp;
		temp.cid = item.cid + 1;
		temp.pres = 0;
		temp.af = 0;
		temp.grade_award = 0;
		temp.great_num = 0;
		temp.video_award = 0;
		temp.activity_award = 0;
		temp.id = 0;
		m_pChapter.insert(std::make_pair(temp.cid, temp));
		get_fight_map_sub(temp.cid);
	}

	if (chapter.cid == m_pChapter.rbegin()->second.cid)
	{
		run_fight();
		get_challenge();
	}
}

void Client::fight(int map)
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Explore&m=exec&a={\"mid\":\"";
	request.url += Helper::IntToString(map);
	request.url += "\"}&v=1.0.7&sys=ios";
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";
	m_manager->post(request, std::bind(&Client::playgame_response, this, map, std::placeholders::_1));
}

void Client::playgame_response(int map, http::http_response& response)
{
	if (response.code != 200)
		abort();

	std::string& body = parse(response.body);

	Json::Reader reader;
	Json::Value value;
	int pos = body.find("{");
	int rpos = body.rfind("}");

	if (pos == std::string::npos || rpos == std::string::npos)
		abort();
	body = body.substr(pos, rpos - pos + 1);
	reader.parse(body, value, false);

	if (value.isMember("code"))
	{
		std::string codess = value["code"].asString();
		int code = Helper::StringToInt(codess);

		std::cout << Helper::Timet2String(time(NULL)) << ": play code:" << code << "\n";

		switch (code)
		{
		case NOON_POWER:
			std::cout << "error code:" << codess << codestr[code] << "\n";
			return;
		case Success:
			pow -= 1;
		case NOON_WAIT:
			break;
		case NEED_PERMISSION:
			as_login_ticket();
			return;
		default:
			get_fight_map();
			return;
		}
	}

	if (value["data"].isMember("pve"))
	{
		if (value["data"]["pve"]["grade"] != 0)
		{
			int cid = map / 100;
			std::map<int, Explore>::iterator it = m_pChapter[cid].explores.find(map);
			if (it == m_pChapter[cid].explores.end())
			{
				Explore temp;
				temp.mid = map;
				temp.score = value["data"]["pve"]["grade"].asInt();
				m_pChapter[cid].explores[map] = temp;
				it = m_pChapter[cid].explores.find(map);
			}

			if (value["data"]["pve"]["grade"].asInt() > it->second.score)
			{
				it->second.score = value["data"]["pve"]["grade"].asInt();
			}	
			it->second.todayTimes++;
		}
	}
	if (m_fight != NULL)
	{
		m_pTimerManager->RemoveTimer(m_fight);
	}
	m_fight = m_pTimerManager->AddIntervalTimer(5, std::bind(&Client::run_fight, this));
	//run_fight();
}

void Client::set_mode(std::string& param)
{
	std::vector<std::string> vec;
	Helper::SplitString(param, " ", vec);
	if (vec.size() != 1)
		return;

	fight_mode = Helper::StringToInt(vec[0]);
}

void Client::get_challenge()
{
	if (m_challenge != NULL)
	{
		m_pTimerManager->RemoveTimer(m_challenge);
		m_challenge = NULL;
	}

	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Challenge&m=init&a={}&v=1.0.7&sys=ios";
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	m_manager->post(request, std::bind(&Client::get_challenge_response, this, std::placeholders::_1));
}
void Client::get_challenge_response(http::http_response& response)
{
	if (response.code != 200)
		abort();

	std::string& body = parse(response.body);

	Json::Reader reader;
	Json::Value value;
	reader.parse(body, value);
	if (value.isMember("data"))
	{
		int trank = value["data"]["chAvailRanks"][1].asInt();
		int tid = 0;

		for (int i = 0; i < value["data"]["chRanks"].size(); i++)
		{
			if (value["data"]["chRanks"][i]["rank"].asInt() == trank)
				tid = value["data"]["chRanks"][i]["uid"].asInt();
		}

		int srank = value["data"]["challenge"]["rank"].asInt();
		http::http_request request;
		request.type = HTTP_GET;
		request.url = "/s20042/port/interface.php?s=Challenge&m=challenge&a={\"sRank\":";
		request.url += Helper::IntToString(srank);
		request.url += ",\"tRank\":";
		request.url += Helper::IntToString(trank);
		request.url += ",\"tid\":";
		request.url += Helper::IntToString(tid);
		request.url += "}&v=1.0.7&sys=ios";
		request.host = "pl-game.thedream.cc";
		request.head["Cookie"] = cookie;
		request.pend_flag = "0\r\n\r\n";

		m_manager->post(request, std::bind(&Client::challenge_response, this, std::placeholders::_1));
	}
}

void Client::challenge_response(http::http_response& response)
{
	if (response.code != 200)
		abort();

	std::string& body = parse(response.body);

	Json::Reader reader;
	Json::Value value;
	int pos = body.find("{");
	int rpos = body.rfind("}");

	if (pos == std::string::npos || rpos == std::string::npos)
		abort();
	body = body.substr(pos, rpos - pos + 1);
	reader.parse(body, value, false);

	if (value.isMember("code"))
	{
		std::string codess = value["code"].asString();
		int code = Helper::StringToInt(codess);

		std::cout << Helper::Timet2String(time(NULL)) << ": challenge code:" << code << "\n";

		switch (code)
		{
		case NOON_DEDURENCE:
			std::cout << "error code:" << codess << codestr[code] << "\n";
			return;
		case Success:
			endurance -= 1;
		case NOON_WAIT:
			break;
		default:
			break;
		}
	}

	get_challenge();
}

void Client::request_eat()
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Activity&m=eat&a={}&v=1.0.7&sys=ios";
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	m_manager->post(request, std::bind(&Client::request_eat_response, this, std::placeholders::_1));
}

void Client::request_eat_response(http::http_response& response)
{
	if (response.code != 200)
		abort();

	std::string& body = parse(response.body);

	Json::Reader reader;
	Json::Value value;
	int pos = body.find("{");
	int rpos = body.rfind("}");

	if (pos == std::string::npos || rpos == std::string::npos)
		abort();
	body = body.substr(pos, rpos - pos + 1);
	reader.parse(body, value, false);

	std::cout << Helper::Timet2String(time(NULL)) << ": challenge code:" << value["code"].asString() << "\n";

	if (Helper::StringToInt(value["code"].asString()) == 0)
	{
		pow = value["data"]["user"]["pow"].asInt();
	}
}

void Client::getHero()
{
	if (m_hero == NULL)
	{
		m_hero = m_pTimerManager->AddIntervalTimer(5, std::bind(&Client::getHero, this));
	}

	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Explore&m=getHero&a={}&v=1.0.7&sys=ios";
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	m_manager->post(request, std::bind(&Client::getHero_response, this, std::placeholders::_1));
}

void Client::getHero_response(http::http_response& response)
{
	if (response.code != 200)
		abort();

	std::string& body = parse(response.body);

	Json::Reader reader;
	Json::Value value;
	int pos = body.find("{");
	int rpos = body.rfind("}");

	if (pos == std::string::npos || rpos == std::string::npos)
		abort();
	body = body.substr(pos, rpos - pos + 1);
	reader.parse(body, value, false);

	std::cout << Helper::Timet2String(time(NULL)) << ": getHero code:" << value["code"].asString() << "\n";

	if (Helper::StringToInt(value["code"].asString()) == 0)
	{
		std::map<int, int> mm;
		Json::Value explores = value["data"]["explores"];
		for (int i = 0; i < explores.size(); i++)
		{
			if (explores[i]["todayTimes"].asInt() == 0)
				mm[explores[i]["mid"].asInt()] = 0;
		}

		if (mm.size() == 0)
		{
			m_pTimerManager->RemoveTimer(m_hero);
			m_hero = NULL;
			return;
		}

		http::http_request request;
		request.type = HTTP_GET;
		request.url = "/s20042/port/interface.php?s=Explore&m=exec&a={\"mid\":\"";
		request.url += Helper::IntToString(mm.begin()->first);
		request.url += "\"}&v=1.0.7&sys=ios";
		request.host = "pl-game.thedream.cc";
		request.head["Cookie"] = cookie;
		request.pend_flag = "0\r\n\r\n";
		m_manager->post(request, std::function<void(http::http_response&)>());
	}


}