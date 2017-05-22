#pragma once
#include <random>
#include <chrono>
#include <vector>

#include "../include/define/define.h"

namespace xlib
{
	namespace Helper
	{
		int XDLL ABS(int x);
		int XDLL GetRandom(int min, int max);
		double XDLL GetRandom(double min, double max);
		size_t XDLL FindString(const char* str, size_t size, const std::string&spl);
		size_t XDLL FindString(const char* str, size_t size, const std::vector<std::string>&, size_t&);
		void XDLL SplitString(const char* str, size_t size, const std::string&spl, std::vector<std::string>&sAstr, bool bEmpty = false);
		void XDLL SplitString(const std::string&, const std::string&, std::vector<std::string>&);

		void XDLL SplitString(const char* str, size_t size, const std::vector<std::string>&, std::vector<std::string>&sAstr, bool bEmpty = false);
		void XDLL SplitString(const std::string&, const std::vector<std::string>&, std::vector<std::string>&);

		void XDLL SplitStringHasEmpty(const std::string&str, const std::string&spl, std::vector<std::string>&sAstr);

		int XDLL StringToInt32(const std::string&);

		long long XDLL StringToInt64(const std::string&);

		std::string XDLL Int32ToString(int);

		std::string XDLL Int64ToString(long long);

		long long XDLL Getdbid(int playerid, int unique);

		std::chrono::steady_clock::time_point XDLL getMSTime();

		double XDLL calcInterval(std::chrono::steady_clock::time_point& fPrev, std::chrono::steady_clock::time_point& fNow);

		/**
		* @brief		GB2312转UTF8编码
		*/
		std::string XDLL Gb2312ToUTF8(const std::string& gb2312);

		/**
		* @brief		UTF8转换成GB2312
		*/
		std::string XDLL UTF8ToGb2312(const  std::string& uft8);

		/**
		* @brief		UTF8转换成unicode
		*/
		std::wstring  XDLL UTF8ToUnicode(const std::string& utf8);

		/**
		* @brief		unicode转换成gb2312
		*/
		std::string XDLL UnicodeToGb2312(const std::wstring unico);

		std::wstring XDLL Gb2312ToUnicode(const std::string gb2312);

		/**
		* @brief		判断字符串是否是utf8
		*/
		bool XDLL IsTextUTF8(const char* str, int length);

		std::string XDLL Timet2String(time_t t);
		time_t XDLL String2Timet(const std::string& str);

		std::string XDLL chartohex(unsigned char p);

		bool XDLL hextoint32(const std::string& _In_str, int& _out_num);
		bool XDLL hextoint64(const std::string& _In_str, long long& _out_num);

		template<size_t T>
		inline void XDLL convert(char *val)
		{
			std::swap(*val, *(val + T - 1));
			convert<T - 2>(val + 1);
		}

		template<> inline void XDLL convert<0>(char *) {}
		template<> inline void XDLL convert<1>(char *) {}            // ignore central byte

		template<typename T> inline void XDLL apply(T *val)
		{
			convert<sizeof(T)>((char *)(val));
		}
	}
}
