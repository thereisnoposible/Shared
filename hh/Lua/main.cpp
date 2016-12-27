#include <iostream>
extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include "lua/tolua++.h"
}
static const char * load_config = "\
local config_name = ... \
local f = assert(io.open(config_name)) \
local code = assert(f:read \'*a\') \
local function getenv(name) return assert(os.getenv(name), \'os.getenv() failed: \' .. name) end \
code = string.gsub(code, \'%$([%w_%d]+)\', getenv) \
print(code)\
f:close() \
local result = {} \
assert(load(code,\'=(load)\',\'t\',result))() \
print(result)\
return result \
";
#include <vector>
#include <time.h>
#include "boost/regex.hpp"
#include "../Public/Helper.h"
#include "../Public/Helper.cpp"

boost::regex expression2("^[1-9]\\d{5}[1-9]\\d{3}((0[1-9])|(1[0-2]))(([0][1-9]|[12]\\d)|3[0-1])\\d{3}([0-9]|X)$");
int Wi[] = { 7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2 };
int Y[] = { 1, 0, 10, 9, 8, 7, 6, 5, 4, 3, 2 };
int poss[] = { 2, 2, 2, 4, 2, 2, 2, 1, 1 };
class IDCARD
{
public:
	IDCARD(std::string _name, std::string _idcard)
	{
		name = _name;
		id = _idcard;
		int pos = 0;
		std::string temp;
		int* point = &this->province;
		for (int i = 0; i < sizeof(poss) / sizeof(int); i++)
		{
			temp.assign(_idcard.begin() + pos, _idcard.begin() + pos + poss[i]);
			pos += poss[i];
			if (temp == "X" || temp == "x")
				*(point + i) = 10;
			else
				*(point + i) = Helper::StringToInt3232(temp);
		}

		//pos = 0;
		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//province = Helper::StringToInt32(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//city = Helper::StringToInt32(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//county = Helper::StringToInt32(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 4);
		//pos += 4;
		//year = Helper::StringToInt32(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//month = Helper::StringToInt32(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//day = Helper::StringToInt32(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//code = Helper::StringToInt32(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 1);
		//pos += 1;
		//sex = Helper::StringToInt32(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 1);
		//pos += 1;
		//if (temp == "X" || temp == "x")
		//	checkcode = 10;
		//else
		//	checkcode = Helper::StringToInt32(temp);
	}

	bool check()
	{
		return (checkAreaCode() && checkCode());
	}

private:
	bool checkCode()
	{
		int total = 0;
		const char*p = id.c_str();
		for (int i = 0; i < 17; i++)
		{
			std::string str;
			str = *(p + i);
			total += Helper::StringToInt3232(str) * Wi[i];
		}
		total = total % 11;
		return Y[total] == checkcode;
	}

	bool checkAreaCode()
	{
		return KeywordFilter::GetSingleton().IsRealAreaCode(province, city, county);
	}

private:
	std::string name;
	std::string id;
	int province;
	int city;
	int county;
	int year;
	int month;
	int day;
	int code;
	int sex;
	int checkcode;
};
boost::regex expressionname("[^\\s]");

int main()
//int aa()
{
	lua_State* lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
	if(luaL_dofile(lua_state, "hellolua.lua"))
		std::cout << luaL_checkstring(lua_state, -1);



	while (1)
	{
		std::string str;
		std::getline(std::cin, str);
		std::vector<std::string> sAstr;
		Helper::SplitString(str, " ", sAstr);
		int year = Helper::StringToInt32(sAstr[0]);
		int month = Helper::StringToInt32(sAstr[1]);
		int day = Helper::StringToInt32(sAstr[2]);

			time_t tNow = time(NULL);
			tm tmNow = *localtime(&tNow);

			if (year + 18 > tmNow.tm_year + 1900)
			{
				std::cout << "false\n";
				continue;
			}
			else if (year + 18 == tmNow.tm_year + 1900 && month > tmNow.tm_mon + 1)
			{
				std::cout << "false\n";
				continue;
			}
			else if (month == tmNow.tm_mon + 1 && day > tmNow.tm_mday)
			{
				std::cout << "false\n";
				continue;
			}			
			std::cout << "true\n";
		
		
	}

	int err = luaL_loadstring(lua_state, load_config);
	lua_pushstring(lua_state, "hellolua.lua");		
	lua_pcall(lua_state, 1, 1, 0);

	lua_close(lua_state);
	return 0;
}