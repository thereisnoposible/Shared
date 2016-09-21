#include "Helper.h"
#include <time.h>
#ifdef WIN32
#include <windows.h>
#endif
namespace Helper
{
	int ABS(int x){ return (x > 0 ? x : -x); }
	int GetRandom(int min, int max)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(gen);
	}

	double GetRandom(double min, double max)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> distribution(min, max);
		return distribution(gen);
	}

	int StringToInt(const std::string& str)
	{
		int num = 0;
		const char* p = str.c_str();
		for (int temp = 0; temp < (int)str.size(); temp++)
		{
			num *= 10;
			num += *(p + temp) - '0';
		}
		return num;
	}

	long long StringToLongLong(const std::string& str)
	{
		long long num = 0;
		const char* p = str.c_str();
		for (int temp = 0; temp < (int)str.size(); temp++)
		{
			if (*(p + temp) > '9' || *(p + temp) < '0')
				continue;
			num *= 10;
			num += *(p + temp) - '0';
		}
		return num;
	}

	long long Getdbid(int playerid,int unique)
	{
		time_t tNow = time(NULL);
		long long dbid = tNow;
		dbid = dbid << 32;
		dbid += playerid << 16;
		dbid += unique;
		return dbid;
	}

	std::string IntToString(int num)
	{
		std::string str;
		char a[30] = { 0 };
		sprintf_s(a, "%d", num);
		str = a;
		return str;
	}

	std::string LongLongToString(long long num)
	{
		std::string str;
		char a[30] = { 0 };
		sprintf_s(a, "%lld", num);
		str = a;
		return str;
	}
	void SplitString(const std::string&str, const std::string&spl, std::vector<std::string>&sAstr)
	{
		if (str.empty())
			return;
		int pos = 0;
		pos = str.find(spl, pos);
		if (pos != std::string::npos)
		{
			if (pos != 0)
				sAstr.push_back(std::string().assign(str.begin(), str.begin() + pos));
			SplitString(str.c_str() + pos + spl.length(), spl, sAstr);
		}
		else
			sAstr.push_back(str);
	}
	void SplitStringHasEmpty(const std::string&str, const std::string&spl, std::vector<std::string>&sAstr)
	{
		if (str.empty())
		{
			sAstr.push_back(str);
			return;
		}
		int pos = 0;
		pos = str.find(spl, pos);
		if (pos != std::string::npos)
		{
			if (pos != 0)
				sAstr.push_back(std::string().assign(str.begin(), str.begin() + pos));
			SplitStringHasEmpty(str.c_str() + pos + spl.length(), spl, sAstr);
		}
		else
			sAstr.push_back(str);
	}
	void SplitStringOld(const std::string& str, const std::string& spl, std::vector<std::string>& sAstr)
	{
		int pos = 0;
		int nextpos = 0;
		pos = str.find(spl, pos);
		nextpos = str.find(spl, pos + 1);
		if (pos == std::string::npos)
		{
			sAstr.push_back(str);
			return;
		}
		else
		{
			sAstr.push_back(std::string().assign(str, 0, pos));
		}
		while (pos != std::string::npos)
		{
			if (pos == 0)
			{
				if (nextpos == std::string::npos)
				{
					sAstr.push_back(std::string());
					sAstr.push_back(std::string().assign(str, pos + 1, str.size() - pos));
					return;
				}
				else
				{
					sAstr.push_back(std::string().assign(str, pos + 1, nextpos - pos - 1));
					pos = str.find(spl, nextpos);
					nextpos = str.find(spl, pos + 1);
				}
			}
			else
			{
				if (nextpos == std::string::npos)
				{
					sAstr.push_back(std::string().assign(str, pos + 1, str.size() - pos));
					return;
				}
				else
				{
					sAstr.push_back(std::string().assign(str, pos + 1, nextpos - pos - 1));
					pos = str.find(spl, nextpos);
					nextpos = str.find(spl, pos + 1);
				}
			}
		}
	}

	void SplitString(const std::string&str, const std::vector<std::string>&spl, std::vector<std::string>&sAstr)
	{
		if (str.empty())
			return;
		int minpos = std::string::npos;
		int pos = 0;
		int vecpos = 0;
		for (int i = 0; i < (int)spl.size(); i++)
		{
			pos = str.find(spl[i], 0);
			if (pos != std::string::npos)
			{
				if (minpos == std::string::npos)
				{
					minpos = pos;
					vecpos = i;
				}
				else
				{
					if (minpos > pos)
					{
						minpos = pos;
						vecpos = i;
					}
				}
			}
		}
		if (minpos != std::string::npos)
		{
			if (minpos != 0)
				sAstr.push_back(std::string().assign(str.begin(), str.begin() + minpos));
			SplitString(str.c_str() + minpos + spl[vecpos].length(), spl, sAstr);
		}
		else
			sAstr.push_back(str);
	}

	std::chrono::steady_clock::time_point getMSTime()
	{
		return std::chrono::steady_clock::now();
	}

	double calcInterval(std::chrono::steady_clock::time_point& fPrev, std::chrono::steady_clock::time_point& fNow)
	{
		std::chrono::duration<double> usetime = std::chrono::duration_cast<std::chrono::duration<double>>(fNow - fPrev);
		return usetime.count();
	}

	//-----------------------------------------------------------------------
	std::string Gb2312ToUTF8(const  std::string& gb2312)
	{
		if (gb2312.empty())
			return "";

		std::string tem = "";

#ifdef WIN32
		int len = MultiByteToWideChar(CP_ACP, 0, gb2312.c_str(), -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, gb2312.c_str(), -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);

		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		if (wstr)
			delete[] wstr;

		tem = str;

		delete[]str;
#else
		iconv_t cd;
		if ((cd = iconv_open("gb2312", "utf-8")) == 0)
		{
			return tem;
		}

		char* src = const_cast<char*>(gb2312.c_str());
		size_t src_len = gb2312.length();

		char dst[1024] = { 0 };
		size_t dst_len = sizeof(dst);

		char* in = src;
		char* out = dst;

		if (iconv(cd, &in, &src_len, &out, &dst_len) != 0)
		{
			tem = dst;
		}

#endif

		return tem;
	}


	std::string UTF8ToGb2312(const  std::string& utf8)
	{
		if (utf8.empty())
			return "";

		std::string tem = "";

#ifdef WIN32

		int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wstr, len);
		len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
		if (wstr)
			delete[] wstr;
		tem = str;
		delete[] str;

		return tem;


#else
		iconv_t cd;
		if ((cd = iconv_open("utf-8", "gb2312")) == 0)
		{
			return tem;
		}

		char* src = const_cast<char*>(utf8.c_str());
		size_t src_len = utf8.length();

		char dst[1024] = { 0 };
		size_t dst_len = sizeof(dst);

		char* in = src;
		char* out = dst;

		if (iconv(cd, &in, &src_len, &out, &dst_len) != 0)
		{
			tem = dst;
		}
#endif

		return "";
	}

	//-------------------------------------------------------------------------------------------
	std::wstring  UTF8ToUnicode(const std::string& utf8)
	{
		if (utf8.empty())
		{
			return L"";
		}

		std::wstring strUnicode = L"";

#ifdef _WIN32
		int dwUnicodeLen;
		wchar_t *pwText;
		dwUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);

		pwText = new wchar_t[dwUnicodeLen];

		if (!pwText)
		{
			return strUnicode;
		}

		MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, pwText, dwUnicodeLen);

		strUnicode = pwText;

		delete[] pwText;

#else
		iconv_t cd;
		if ((cd = iconv_open("utf-8", "wchar_t")) == 0)
		{
			return strUnicode;
		}

		char* src = const_cast<char*>(utf8.c_str());
		size_t src_len = utf8.length();

		wchar_t dst[1024] = { 0 };
		size_t dst_len = sizeof(dst);

		char* in = src;
		char* out = (char*)dst;

		if (iconv(cd, &in, &src_len, &out, &dst_len) != 0)
		{
			strUnicode = dst;
		}
#endif

		return strUnicode;
	}

	std::string UnicodeToGb2312(const std::wstring unico)
	{
		std::string strTmp = "";

#ifdef _WIN32

#else
		iconv_t cd;
		if ((cd = iconv_open("wchat_t", "gb2312")) == 0)
		{
			return strTmp;
		}

		const wchar_t* src = unico.c_str();
		size_t src_len = unico.length();

		char dst[1024] = { 0 };
		size_t dst_len = sizeof(dst);

		char* in = const_cast<char*>((const char *)src);
		char* out = dst;

		if (iconv(cd, &in, &src_len, &out, &dst_len) != 0)
		{
			strTmp = dst;
		}
#endif
		return strTmp;
	}

	//-------------------------------------------------------------------------------------------
	std::wstring Gb2312ToUnicode(const std::string gb2312)
	{
		std::wstring strTmp = L"";

#ifdef _WIN32

#else
		iconv_t cd;
		if ((cd = iconv_open("gb2312", "wchat_t")) == 0)
		{
			return strTmp;
		}

		const char* src = gb2312.c_str();
		size_t src_len = gb2312.length();

		char dst[1024] = { 0 };
		size_t dst_len = sizeof(dst);

		char* in = const_cast<char*>(src);
		char* out = dst;

		if (iconv(cd, &in, &src_len, &out, &dst_len) != 0)
		{
			strTmp = (wchar_t*)dst;
		}
#endif
		return strTmp;
	}

	//-------------------------------------------------------------------------------------------
	bool IsTextUTF8(const char* str, int length)
	{
		int i;
		int nBytes = 0;					//UFT8可用1-6个字节编码,ASCII用一个字节
		unsigned char chr;
		bool bAllAscii = true;				//如果全部都是ASCII, 说明不是UTF-8
		for (i = 0; i < length; i++)
		{
			chr = *(str + i);
			if ((chr & 0x80) != 0)			// 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
				bAllAscii = false;
			if (nBytes == 0)				//如果不是ASCII码,应该是多字节符,计算字节数
			{
				if (chr >= 0x80)
				{
					if (chr >= 0xFC && chr <= 0xFD)
						nBytes = 6;
					else if (chr >= 0xF8)
						nBytes = 5;
					else if (chr >= 0xF0)
						nBytes = 4;
					else if (chr >= 0xE0)
						nBytes = 3;
					else if (chr >= 0xC0)
						nBytes = 2;
					else
					{
						return false;
					}
					nBytes--;
				}
			}
			else //多字节符的非首字节,应为 10xxxxxx
			{
				if ((chr & 0xC0) != 0x80)
				{
					return false;
				}
				nBytes--;
			}
		}
		if (nBytes > 0) //违返规则
		{
			return false;
		}
		if (bAllAscii) //如果全部都是ASCII, 说明不是UTF-8
		{
			return false;
		}

		return true;
	}
}