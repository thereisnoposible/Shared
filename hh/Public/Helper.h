#pragma once
#include <random>
#include <chrono>
#include <vector>
namespace Helper
{
	int ABS(int x);
	int GetRandom(int min, int max);
	double GetRandom(double min, double max);
    int FindString(const char* str, int size, const std::string&spl);
    int FindString(const char* str, int size, const std::vector<std::string>&);
    void SplitString(const char* str, int size, const std::string&spl, std::vector<std::string>&sAstr, bool bEmpty = false);
	void SplitString(const std::string&, const std::string&, std::vector<std::string>&);

    void SplitString(const char* str, int size, const std::vector<std::string>&, std::vector<std::string>&sAstr, bool bEmpty = false);
	void SplitString(const std::string&, const std::vector<std::string>&, std::vector<std::string>&);

	void SplitStringHasEmpty(const std::string&str, const std::string&spl, std::vector<std::string>&sAstr);

	int StringToInt32(const std::string&);
	
	long long StringToInt64(const std::string&);

	std::string Int32ToString(int);
	
	std::string Int64ToString(long long);

	long long Getdbid(int playerid, int unique);

	std::chrono::steady_clock::time_point getMSTime();

	double calcInterval(std::chrono::steady_clock::time_point& fPrev, std::chrono::steady_clock::time_point& fNow);

	/**
	* @brief		GB2312转UTF8编码
	*/
	std::string Gb2312ToUTF8(const std::string& gb2312);

	/**
	* @brief		UTF8转换成GB2312
	*/
	std::string UTF8ToGb2312(const  std::string& uft8);

	/**
	* @brief		UTF8转换成unicode
	*/
	std::wstring  UTF8ToUnicode(const std::string& utf8);

	/**
	* @brief		unicode转换成gb2312
	*/
	std::string UnicodeToGb2312(const std::wstring unico);

	std::wstring Gb2312ToUnicode(const std::string gb2312);

	/**
	* @brief		判断字符串是否是utf8
	*/
	bool IsTextUTF8(const char* str, int length);

	std::string Timet2String(time_t t);
	time_t String2Timet(const std::string& str);

	std::string chartohex(unsigned char p);

	bool hextoint32(const std::string& _In_str, int&& _out_num);
	bool hextoint64(const std::string& _In_str, long long&& _out_num);

	template<size_t T>
	inline void convert(char *val)
	{
		std::swap(*val, *(val + T - 1));
		convert<T - 2>(val + 1);
	}

	template<> inline void convert<0>(char *) {}
	template<> inline void convert<1>(char *) {}            // ignore central byte

	template<typename T> inline void apply(T *val)
	{
		convert<sizeof(T)>((char *)(val));
	}
}