#include "Client.h"
#include "json/json.h"
#include "helper/Helper.h"
#include "json/json_reader.cpp"
#include "json/json_value.cpp"
#include "json/json_writer.cpp"
#include <assert.h>

enum code
{
	Success,			//成功
	NOON_POWER = 1101,	//无体力
	NOON_ACCESS = 1104,	//之前小节未战斗
	NOON_WAIT = 1210,	//操作太频繁
	TOWER_DIE = 1706,	//
	NOON_DEDURENCE = 2210,	//耐力不足
	SAILING_FIGHT = 4726,	//有战斗
	SAILING_OVER = 4727,	//航海结束
	NEED_PERMISSION = 9000,	//许可证
	NOT_EXIST = 9002,	//关卡不存在
};

int Client::ip1 = 36;
int Client::ip2 = 17;
int Client::ip3 = 0;
int Client::ip4 = 0;

std::unordered_map<int, std::string> codestr;

void Client::registmessage()
{
	m_message["as_login"] = std::bind(&Client::as_login, this, std::placeholders::_1);
	m_message["set_mode"] = std::bind(&Client::set_mode, this, std::placeholders::_1);
    m_message["set_fight"] = std::bind(&Client::set_fight, this, std::placeholders::_1);
	m_message["set_ip"] = std::bind(&Client::set_ip, this, std::placeholders::_1);

	codestr[NOON_POWER] = "无体力";
	codestr[NOON_ACCESS] = "之前小节未战斗";
	codestr[NOON_WAIT] = "操作太频繁";
	codestr[NOON_DEDURENCE] = "耐力不足";
}

void test(int i)
{
	std::cout << Helper::Timet2String(time(NULL)) << ": " << i << "\n";
}

friendStep Client::step_ = INIT;

Client::Client(std::string& plat)
{
	m_manager = new http::connect_manager();
	registmessage();

	m_pTimerManager = new TimerManager;
    m_pTimeWheel = new timewheel::TimeWheel;
    m_pTimeWheel->Init(30, 1);

	//m_fight = m_pTimerManager->AddIntervalTimer(30, std::bind(&Client::run_fight, this));

	fight_mode = 1;

    m_pTimerManager->AddIntervalTimer(6 * 60, std::bind(&Client::request_player_data, this), -1);
    m_pTimerManager->AddIntervalTimer(60 * 60, std::bind(&Client::onlineGift, this), -1);
    m_pTimerManager->AddIntervalTimer(5, std::bind(&Client::buyPow, this), -1);

    m_pTimerManager->AddTriggerTimer(12, 1, 0, std::bind(&Client::request_eat, this), -1);
    m_pTimerManager->AddTriggerTimer(17, 1, 0, std::bind(&Client::request_eat, this), -1);
    m_pTimerManager->AddTriggerTimer(21, 1, 0, std::bind(&Client::request_eat, this), -1);
    m_pTimerManager->AddTriggerTimer(0, 1, 0, std::bind(&Client::signin, this), -1);

    m_pTimerManager->AddTriggerTimer(0, 1, 0, std::bind(&Client::guildSign, this), -1);

    m_pTimerManager->AddIntervalTimer(60, std::bind(&Client::CardDraw, this, 2), -1);
    m_pTimerManager->AddIntervalTimer(60, std::bind(&Client::CardDraw, this, 3), -1);
    m_pTimerManager->AddIntervalTimer(60, std::bind(&Client::CardDraw, this, 5), -1);

    m_pTimerManager->AddIntervalTimer(60, std::bind(&Client::Activeness, this, 25), -1);
    m_pTimerManager->AddIntervalTimer(60, std::bind(&Client::Activeness, this, 60), -1);
    m_pTimerManager->AddIntervalTimer(60, std::bind(&Client::Activeness, this, 90), -1);
    m_pTimerManager->AddIntervalTimer(60, std::bind(&Client::Activeness, this, 120), -1);

	m_pTimerManager->AddIntervalTimer(60, std::bind(&Client::loulanPray, this), -1);
	m_pTimerManager->AddIntervalTimer(60, std::bind(&Client::prayReward, this), -1);

	m_pTimeWheel->AddTimer(300000, std::bind(&Client::ManorGet, this), -1);

	m_pTimeWheel->AddTimer(300000, std::bind(&Client::UseItem, this, 10009), -1);
	m_pTimeWheel->AddTimer(300000, std::bind(&Client::UseItem, this, 10010), -1);

    m_pTimerManager->AddTriggerTimer(12, 1, 0, std::bind(&Client::GuildBoss, this), -1);

    m_step = nullptr;
    m_fight = nullptr;
    m_challenge = nullptr;
    m_hero = nullptr;
    m_tower = nullptr;
    m_sailing = nullptr;

	platform_ = "ios";
	if (plat == "android")
	{
		platform_ = plat;
	}


    maf[40] = 5300;
	maf[44] = 6100;
	maf[46] = 6500;
	maf[47] = 6700;
	maf[48] = 6900;

    //fCurr = std::chrono::steady_clock::now();
    //for (int i = 0; i < 10000; i++)
    //{
	http::http_request request;
	if (platform_ == "ios")
	{
		request.type = HTTP_GET;
		request.url = "/";
		request.host = "www.baidu.com";

		request.head["Proxy-Connection"] = "keep-alive";
		request.head["Keep-Alive"] = "timeout = 10,max = 0";
		request.head["Accept-Encoding"] = "gzip";
		request.head["Accept"] = "text/html, application/xhtml+xml, */*";
		request.head["User-Agent"] = "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko";
		request.head["Cookie"] = "BAIDUID=F0435850FA49644A2253602E33CEABBB:FG=1";
		request.head["Accept-Language"] = "zh-CN";

		std::stringstream ss;
		ss << request.body.size();
		request.head["Content-Length"] = ss.str();

		m_manager->post_https(request, std::bind(&Client::webreward_response, this, std::placeholders::_1, 
			std::string("117.86.64.21"), std::string("8998"), std::chrono::steady_clock::now()));
	}
    //}

	if (platform_ == "ios")
	{
		//m_pinger = new pinger(io_service);
		//boost::thread* thr = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
	}
}

#include "zlib/zlib.h"



bool Client::webreward_response(http::http_response& response, std::string& addr, std::string& port, std::chrono::steady_clock::time_point& tim)
{
    static int count = 0;
    count++;

	std::string strrr;
	strrr.resize(150000);
	unsigned i = 150000;

	int res = gzuncompress((Bytef*)response.body.c_str(), response.body.size(), (Bytef*)strrr.c_str(), (uLongf*)&i);
	

	std::chrono::steady_clock::time_point fLast = std::chrono::steady_clock::now();
	std::chrono::duration<int, std::ratio<1000000>> usetime = std::chrono::duration_cast<std::chrono::duration<int, std::ratio<1000000>>>(fLast - tim);

	//std::cout << usetime.count();

	if (response.body.find("sds") != std::string::npos)
	{
		std::cout << addr << " : " << port << "\n";

		std::string filename = "proxy.txt";
#ifdef _WIN32
		FILE * m_hFile = _fsopen(filename.c_str(), "a+", _SH_DENYWR);
#else
		m_hFile = fopen(filename.c_str(), "a+");
#endif
		std::string ss;
		ss = addr;
		ss += " : ";
		ss += port;
		ss += "\t\t use_time: ";
		ss += Helper::Int32ToString(usetime.count());
		ss += "\n";
		fwrite(ss.c_str(), ss.length(), 1, m_hFile);
		fflush(m_hFile);
		fclose(m_hFile);
	}

    if (count == 10000)
    {
        std::chrono::steady_clock::time_point fLast = std::chrono::steady_clock::now();
		std::chrono::duration<int, std::ratio<1000>> usetime = std::chrono::duration_cast<std::chrono::duration<int, std::ratio<1000>>>(fLast - tim);

        //std::cout << usetime.count();

//		if (response.code == 200)
//		{
//			std::cout << addr << " : " << port << "\n";
//
//			std::string filename = "proxy.txt";
//#ifdef _WIN32
//			FILE * m_hFile = _fsopen(filename.c_str(), "a+", _SH_DENYWR);
//#else
//			m_hFile = fopen(filename.c_str(), "a+");
//#endif
//			std::string ss;
//			ss = addr;
//			ss += " : ";
//			ss += port;
//			ss += "\t\t use_time: ";
//			ss += Helper::Int32ToString(usetime.count());
//			ss += "\n";
//			fwrite(ss.c_str(), ss.length(), 1, m_hFile);
//			fflush(m_hFile);
//			fclose(m_hFile);
//		}
    }

	return true;
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

	delete m_manager;
	delete m_pTimerManager;

	io_service.stop();
	//delete m_pinger;
}

void Client::run(double diff)
{
	m_manager->update();
	std::chrono::steady_clock::time_point _update_time = std::chrono::steady_clock::now();
	m_pTimerManager->Update(_update_time);
 
    m_pTimeWheel->Update();


	if (platform_ == "ios")
	{
		http::http_request request;

		static int ports[] = {
			80,
			843,
			808,
			3128,
			8000,
			8008,
			8080,
			8081,
			8082,
			8090,
			8088,
			8089,
			8111,
			8118,
			8123,
			8668,
			8888,
			8998,
			9000,
			9797,
			9898,
			9999,
			63000,
			55336,
			62222,
		};

		if (ip1 >= 224)
			return;

		if (m_manager->get_conn_size() > 5000)
			return;

		ip4 += 1;
		if (ip4 > 255)
		{
			ip4 = 0;
			ip3 += 1;
			if (ip3 > 255)
			{
				ip3 = 0;
				ip2 += 1;
				if (ip2 > 255)
				{
					ip2 = 0;
					ip1 += 1;
					if (ip1 > 255)
						return;

					if (ip1 == 10)
						ip1 = 11;

					if (ip1 == 127)
						ip1 = 128;
				}

				if (ip1 == 168 && ip2 == 192)
					ip2 = 193;

				if (ip1 == 169 && ip2 == 254)
					ip2 = 255;

				if (ip1 == 172 && ip2 == 16)
					ip2 = 32;

				if (ip1 == 192 && ip2 == 168)
					ip2 = 169;

				//std::cout << ip1 << ":" << ip2 << "\n";
			}
			//std::cout << ip1 << ":" << ip2 << ":" << ip3 << "\n";
		}
		if (request.host.empty())
		{
			request.type = HTTP_GET;
			request.url = "http://mall.geely.com/a.txt";
			request.host = "mall.geely.com";

			request.head["Proxy-Connection"] = "keep-alive";
			request.head["Keep-Alive"] = "timeout = 10,max = 0";
			request.head["Accept-Encoding"] = "gzip, deflate";
			request.head["Accept"] = "text/html, application/xhtml+xml, */*";
			request.head["User-Agent"] = "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko";
			request.head["Cookie"] = "BAIDUID=F0435850FA49644A2253602E33CEABBB:FG=1";
			request.head["Accept-Language"] = "zh-CN";
		}

		std::string addr = Helper::Int32ToString(ip1);
		addr += ".";
		addr += Helper::Int32ToString(ip2);
		addr += ".";
		addr += Helper::Int32ToString(ip3);
		addr += ".";
		addr += Helper::Int32ToString(ip4);

		for (int i = 0; i < sizeof(ports) / sizeof(int); i++)
		{
			std::string por = Helper::Int32ToString(ports[i]);
			//m_manager->post(request, std::bind(&Client::webreward_response, this, std::placeholders::_1, addr, por, _update_time), addr, por);
		}

		//m_pinger->start_send(addr.c_str());
		//m_pinger->start_send("115.28.167.223");
		
		//m_manager->post(request, std::bind(&Client::webreward_response, this, std::placeholders::_1,
			//std::string("115.28.167.223"), std::string("80"), _update_time), "121.199.9.161", "3128");
	}
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

        if (mid % 100 > 10)
        {
            Chapter temp;
            temp.cid = mid / 100 + 1;
            m_pChapter[temp.cid] = temp;
            mid = temp.cid * 100 + 1;
        }

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
				if (exit->second.score != 3 && exit->second.todayTimes < 5)
				{
					fight(exit->second.mid);
					return;
				}
			}
		}
	}

	if (fight_mode == 3)
	{
		fight(map_high * 100 + map_low);
	}

    if (fight_mode == 4)
    {
        std::map<int, int>::iterator it = maf.begin();
        for (; it != maf.end(); ++it)
        {
            auto cpit = m_pChapter.find(it->first);
            if (cpit != m_pChapter.end())
            {
                if (cpit->second.pres >= it->second)
                    continue;

                std::map<int, Explore>::iterator exit = cpit->second.explores.begin();
                for (; exit != cpit->second.explores.end(); ++exit)
                {
                    if (exit->second.todayTimes < 5)
                    {
                        fight(exit->second.mid);
                        return;
                    }
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
	if (platform_ == "ios")
	{
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

	if (platform_ == "android")
	{
		request.type = HTTP_POST;
		request.url = "/sdk/as_login.php?method=ticket";
		request.host = "auth.thedream.cc";
		request.head["Content-Type"] = "application/x-www-form-urlencoded; charset=UTF-8";
		//request.head["User-Agent"] = "Dalvik/1.6.0 (IOS; U; iphone 6.1)";
		request.head["User-Agent"] = "Dalvik/1.6.0 (Linux; U; Android 4.4.2; SM-G900F Build/KOT49H)";
		request.head["Connection"] = "Keep-Alive";
		request.head["Keep-Alive"] = "timeout= 5,max = 0";
		request.head["Accept-Encoding"] = "gzip";

		request.body = "app_id=66&os=1&ch_id=103000";
		std::stringstream ss;
		ss << request.body.size();
		request.head["Content-Length"] = ss.str();

		m_manager->post(request, std::bind(&Client::as_login_ticket_response, this, std::placeholders::_1));
	}
}

bool Client::as_login_ticket_response(http::http_response& response)
{
	if (response.code != 200)
	{
		as_login_ticket();
		return false;
	}

	Json::Reader reader;
	Json::Value value;
	reader.parse(response.body, value, false);

	http::http_request request;

	if (platform_ == "ios")
	{
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
	}
	else
	{
		request.type = HTTP_POST;
		request.url = "/sdk/as_login.php?method=login";
		request.host = "auth.thedream.cc";
		request.head["Content-Type"] = "application/x-www-form-urlencoded; charset=UTF-8";
		request.head["User-Agent"] = "Dalvik/1.6.0 (Linux; U; Android 4.4.2; SM-G900F Build/KOT49H)";
		request.head["Connection"] = "Keep-Alive";
		request.head["Keep-Alive"] = "timeout = 5, max = 1";
		request.head["Accept-Encoding"] = "gzip";

		std::stringstream ss;

		request.body = "app_id=66&upw=e3dd4b27ec803a0c489b448c0740e197523a294e&uid=a65508333&ticket=";
		request.body += value["ticket"].asString();
		request.body += "&ch_id=103000&os=1";

		ss << request.body.size();
		request.head["Content-Length"] = ss.str();
	}

	m_manager->post(request, std::bind(&Client::as_login_response, this, std::placeholders::_1));

	return true;
}

bool Client::as_login_response(http::http_response& response)
{
	if (response.code != 200)
	{
		as_login_ticket();
		return false;
	}
	Json::Reader reader;
	Json::Value value;
	reader.parse(response.body, value, false);

	token = value["token"].asString();

	loggedin();

	return true;
}

void Client::loggedin()
{
	http::http_request request;
	if (platform_ == "ios")
	{
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
		content["idfa"] = ios_deviceid;
		content["ryos"] = "ios";
		content["lats"] = "1";
		content["ryosversion"] = "9.2.1";
		content["openudid"] = "";
		content["ry_sdk_name"] = "ios_channel";
		content["rydevicetype"] = "iPhone";
		content["deviceid"] = ios_deviceid;
		content["ry_sdk_ver"] = "2.0.0";
		content["idfv"] = "BFD680A7-1CDA-4940-B874-ACF9E40FE4FC";



		value["context"] = content;
		value["appid"] = "f7e3fd6fe2a69e4b70660ed30c6d6486";
		value["what"] = "loggedin";
		value["when"] = Helper::Timet2String(time(NULL));
		value["who"] = "a65508333";

		request.body = writer.write(value);
	}
	else
	{
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
		content["ry_sdk_ver"] = "1.0.4";
		content["androidid"] = "f1e69f274477cf33";
		content["rydevicetype"] = "SM-G900F";
		content["op"] = "............";
		content["ryosversion"] = "4.4.2";
		content["imei"] = "127468846146468";
		content["ry_sdk_name"] = "android_channel";
		content["istablet"] = "table";
		content["ryos"] = "19";
		content["network"] = "WIFI";
		content["channelid"] = "_default_";
		content["deviceid"] = "127468846146468";


		value["context"] = content;
		value["appid"] = "c941f77fcfd28cff0afeb1547c530565";
		value["what"] = "loggedin";
		value["when"] = Helper::Timet2String(time(NULL));
		value["who"] = "a65508333";

		request.body = writer.write(value);
	}
	//request.pend_flag = "0\r\n\r\n";
	m_manager->post(request, std::bind(&Client::loggedin_response, this, std::placeholders::_1));

}
bool Client::loggedin_response(http::http_response& response)
{
	if (response.code != 200)
	{
		as_login_ticket();
		return false;
	}
	http::http_request request;
	if (platform_ == "ios")
	{
		request.type = HTTP_GET;
		request.url = "/s20043/port/dream/login_cb.php?u=&c=";
		request.url += ios_deviceid;
		request.url += "&et=iPhone6,2_Darwin_15.0.0&t=";
		request.url += token;
		request.url += "&uid=&plat_lj=&plat_lj_uid=&transfer=&v=";
        request.url += version;
        request.url += "&plat=100000";
		request.host = "pl-game.thedream.cc";
		request.head["Accept"] = "*/*";

		request.pend_flag = "0\r\n\r\n";
	}
	else
	{
		request.type = HTTP_GET;
		request.url = "/s20042/port/dream/login_cb.php?u=&c=";
		request.url += android_deviceid;
		request.url += "&et=SM-G900F_19_4.4.2&t=";
		request.url += token;
        request.url += "&uid=&plat_lj=&plat_lj_uid=&transfer=&v=";
        request.url += version;
        request.url += "&plat=103000";
		request.host = "pl-game.thedream.cc";
		request.head["Accept"] = "*/*";

		request.pend_flag = "0\r\n\r\n";
	}
	m_manager->post(request, std::bind(&Client::login_cb_response, this, std::placeholders::_1));

	return true;
}

bool Client::login_cb_response(http::http_response& response)
{
	if (response.code != 200)
	{
		as_login_ticket();
		return false;
	}
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
	if (platform_ == "ios")
	{
		request.url += ios_deviceid;
		request.url += "&et=iPhone6,C2_Darwin_15.0.0&t=";
	}
	else
	{
		request.url += android_deviceid;
		request.url += "&et=SM-G900F_19_4.4.2&t=";
	}	
	request.url += token;
	request.url += "&is_guest=0&e=";
	request.url += expire;
	request.url += "&s=";
	request.url += sig;
    if (platform_ == "ios")
    {
        request.url += "&plat_lj=&v=";
        request.url += version;
        request.url += "&&sys=ios&plat=100000";
    }
    else
    {
        request.url += "&plat_lj=&v=";
        request.url += version;
        request.url += "&&sys=android&plat=103000";
    }
	request.host = "pl-game.thedream.cc";
	request.head["Accept"] = "*/*";
	request.pend_flag = "0\r\n\r\n";

	m_manager->post(request, std::bind(&Client::login_response, this, std::placeholders::_1));

	return true;
}

bool Client::login_response(http::http_response& response)
{
	if (response.code != 200)
	{
		as_login_ticket();
		return false;
	}
	cookie = response.head["Set-Cookie"];
	int pos = cookie.find(";");
	cookie = cookie.substr(0, pos);

	init();

	return true;
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
    request.url = " /s20042/port/interface.php?s=User&m=get&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::request_player_data_response, this, std::placeholders::_1));
}
void Client::request_player_data_response(Json::Value& value)
{
	if (value.isMember("code"))
	{
		if (Helper::StringToInt32(value["code"].asString()) == NEED_PERMISSION)
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
	startTower();
	signin();
	onlineGift();
	buyPow();
	CalendarSign();
  	startSail();
    worldBoss();
    shared();
    guildSign();
    GuildBoss();
    CupSignUp();
    ManorGet();
	Wish();

	//luckyTurnHappyReset(2400);
	//luckyTurnHappyAward(2, 2400);
    if (m_step != nullptr)
    {
        m_pTimerManager->RemoveTimer(m_step);
    }

	m_step = m_pTimerManager->AddIntervalTimer(0.2, std::bind(&Client::checkStep, this));
}

void Client::post(http::http_request request, std::function<void(Json::Value&)> value)
{
	m_manager->post(request, std::bind(&Client::post_callback, this, std::placeholders::_1, value));
}

bool Client::post_callback(http::http_response response, std::function<void(Json::Value&)> func)
{
	//assert(response.code == 200);
	if (response.code != 200)
	{
		return true;
	}

	std::string body = (response.body);

	Json::Reader reader;
	Json::Value value;
	int pos = body.find("{");
	int rpos = body.rfind("}");

	if (pos == std::string::npos || rpos == std::string::npos)
		return true;
	body = body.substr(pos, rpos - pos + 1);
	reader.parse(body, value, false);

	if (!func._Empty())
		func(value);

	return true;
}

void Client::request_init_data()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=System&m=init&a=%7B%7D&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::request_init_data_response, this, std::placeholders::_1));
}

void Client::request_init_data_response(Json::Value& value)
{
	if (value.isMember("data"))
	{
		endurance = value["data"]["user"]["endurance"].asInt();
		pow = value["data"]["user"]["pow"].asInt();
        //当前最高关卡
        int mid = value["data"]["user"]["mid"].asInt();
        int weektype = value["data"]["weeklyTypes"][0].asInt();
        //3 棋盘

        m_wid.clear();
        m_wid_vec.clear();
        Json::Value warriors = value["data"]["warriors"];
        for (int i = 0; i < (int)warriors.size(); i++)
        {
            if (warriors[i]["wid"].asInt()>=1000)
                continue;
            m_wid.insert(warriors[i]["wid"].asInt());
        }

        for (auto it = m_wid.begin(); it != m_wid.end(); it++)
        {
            m_wid_vec.push_back(*it);
        }
	}

	//fight(map_high * 100 + map_low);
}

void Client::get_fight_map()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Explore&m=getC&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::get_fight_map_response, this, std::placeholders::_1));
}

void Client::get_fight_map_response(Json::Value& value)
{
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
	request.url += Helper::Int32ToString(cid);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::get_fight_map_sub_response, this, cid, std::placeholders::_1));
}

void Client::get_fight_map_sub_response(int cid, Json::Value& value)
{
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
	request.url += Helper::Int32ToString(map);
    request.url += "\"}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";
	post(request, std::bind(&Client::playgame_response, this, map, std::placeholders::_1));
}

void Client::playgame_response(int map, Json::Value& value)
{
	if (value.isMember("code"))
	{
		std::string codess = value["code"].asString();
		int code = Helper::StringToInt32(codess);

		switch (code)
		{
		case NOON_POWER:
			std::cout << platform_<< " error code:" << codess << codestr[code] << "\n";
			return;
		case Success:
			pow -= 1;
			{
				auto it = m_pChapter.find(map / 100);
				if (it != m_pChapter.end())
				{
					it->second.pres++;
				}
			}

		case NOON_WAIT:
			break;
		case NEED_PERMISSION:
			return;
		case NOT_EXIST:
			fight(((map / 100) + 1) * 100 + 1);
		case NOON_ACCESS:
			m_pChapter.erase(map / 100);
			run_fight();
			return;
		default:
			get_fight_map();
			return;
		}
	}

	if (value["data"].isMember("pve"))
	{
		if (value["data"]["pve"]["grade"].asInt() != 0)
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
	m_fight = m_pTimerManager->AddIntervalTimer(1, std::bind(&Client::run_fight, this));
	//run_fight();
}

void Client::set_mode(std::string& param)
{
	std::vector<std::string> vec;
	Helper::SplitString(param, " ", vec);
	if (vec.size() < 1)
		return;

	fight_mode = Helper::StringToInt32(vec[0]);
	if (fight_mode == 3)
	{
		map_high = vec.size() >= 2 ? Helper::StringToInt32(vec[1]) : 0;
		map_low = vec.size() >= 3 ? Helper::StringToInt32(vec[2]) : 0;
	}
}

void Client::set_fight(std::string& param)
{
    std::vector<std::string> vec;
    Helper::SplitString(param, " ", vec);
    if (vec.size() < 1)
        return;

    fight_low = Helper::StringToInt32(vec[0]);
    fight_high = Helper::StringToInt32(vec[1]);
}

void Client::set_ip(std::string& param)
{
	std::vector<std::string> vec;
	Helper::SplitString(param, " ", vec);
	if (vec.size() < 4)
		return;

	ip1 = Helper::StringToInt32(vec[0]);
	ip2 = Helper::StringToInt32(vec[1]);
	ip3 = Helper::StringToInt32(vec[2]);
	ip4 = Helper::StringToInt32(vec[3]);
}

void Client::get_challenge()
{
	if (m_challenge == NULL)
	{
		m_challenge = m_pTimerManager->AddIntervalTimer(5, std::bind(&Client::get_challenge, this));
	}

	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Challenge&m=init&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::get_challenge_response, this, std::placeholders::_1));
}
void Client::get_challenge_response(Json::Value& value)
{
	if (value.isMember("data"))
	{
		int trank = value["data"]["chAvailRanks"][1].asInt();
		int tid = 0;

		for (int i = 0; i < (int)value["data"]["chRanks"].size(); i++)
		{
			if (value["data"]["chRanks"][i]["rank"].asInt() == trank)
				tid = value["data"]["chRanks"][i]["uid"].asInt();
		}

        {
            int srank = value["data"]["challenge"]["rank"].asInt();
            http::http_request request;
            request.type = HTTP_GET;
            request.url = "/s20042/port/interface.php?s=Challenge&m=challenge&a={\"sRank\":";
            request.url += Helper::Int32ToString(srank);
            request.url += ",\"tRank\":";
            request.url += Helper::Int32ToString(trank);
            request.url += ",\"tid\":";
            request.url += Helper::Int32ToString(tid);
            request.url += "}&v=";
            request.url += version;
            request.url += "&sys=";
            request.url += platform_;
            request.host = "pl-game.thedream.cc";
            request.head["Cookie"] = cookie;
            request.pend_flag = "0\r\n\r\n";

            post(request, std::bind(&Client::challenge_response, this, std::placeholders::_1));
        }

        {
            for (int j = 0; j < (int)value["data"]["chMall"]["items"].size(); j++)
            {
                Json::Value& item = value["data"]["chMall"]["items"][j];
                if (item["type"].asInt() == 7 && item["buyType"].asInt() == 1)
                {
                    http::http_request request;
                    request.type = HTTP_GET;
                    request.url = "/s20042/port/interface.php?s=Challenge&m=exchange&a={\"id\":";
                    request.url += Helper::Int32ToString(item["cId"].asInt());
                    request.url += "}&v=";
                    request.url += version;
                    request.url += "&sys=";
                    request.url += platform_;
                    request.host = "pl-game.thedream.cc";
                    request.head["Cookie"] = cookie;
                    request.pend_flag = "0\r\n\r\n";

                    post(request, std::function<void(Json::Value&)>());
                }
            }
        }
	}
}

void Client::challenge_response(Json::Value& value)
{
	if (value.isMember("code"))
	{
		std::string codess = value["code"].asString();
		int code = Helper::StringToInt32(codess);

		switch (code)
		{
		case NOON_DEDURENCE:
			std::cout << "error code:" << codess << codestr[code] << "\n";
			m_pTimerManager->RemoveTimer(m_challenge);
			m_challenge = NULL;
			return;
		case Success:
			endurance -= 1;
		case NOON_WAIT:
			break;
		default:
			break;
		}
	}
}

void Client::request_eat()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Activity&m=eat&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::request_eat_response, this, std::placeholders::_1));
}

void Client::request_eat_response(Json::Value& value)
{
	if (Helper::StringToInt32(value["code"].asString()) == 0)
	{
		pow = value["data"]["user"]["pow"].asInt();
	}
}

void Client::getHero()
{
	if (m_hero == NULL)
	{
		m_hero = m_pTimerManager->AddIntervalTimer(2, std::bind(&Client::getHero, this));
	}

	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Explore&m=getHero&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::getHero_response, this, std::placeholders::_1));
}

void Client::getHero_response(Json::Value& value)
{
	if (Helper::StringToInt32(value["code"].asString()) == 0)
	{
		std::map<int, int> mm;
		Json::Value explores = value["data"]["explores"];
		for (int i = 0; i < (int)explores.size(); i++)
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
		request.url += Helper::Int32ToString(mm.begin()->first);
        request.url += "\"}&v=";
        request.url += version;
        request.url += "&sys=";
		request.url += platform_;
		request.host = "pl-game.thedream.cc";
		request.head["Cookie"] = cookie;
		request.pend_flag = "0\r\n\r\n";
		m_manager->post(request, std::function<bool(http::http_response&)>());
	}
}

void Client::startTower()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Tower&m=start&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::startTower_response, this, std::placeholders::_1));

}
void Client::startTower_response(Json::Value& value)
{
	startTowerFight();
}
void Client::startTowerFight()
{
	if (m_tower == NULL)
	{
		m_tower = m_pTimerManager->AddIntervalTimer(5, std::bind(&Client::startTowerFight, this));
	}
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Tower&m=fight&a={\"lv\":3}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::startTowerFight_response, this, std::placeholders::_1));
}
void Client::startTowerFight_response(Json::Value& value)
{
	if (TOWER_DIE == Helper::StringToInt32(value["code"].asString()))
	{
		m_pTimerManager->RemoveTimer(m_tower);
		m_tower = NULL;
		return;
	}

	if (Helper::StringToInt32(value["code"].asString()) == 0)
	{
		int size = value["data"]["tower"]["buffstr"].size();
		if (size != 0)
		{
			int id = value["data"]["tower"]["buffstr"][size - 1]["id"].asInt();

			http::http_request request;
			request.type = HTTP_GET;
			request.url = "/s20042/port/interface.php?s=Tower&m=setBuff&a={\"id\":";
			request.url += Helper::Int32ToString(id);
            request.url += "}&v=";
            request.url += version;
            request.url += "&sys=";
			request.url += platform_;
			request.host = "pl-game.thedream.cc";
			request.head["Cookie"] = cookie;
			request.pend_flag = "0\r\n\r\n";

			m_manager->post(request, std::function<bool(http::http_response&)>());
		}
		if (value["data"]["win"].asBool() == false)
		{
			m_pTimerManager->RemoveTimer(m_tower);
			m_tower = NULL;
			return;
		}
	}
}

void Client::worldBoss()
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WorldBoss&m=get&a={}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::bind(&Client::worldBoss_response, this, std::placeholders::_1));
}

void Client::worldBoss_response(Json::Value& value)
{
    if (value["data"]["worldBoss"]["playNum"].asInt() >= 3)
        return;

    startWorldBoss();

}

void Client::startWorldBoss()
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WorldBoss&m=start&a={}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::bind(&Client::startWorldBoss_response, this, std::placeholders::_1));
}

void Client::startWorldBoss_response(Json::Value& value)
{
    if (platform_ != "ios")
        return;

    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WorldBoss&m=play&a={\"damages\":[";
    int loop_count = Helper::GetRandom(74,75);
    for (int i = 0; i < loop_count; i++)
    {
        double range = Helper::GetRandom(0.0, 1.0);
		int fight_value = fight_low;
        if (range > 0.8)
        {
            fight_value = fight_high;
        }
        if (i == 0)
            fight_value = fight_high;

        request.url += Helper::Int32ToString(fight_value);
        if (i != loop_count - 1)
            request.url += ",";
    }
    request.url += "],\"sign\":\"5a8f8013288c5e45ff1620ded5fe28f4\"}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::bind(&Client::playWorldBoss_response, this, std::placeholders::_1));
}

void Client::playWorldBoss_response(Json::Value& value)
{
    worldBoss();
}

void Client::guildSign()
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Guild&m=sign&a={}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::function<void(Json::Value&)>());
}

void Client::GuildBoss()
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=GuildBoss&m=start&a={}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::bind(&Client::GuildBoss_response, this, std::placeholders::_1));
}

void Client::GuildBoss_response(Json::Value& value)
{
    if (value["data"]["bossOpen"].asBool() == false)
        return;

    int times = value["data"]["guildBossUserDetail"]["availTimes"].asInt();
    
    if (times == 0)
        return;

    m_pTimerManager->AddIntervalTimer(11, std::bind(&Client::GuildBossAttack, this), times);
}

void Client::GuildBossAttack()
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=GuildBoss&m=attackBoss&a={\"isPay\":0}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::bind(&Client::GuildBossAttack_response, this, std::placeholders::_1));
}

void Client::GuildBossAttack_response(Json::Value& value)
{
    if (value["data"]["guildBossUserDetail"]["bonusScores"].asInt() >= 10)
    {
        http::http_request request;
        request.type = HTTP_GET;
        request.url = "/s20042/port/interface.php?s=GuildBoss&m=autoAttackBoss&a={}&v=";
        request.url += version;
        request.url += "&sys=";
        request.url += platform_;
        request.host = "pl-game.thedream.cc";
        request.head["Cookie"] = cookie;
        request.pend_flag = "0\r\n\r\n";

        post(request, std::function<void(Json::Value&)>());
    }
}

void Client::shared()
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Activity&m=share&a={\"sign\":\"f40e667a047e7b16da7f3204b3760bd5\",\"ts\":";
    request.url+=Helper::Int32ToString((int)time(NULL));
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::bind(&Client::shared_response, this, std::placeholders::_1));
}

void Client::shared_response(Json::Value& value)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Activity&m=shareZh&a={\"sign\":\"20d1b33f523e52b57e03b5dfb575cf47\",\"ts\":";
    request.url += Helper::Int32ToString((int)time(NULL));
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::function<void(Json::Value&)>());

    static int ios_count = 0;
    static int android_count = 0;

    if (platform_ == "ios")
    {
        ios_count++;
        if (ios_count > 3)
            return;
    }

    if (platform_ == "android")
    {
        android_count++;
        if (android_count > 3)
            return;
    }

    shared();
}

void Client::startSail()
{
	if (m_sailing == NULL)
	{
		m_sailing = m_pTimerManager->AddIntervalTimer(5, std::bind(&Client::sailing, this));
	}

	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WeeklyActivity&m=startSail&a={\"level\":3}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::startSail_response, this, std::placeholders::_1));
}

void Client::startSail_response(Json::Value& value)
{
	boat();
}

void Client::boat()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WeeklyActivity&m=boat&a={\"boatId\":1}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::boat_response, this, std::placeholders::_1));
}

void Client::boat_response(Json::Value& value)
{

}

void Client::sailing()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WeeklyActivity&m=sailing&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::sailing_response, this, std::placeholders::_1));
}
void Client::sailing_response(Json::Value& value)
{
	if (SAILING_OVER == Helper::StringToInt32(value["code"].asString()))
	{
		m_pTimerManager->RemoveTimer(m_sailing);
		m_sailing = NULL;
	}

	if (Helper::StringToInt32(value["code"].asString()) == 0)
	{
		Json::Value weeklyActivity = value["data"]["weeklyActivity"];

        //sailing
        int pos = 0;


            if (weeklyActivity["type"].asInt() == 2)
            {
                switch (weeklyActivity["sail_type"].asInt())
                {
                case 0:
                    sailing();
                    break;
                case 2:
                    sailing_type2();
                    break;
                case 3:
                    sailing_type3();
                    break;
                case 5:
                    sailing_type5();
                    break;
                case 6:
                    sailing_type6();
                    break;
                default:
                    std::cout << "sail_type:" << weeklyActivity["sail_type"].asInt() << "\n";
                    break;
                }
            }
        
	}
}

void Client::sailing_type2()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WeeklyActivity&m=pirate&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::function<void(Json::Value&)>());
}
void Client::sailing_type3()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WeeklyActivity&m=island&a={\"isBattle\":true}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::function<void(Json::Value&)>());
}
void Client::sailing_type5()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WeeklyActivity&m=pk&a={\"isBattle\":true}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::function<void(Json::Value&)>());
}
void Client::sailing_type6()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=WeeklyActivity&m=hill&a={\"isDouble\":false}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::function<void(Json::Value&)>());
}

void Client::checkStep()
{
	switch (step_)
	{
	case INIT:
		delete_friend();
		break;
	case DELETE_FRIEND:
		break;
	case DELETE_FRIEND_RESPONSE:
		add_friend();
		break;
	case ADD_FRIEND:
		break;
	case ADD_FRIEND_RESPONSE:
		get_new();
		break;
	case GET_NEW:
		break;
	case GET_NEW_RESPONSE:
		accept_add_friend();
		break;
	case ACCEPT_ADD_FRIEND:
		break;
	case ACCEPT_ADD_FRIEND_RESPONSE:
		send_power();
		break;
	case SEND_POWER:
		break;
	case SEND_POWER_RESPONSE:
		accept_power();
		break;
	case ACCEPT_POWER:
		break;
	case REMOVE_TIMER:
		if (m_step != NULL)
		{
			m_pTimerManager->RemoveTimer(m_step);            
		}
        step_ = CHECKSTEP;
        m_step = m_pTimerManager->AddTriggerTimer(0, 1, 0, std::bind(&Client::checkStep, this));
		break;
    case CHECKSTEP:
        if (m_step != NULL)
        {
            m_pTimerManager->RemoveTimer(m_step);          
        }
        step_ = INIT;
        m_step = m_pTimerManager->AddIntervalTimer(1, std::bind(&Client::checkStep, this));
	default:
		break;
	}
}

void Client::delete_friend()
{
	if (platform_ == "ios")
		return;

	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Friend&m=deleteFriends&a={\"fid\":1021,\"fids\":[1021]}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::delete_friend_response, this, std::placeholders::_1));

	step_ = DELETE_FRIEND;
}

void Client::delete_friend_response(Json::Value& value)
{
	step_ = DELETE_FRIEND_RESPONSE;
	//std::cout << "delete_friend_response by android\n";
}

void Client::add_friend()
{
	if (platform_ == "ios")
		return;

	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Friend&m=sendRequest&a={\"fid\":1021}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::add_friend_response, this, std::placeholders::_1));

	step_ = ADD_FRIEND;
}

void Client::add_friend_response(Json::Value& value)
{
	step_ = ADD_FRIEND_RESPONSE;
	//std::cout << "add_friend_response by android\n";
}

void Client::get_new()
{
	if (platform_ != "ios")
		return;

	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Friend&m=getNewRequest&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::get_new_response, this, std::placeholders::_1));

	step_ = GET_NEW;
}

void Client::get_new_response(Json::Value& value)
{
	step_ = GET_NEW_RESPONSE;
	//std::cout << "get_new_response by ios\n";
}

void Client::accept_add_friend()
{
	if (platform_ != "ios")
		return;

	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Friend&m=accept&a={\"fid\":1743}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::accept_add_friend_response, this, std::placeholders::_1));

	step_ = ACCEPT_ADD_FRIEND;
}

void Client::accept_add_friend_response(Json::Value& value)
{
	step_ = ACCEPT_ADD_FRIEND_RESPONSE;
	//std::cout << "accept_add_friend_response by ios\n";
}

void Client::send_power()
{
	if (platform_ == "ios")
		return;

	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Friend&m=sendGift&a={\"fid\":1021}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::send_power_response, this, std::placeholders::_1));

	step_ = SEND_POWER;
}

void Client::send_power_response(Json::Value& value)
{
	step_ = SEND_POWER_RESPONSE;
	//std::cout << "send_power_response by android\n";

	//对方接收体力次数太多
	if (value["code"].asString() == "4416")
		step_ = REMOVE_TIMER;
}

void Client::accept_power()
{
	if (platform_ != "ios")
		return;

	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Friend&m=receiveAndSendGiftAll&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::accept_power_response, this, std::placeholders::_1));

	step_ = ACCEPT_POWER;
}

void Client::accept_power_response(Json::Value& value)
{
	static int count = 0;
	step_ = ACCEPT_POWER_RESPONSE;
}

void Client::signin()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Activity&m=signIn&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::function<void(Json::Value&)>());
}

void Client::onlineGift()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Activity&m=onlineGift&a={}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::function<void(Json::Value&)>());
}

void Client::buyPow()
{
	http::http_request request;
	request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Mall&m=buyPow&a={\"num\":1}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::function<void(Json::Value&)>());
}

void Client::CalendarSign()
{
	time_t tNow = time(NULL);
	tm tmNow = *localtime(&tNow);
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Activity&m=CalendarSign&a={\"id\":";
	request.url += Helper::Int32ToString(tmNow.tm_mday);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::function<void(Json::Value&)>());
}

void Client::CardDraw(int type)
{
	time_t tNow = time(NULL);
	tm tmNow = *localtime(&tNow);
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Card&m=draw&a={\"cost\":0,\"type\":";
	request.url += Helper::Int32ToString(type);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, std::function<void(Json::Value&)>());
}

void Client::Activeness(int point)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Activeness&m=getReward&a={\"point\":";
    request.url += Helper::Int32ToString(point);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::function<void(Json::Value&)>());
}

void Client::CupSignUp()
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Cup&m=signUp&a={}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::function<void(Json::Value&)>());
}

void Client::ManorGet()
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Manor&m=get&a={}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::bind(&Client::ManorGet_response, this, std::placeholders::_1));
}

void Client::ManorGet_response(Json::Value& value)
{
    int tNow = value["ts"].asInt();

	if (platform_ == "ios")
	{
		std::cout << "ManorGet: " << Helper::Timet2String(tNow) << "\n";
	}
    std::vector<int> task_vec;
    std::set<int> worker;

    for (int i = 0; i < (int)value["data"]["manorBuildings"].size(); i++)
    {
        Json::Value& temp = value["data"]["manorBuildings"][i];

        int type = temp["type"].asInt();
        int lv = temp["lv"].asInt();

        int count = m_wid_vec.size();
        //酒楼
        if (type == 1)
        {
            Json::Value& tasks = temp["tasks"];

            Json::Value& newTasks = temp["newTasks"];
            Json::ValueIterator it = tasks.begin();
            for (; it != tasks.end(); ++it)
            {
                Json::Value task = *it;           
                ManortaskReward(task["w"].asInt());
                worker.insert(task["w"].asInt());
            }

            it = newTasks.begin();
            for (; it != newTasks.end(); ++it)
            {
                task_vec.push_back((*it).asInt());
            }
        }

        if (type == 2)
        {
            Json::Value& warriors = temp["warriors"];

            Json::ValueIterator it = warriors.begin();
            for (; it != warriors.end(); ++it)
            {
                Json::Value warrior = *it;

                ManordojoReward(warrior["p"].asInt());
                worker.insert(warrior["w"].asInt());
            }
        }

        //钓鱼
        if (type == 4)
        {
            Json::Value& orders = temp["orders"];
            Json::ValueIterator it = orders.begin();
            for (; it != orders.end(); ++it)
            {
                Json::Value order = *it;

                ManorfishReward(order["w"].asInt());
                worker.insert(order["w"].asInt());
            }    
        }

        if (type == 3)
        {
            Json::Value& orders = temp["orders"];
            Json::ValueIterator it = orders.begin();
            for (; it != orders.end(); ++it)
            {
                Json::Value order = *it;

                ManororderReward(order["w"].asInt());   
                worker.insert(order["w"].asInt());
            }
        }
    }
    
    int count = m_wid_vec.size() - 1;
    for (size_t i = 0; i < task_vec.size(); i++)
    {
		if (count <= 0)
            return;

        if (worker.find(m_wid_vec[count]) != worker.end())
        {
            i--;
            count--;
            continue;
        }

        count--;
        ManordoTask(task_vec[i], m_wid_vec[count]);
    }

    for (int i = 1; i < 5; i++)
    {
		if (count <= 0)
            return;

        if (worker.find(m_wid_vec[count]) != worker.end())
        {
            i--;
            count--;
            continue;
        }

        count--;
        ManordojoTrain(i, m_wid_vec[count]);
    }

    int Order[] = { 301, 302, 303, 202 };
    for (int i = 0; i < 4; i++)
    {
		if (count <= 0)
            return;

        if (worker.find(m_wid_vec[count]) != worker.end())
        {
            i--;
            count--;
            continue;
        }

        count--;
        ManordoOrder(Order[i], m_wid_vec[count]);
    }

    for (int i = 1; i <= 4; i++)
    {
        if (count <= 0)
            return;

        if (worker.find(m_wid_vec[count]) != worker.end())
        {
            i--;
            count--;
            continue;
        }

        count--;
		Manorfish(1103, i, m_wid_vec[count]);
    }
}

void Client::ManortaskReward(int wid)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Manor&m=taskReward&a={\"wid\":";
    request.url += Helper::Int32ToString(wid);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::function<void(Json::Value&)>());
}

void Client::ManordoTask(int task, int wid)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Manor&m=doTask&a={\"tid\":";
    request.url += Helper::Int32ToString(task);
    request.url += ",\"wid\":";
    request.url += Helper::Int32ToString(wid);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::ManorRespone, this, std::placeholders::_1, 1, task, wid));
}

void Client::ManorRespone(Json::Value& value, int type, int manor, int wid)
{
	if (Helper::StringToInt32(value["code"].asString()) != 0)
	{
		int code = Helper::StringToInt32(value["code"].asString());
		int pos = -1;
		for (int i = 0; i < (int)m_wid_vec.size(); i++)
		{
			if (m_wid_vec[i] == wid)
			{
				pos = i - 1;
				break;
			}
		}

		if (pos < 0)
			return;

		if (code == 6202 || code == 9002 || code == 6223 || code == 6203 || code == 6224 || code == 6222)
			return;

		if (type == 1)
		{
			ManordoTask(manor, m_wid_vec[pos]);
		}

		if (type == 2)
		{
			ManordojoTrain(manor, m_wid_vec[pos]);
		}

		if (type == 3)
		{
			ManordoOrder(manor, m_wid_vec[pos]);
		}

		if (type == 4)
		{
			Manorfish(1103, manor, m_wid_vec[pos]);
		}
	}
}

void Client::ManordojoReward(int pid)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Manor&m=dojoReward&a={\"pid\":";
    request.url += Helper::Int32ToString(pid);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::function<void(Json::Value&)>());
}

void Client::ManordojoTrain(int pid, int wid)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Manor&m=dojoTrain&a={\"isPay\":0,\"type\":2,\"pid\":";
    request.url += Helper::Int32ToString(pid);
    request.url += ",\"wid\":";
    request.url += Helper::Int32ToString(wid);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::ManorRespone, this, std::placeholders::_1, 2, pid, wid));
}

void Client::ManorfishReward(int wid)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Manor&m=fishReward&a={\"wid\":";
    request.url += Helper::Int32ToString(wid);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::function<void(Json::Value&)>());
}

void Client::Manorfish(int bid, int pid, int wid)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Manor&m=fish&a={\"bid\":";
    request.url += Helper::Int32ToString(bid);
    request.url += ",\"type\":2,\"pid\":";
    request.url += Helper::Int32ToString(pid);
    request.url += ",\"wid\":";
    request.url += Helper::Int32ToString(wid);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::ManorRespone, this, std::placeholders::_1, 4, pid, wid));
}

void Client::ManororderReward(int wid)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Manor&m=orderReward&a={\"wid\":";
    request.url += Helper::Int32ToString(wid);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

    post(request, std::function<void(Json::Value&)>());
}

void Client::ManordoOrder(int oid, int wid)
{
    http::http_request request;
    request.type = HTTP_GET;
    request.url = "/s20042/port/interface.php?s=Manor&m=doOrder&a={\"type\":2,\"oid\":";
    request.url += Helper::Int32ToString(oid);
    request.url += ",\"wid\":";
    request.url += Helper::Int32ToString(wid);
    request.url += "}&v=";
    request.url += version;
    request.url += "&sys=";
    request.url += platform_;
    request.host = "pl-game.thedream.cc";
    request.head["Cookie"] = cookie;
    request.pend_flag = "0\r\n\r\n";

	post(request, std::bind(&Client::ManorRespone, this, std::placeholders::_1, 3, oid, wid));
}

void Client::UseItem(int id)
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Item&m=useItem&a={\"id\":";
	request.url += Helper::Int32ToString(id);
	request.url += ",\"newName\":\"\",\"noSort\":1,\"num\":-1}&v=";
	request.url += version;
	request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, nullptr);
}

void Client::Wish()
{
	if (platform_ == "ios")
		return;

	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Wish&m=wish&a={\"isTen\":0,\"wishes\":[151,1]}&v=";
	request.url += version;
	request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, nullptr);
}

void Client::luckyTurnHappyReset(int aid)
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Activity&m=luckyTurnHappyReset&a={\"aid\":";
	request.url += Helper::Int32ToString(aid);
	request.url+="}&v=";
	request.url += version;
	request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, nullptr);
}

void Client::luckyTurnHappyAward(int id, int aid)
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=Activity&m=luckyTurnHappyAward&a={\"id\":";
	request.url += Helper::Int32ToString(id);
	request.url +=",\"aid\":";
	request.url += Helper::Int32ToString(aid);
	request.url += "}&v=";
	request.url += version;
	request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, nullptr);
}

void Client::loulanPray()
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=WeeklyActivity&m=loulanPray&a={}&v=";
	request.url += version;
	request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, nullptr);
}

void Client::prayReward()
{
	http::http_request request;
	request.type = HTTP_GET;
	request.url = "/s20042/port/interface.php?s=WeeklyActivity&m=prayReward&a={}&v=";
	request.url += version;
	request.url += "&sys=";
	request.url += platform_;
	request.host = "pl-game.thedream.cc";
	request.head["Cookie"] = cookie;
	request.pend_flag = "0\r\n\r\n";

	post(request, nullptr);
}