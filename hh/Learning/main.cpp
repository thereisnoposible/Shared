//#include "log4cpp/Category.hh"
//#include "log4cpp/Appender.hh"
//#include "log4cpp/FileAppender.hh"
//#include "log4cpp/OstreamAppender.hh"
//#include "log4cpp/Layout.hh"
//#include "log4cpp/BasicLayout.hh"
//#include "log4cpp/Priority.hh"
#include "AStar.h"
#include <iostream>
#include "include/v8.h"

#ifdef _DEBUG
	//#pragma comment(lib,"log4cppD.lib")
#else
	#pragma comment(lib,"log4cppLIB.lib")
#endif // DEBUG

#ifdef _DEBUG
//#pragma comment(lib,"v8.lib")
#else
#pragma comment(lib,"v8_d.lib")
#endif // DEBUG

using namespace v8;

//#include <log4cpp/Category.hh>
//#include <log4cpp/PropertyConfigurator.hh>
//
//#include "../MyMessageProc/MyMessageProc.h"
#include <windows.h>
#include <TlHelp32.h>

#include <list>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <boost/shared_ptr.hpp>

struct POS
{
	POS() :para(nullptr)
	{

	}
	int x;
	int y;
	int distance;

	long long getID()
	{
		return ((long long)(x) << 32) + y;
	}
	int size;

	int dis_size;
	std::shared_ptr<POS> para;
};

char map[10][15] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,		//0
	1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,		//1
	1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1,		//2
	1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1,		//3
	1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1,		//4
	1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1,		//5
	1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1,		//6
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,		//7
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,		//8
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1			//9
//	0  1  2  3  4  5  6  7  8  9 10 11 12 13 14  
};

std::shared_ptr<POS> pos;
POS tempos;
void showmap()
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			int z = map[i][j];
			if (i == tempos.x && j == tempos.y)
			{
				z += 2;
			}
			std::cout << z << " ";
		}
		std::cout << "\n";
	}
}


std::map<long long, std::shared_ptr<POS>> closepos;
std::list<std::shared_ptr<POS>> openpos;

int calc_distance(int x, int y, int dst_x, int dst_y)
{
	return (x - dst_x)*(x - dst_x) + (y - dst_y)*(y - dst_y);
}

std::vector<std::shared_ptr<POS>> findohterpos(std::shared_ptr<POS>& the,POS& dst)
{
	std::vector<std::shared_ptr<POS>> other;

	if (the->x - 1 > 0)
	{
		if (map[the->x - 1][the->y] != 1)
		{
			std::shared_ptr<POS> temp(new POS);
			temp->x = the->x - 1;
			temp->y = the->y;
			temp->para = the;
			temp->size = temp->para->size + 1;
			temp->distance = calc_distance(temp->x, temp->y, dst.x, dst.y);
			temp->dis_size = temp->size + temp->distance;
			other.push_back(temp);
		}
	}
	if (the->x + 1 < 10)
	{
		if (map[the->x + 1][the->y] != 1)
		{
			std::shared_ptr<POS> temp(new POS);
			temp->x = the->x + 1;
			temp->y = the->y;
			temp->para = the;
			temp->size = temp->para->size + 1;
			temp->distance = calc_distance(temp->x, temp->y, dst.x, dst.y);
			temp->dis_size = temp->size + temp->distance;
			other.push_back(temp);
		}
	}
	if (the->y - 1 > 0)
	{
		if (map[the->x][the->y - 1] != 1)
		{
			std::shared_ptr<POS> temp(new POS);
			temp->x = the->x;
			temp->y = the->y - 1;
			temp->para = the;
			temp->size = temp->para->size + 1;
			temp->distance = calc_distance(temp->x, temp->y, dst.x, dst.y);
			temp->dis_size = temp->size + temp->distance;
			other.push_back(temp);
		}
	}
	if (the->y + 1 < 15)
	{
		if (map[the->x][the->y + 1] != 1)
		{
			std::shared_ptr<POS> temp(new POS);
			temp->x = the->x;
			temp->y = the->y + 1;
			temp->para = the;
			temp->size = temp->para->size + 1;
			temp->distance = calc_distance(temp->x, temp->y, dst.x, dst.y);
			temp->dis_size = temp->size + temp->distance;
			other.push_back(temp);
		}
	}
	return other;
}

std::shared_ptr<POS> getBestPOS()
{
	return openpos.front();
}

void InsertOpenPOS(std::shared_ptr<POS>& temp)
{
	std::list<std::shared_ptr<POS>>::iterator fir = openpos.begin();
	bool bInsert = false;
	for (; fir != openpos.end(); ++fir)
	{
		if (fir->get()->dis_size > temp->dis_size)
		{
			openpos.insert(fir, (temp));
			bInsert = true;
			break;
		}
	}
	if (bInsert == false)
	{
		openpos.push_back(temp);
	}
}

struct PlayerNPCBirthRankItem
{
	PlayerNPCBirthRankItem(int a)
	{		
		unique = a;
		intimacy = unique;
		std::map<int, std::set<const PlayerNPCBirthRankItem*>>::iterator it = myaddr.find(unique);
		if (it == myaddr.end())
		{
			myaddr.insert(std::make_pair(a, std::set<const PlayerNPCBirthRankItem*>()));
			it = myaddr.find(unique);
		}
		it->second.insert(this);
	}
	PlayerNPCBirthRankItem(const PlayerNPCBirthRankItem&& Right)
	{
		intimacy = Right.intimacy;
		unique = Right.unique;
		std::map<int, std::set<const PlayerNPCBirthRankItem*>>::iterator it = myaddr.find(unique);
		if (it != myaddr.end())
		{
			myaddr.insert(std::make_pair(unique, std::set<const PlayerNPCBirthRankItem*>()));
			it = myaddr.find(unique);
		}
		it->second.insert(this);
	}
	~PlayerNPCBirthRankItem()
	{
		std::map<int, std::set<const PlayerNPCBirthRankItem*>>::iterator it = myaddr.find(unique);
		if (it != myaddr.end())
		{
			it->second.erase(this);
		}
	}
	int intimacy;

	int unique;
	static std::map<int,std::set<const PlayerNPCBirthRankItem*>> myaddr;
};

std::map<int, std::set<const PlayerNPCBirthRankItem*>> PlayerNPCBirthRankItem::myaddr = std::map<int, std::set<const PlayerNPCBirthRankItem*>>();

bool my_aaa(PlayerNPCBirthRankItem& left, PlayerNPCBirthRankItem& right)
{
	return left.intimacy > right.intimacy;
}

char buf[4096] = {};
FILE* pFile;
#include "Helper.h"
#include "Helper.cpp"
int LoadConfig(char* src,char* dst)
{
	memset(buf, 0, sizeof(buf));
	fopen_s(&pFile, src, "r");
	if (pFile == NULL)
		return -1;

	fseek(pFile, 0, SEEK_SET);

	fread(buf, 4095, 1, pFile);
	std::vector<std::string> sAstr;
	std::vector<std::string> param;
	param.push_back("{");
	param.push_back("}");

	Helper::SplitString(buf, param, sAstr);
	
	//param.push_back("\r");
	param.push_back("\n");
	param.push_back(";");

	std::vector<std::string> sAparam;

	Helper::SplitString(sAstr[1], param, sAparam);

	int pos = 0;
	try
	{
		for (int i = 0; i < sAparam.size(); i++)
		{
            if (sAparam[i].find("//-") != std::string::npos)
                continue;

			sAparam[i].replace(1, 0, "\"");

			int res = sAparam[i].find("=", 0);
			int dot = sAparam[i].find(",", 0);
			if (dot == std::string::npos || dot == 0)
				throw("error");		

			if (res != std::string::npos)
			{
				pos = Helper::StringToInt(sAparam[i].substr(res + 1, dot - res - 1));
				sAparam[i].replace(res, 1, "\":");
				pos++;
				continue;
			}
			
			sAparam[i].replace(dot, 0, "\":" + Helper::IntToString(pos++));
		}
	}
	catch (...)
	{

	}
	sAparam.push_back("};");
	FILE* fWrite;
	fopen_s(&fWrite, dst, "wb+");
	std::string str;
	str += "module.exports = {\r\n";
	fwrite(str.c_str(), str.length(), 1, fWrite);
	std::string temp;
	temp += "\r\n";
	for (int i = 0; i < sAparam.size(); i++)
	{
		fwrite(sAparam[i].c_str(), sAparam[i].size(), 1, fWrite);
		
		fwrite(temp.c_str(), temp.size(), 1, fWrite);
	}

	memset(buf, 0, sizeof(buf));
	fseek(fWrite, 0, SEEK_SET);

	fread(buf, 4095, 1, fWrite);

	fclose(pFile);
	fclose(fWrite);
	return 0;
}

int main(int argc, char* argv[])
{

	//v8::V8::Initialize();
	////v8::Isolate* isolate = v8::Isolate::New();v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
	//printf("version: %s\n", v8::V8::GetVersion());

	//v8::Isolate* isolate = v8::Isolate::GetCurrent();
	//{
	//	// 创建一个句柄作用域 ( 在栈上 )
	//	HandleScope handle_scope(isolate);
	//	// 创建一个新的上下文对象
	//	v8::Handle<v8::Context> context = Context::New(isolate);
	//	if (context.IsEmpty())
	//	{
	//		fprintf(stderr, "Error creating context\n");
	//		return 1;
	//	}
	//	context->Enter();
	//	// 创建一个字符串对象，字符串对象被 JS 引擎
	//	// 求值后，结果为'Hello, World!2013'
	//	//Handle<String> source = String::New("'hello,World!'+(2012+1)");
	//	// 编译字符串对象为脚本对象
	//	//Handle<Script> script = Script::Compile(source);
	//	// 执行脚本，获取结果
	//	//Handle <Value> result = script->Run();
	//	// 转换结果为字符串
	//	//String::AsciiValue ascii(result);
	//	//printf("%s\n", *ascii);
	//	context->Exit();
	//}
	//v8::V8::Dispose();
	//return 0;

	//LARGE_INTEGER nFreq;

	//LARGE_INTEGER nBeginTime;

	//LARGE_INTEGER nEndTime;

	//QueryPerformanceFrequency(&nFreq);

	//POSLocal dst;
	//std::cin >> dst.x;
	//std::cin >> dst.y;
	//POSLocal poslocel;
	//poslocel.x = 6;
	//poslocel.y = 3;
	//AStar astar;
	//astar.LoadMap((char*)map, 10, 15);
	//QueryPerformanceCounter(&nBeginTime);
	//std::vector<POSLocal>& distan = astar.GetDistance(poslocel, dst);
	//QueryPerformanceCounter(&nEndTime);

	//double temptime;
	//////temptime = usetime.count();
	//temptime = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	////

	////std::cout << temptime << "\n";
	//printf("%8.21lf", temptime);
	//Sleep(2000);
	//for (int i = distan.size() - 1; i >= 0; i--)
	//{
	//		system("cls");
	//		tempos.x = distan[i].x;
	//		tempos.y = distan[i].y;

	//		showmap();
	//		Sleep(500);
	//}

	//pos = std::shared_ptr<POS>(new POS);
	//pos->x = 6;
	//pos->y = 3;
	//pos->distance = calc_distance(pos->x, pos->y, dst.x, dst.y);
	//pos->size = 0;
	//pos->dis_size = pos->distance + pos->size;
	//openpos.push_back(pos);
	//closepos.insert(std::make_pair(pos->getID(), pos));
	//std::unordered_map<std::shared_ptr<PlayerNPCBirthRankItem>, int> pmap;

	//std::vector<PlayerNPCBirthRankItem> temp;
	//for (int i = 0; i < 10; i++)
	//{
	//	temp.push_back(std::move(PlayerNPCBirthRankItem(i)));
	//}

	//PlayerNPCBirthRankItem p = temp[8];

	//temp[8].intimacy = 1000;

	//std::sort(temp.begin(), temp.end(), my_aaa);
	//int pos = -1;
	//std::map<int, std::set<const PlayerNPCBirthRankItem*>>::iterator it = p.myaddr.find(p.unique);
	//if (it != p.myaddr.end())
	//{
	//	std::set<const PlayerNPCBirthRankItem*>::iterator iit = it->second.begin();
	//	for (; iit != it->second.end(); ++iit)
	//	{
	//		long long a = *iit - &temp[0];
	//		if (a >= 0 && a <= sizeof(PlayerNPCBirthRankItem)*temp.size() - 1)
	//		{
	//			pos = a / sizeof(PlayerNPCBirthRankItem);
	//		}
	//	}
	//}

	LoadConfig("NetMessage.h","NetMessage.js");
	

	//std::shared_ptr<POS>& best = getBestPOS();
	////std::chrono::steady_clock::time_point fCurr = std::chrono::steady_clock::now();
	//QueryPerformanceCounter(&nBeginTime);

	//while (!(best->x == dst.x && best->y == dst.y))
	//{
	//	std::vector<std::shared_ptr<POS>>& other = findohterpos(best, dst);

	//	openpos.pop_front();
	//	closepos.insert(std::make_pair(best->getID(), best));
	//	for (int i = 0; i < (int)other.size(); i++)
	//	{
	//		if (closepos.find(other[i]->getID()) != closepos.end())
	//			continue;

	//		//other[i].line = best.line;
	//		//other[i].line.push_front(best);

	//		//other[i]->size = other[i]->para->size + 1;
	//		//other[i]->distance = calc_distance(other[i]->x, other[i]->y, dst.x, dst.y);
	//		//other[i]->dis_size = other[i]->size + other[i]->distance;

	//		for (std::list<std::shared_ptr<POS>>::iterator it = openpos.begin(); it != openpos.end(); ++it)
	//		{
	//			if (it->get()->x == other[i]->x && it->get()->y == other[i]->y)
	//			{
	//				if (it->get()->dis_size < other[i]->dis_size)
	//				{
	//					other[i]->para = it->get()->para;
	//				}
	//				break;
	//			}			
	//		}
	//		InsertOpenPOS(other[i]);
	//	}
	//	
	//	best = getBestPOS();
	//}
	////std::chrono::steady_clock::time_point fLast = std::chrono::steady_clock::now();
	////std::chrono::duration<double> usetime = std::chrono::duration_cast<std::chrono::duration<double>>(fLast - fCurr);
	//QueryPerformanceCounter(&nEndTime);

	//double temptime;
	////temptime = usetime.count();
	//temptime = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	//

	//std::cout << temptime << "\n";
	//printf("%8.21lf", temptime);

	//Sleep(2000);

	//while (best.get() != nullptr)
	//{
	//	system("cls");
	//	tempos.x = best->x;
	//	tempos.y = best->y;

	//	showmap();
	//	best = best->para;
	//	Sleep(500);
	//}

	//std::string initFileName = "log4cpp.properties";
	//log4cpp::PropertyConfigurator::configure(initFileName);

	//log4cpp::Category& root = log4cpp::Category::getRoot();

	//log4cpp::Category& sub1 =
	//	log4cpp::Category::getInstance(std::string("sub1"));

	//log4cpp::Category& sub2 =
	//	log4cpp::Category::getInstance(std::string("sub1.sub2"));

	//root.warn("Storm is coming");

	//sub1.debug("Received storm warning");
	//sub1.info("Closing all hatches");

	//sub2.debug("Hiding solar panels");
	//sub2.error("Solar panels are blocked");
	//sub2.debug("Applying protective shield");
	//sub2.warn("Unfolding protective shield");
	//sub2.info("Solar panels are shielded");

	//sub1.info("All hatches closed");

	//root.info("Ready for storm.");

	//log4cpp::Category::shutdown();

	//HMODULE hMod = LoadLibrary(L"MyMessageProc.dll");
	//if (!hMod) return FALSE;
	////得到显示函数的地址  
	//DWORD lpFunc = (DWORD)GetProcAddress(hMod, "MyMessageProc");
	//if (!lpFunc)
	//{
	//	if (hMod) FreeLibrary(hMod);
	//	return FALSE;
	//}
	////得到待注入EXE的进程ID  
	//HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//if (INVALID_HANDLE_VALUE == hSnapshot)
	//{
	//	if (hMod) FreeLibrary(hMod);
	//	return NULL;
	//}
	//PROCESSENTRY32 pe32;
	//pe32.dwSize = sizeof(pe32);
	//BOOL bMORE = ::Process32First(hSnapshot, &pe32);
	//DWORD dwProcessId = 0;
	//while (bMORE)
	//{
	//	wprintf(L"进程名：%s\n", pe32.szExeFile);
	//	wprintf(L"进程ID：%d\n", pe32.th32ProcessID);
	//	if (wcscmp(pe32.szExeFile, L"Win32Project2.exe") == 0)
	//	{
	//		dwProcessId = pe32.th32ProcessID;
	//		break;
	//	}
	//	bMORE = ::Process32Next(hSnapshot, &pe32);
	//}
	////DWORD dwProcessId = GetProcessId("Win32Project2");

	//if (!dwProcessId)
	//{
	//	if (hMod) FreeLibrary(hMod);
	//	return FALSE;
	//}

	//THREADENTRY32 te32;
	//te32.dwSize = sizeof(te32);
	//DWORD dwThreadID = 0;
	//HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	//if (Thread32First(hThreadSnap, &te32))
	//{
	//	do{
	//		if (dwProcessId == te32.th32OwnerProcessID)
	//		{
	//			dwThreadID = te32.th32ThreadID;
	//			break;
	//		}
	//	} while (Thread32Next(hThreadSnap, &te32));
	//}

	//if (!dwThreadID)
	//{
	//	if (hMod) FreeLibrary(hMod);
	//	return FALSE;
	//}

	////得到待注入EXE的线程ID  
	////DWORD dwThreadId = GetThreadId(dwThreadID);
	//DWORD dwThreadId = dwThreadID;
	//if (!dwThreadId)
	//{
	//	DWORD error = GetLastError();
	//	if (hMod) FreeLibrary(hMod);
	//	return FALSE;
	//}
	////利用HOOK进行注入  
	//HHOOK hhook = SetWindowsHookEx(
	//	//WH_GETMESSAGE,
	//	WH_KEYBOARD,
	//	//WH_CALLWNDPROC,  
	//	(HOOKPROC)lpFunc,
	//	hMod,
	//	dwThreadId);

	//while (1)
	//{

	//}

	return 0;
}