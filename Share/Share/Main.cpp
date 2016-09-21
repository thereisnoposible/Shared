#include "Config.h"
#include "Singleton.h"
#ifdef _DEBUG
#pragma comment(lib,"lib/libprotobuf_d.lib")
#else
#pragma comment(lib,"lib/libprotobuf.lib")
#endif // DEBUG
int main()
{
	Config config;
	config.LoadConfig("server.ini");
	config.GetStringValue("BaseApp", "port");

	std::vector<std::string> sql;
	sql.push_back("a");
	sql.push_back("be");
	std::vector<std::string> sAstr;
	Helper::SplitString("acbead", sql, sAstr);
	sAstr.clear();
	Helper::SplitString("bbcbeb", "bb", sAstr);
}