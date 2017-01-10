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

	int StringToInt32(const std::string& str)
	{
        int i = atoi(str.c_str());

        return i;
	}

	long long StringToInt64(const std::string& str)
	{
        long long i = atoll(str.c_str());

        return i;
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

	std::string Int32ToString(int num)
	{
		std::string str;
		char a[30] = { 0 };
		sprintf_s(a, "%d", num);
		str = a;
		return str;
	}

	std::string Int64ToString(long long num)
	{
		std::string str;
		char a[30] = { 0 };
		sprintf_s(a, "%lld", num);
		str = a;
		return str;
	}

    int FindString(const char* str, int size, const std::string&spl)
    {
        int pos = -1;
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < (int)spl.size(); j++)
            {
                if (*(str + i + j) != spl[j])
                    break;

                if (j == spl.size() - 1)
                    pos = i;
            }

            if (pos != -1)
                break;
        }

        return pos;
    }

    int FindString(const char* str, int size, const std::vector<std::string>&spl)
    {
        int pos = -1;
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < (int)spl.size(); j++)
            {
                for (int z = 0; z < (int)spl[j].size(); z++)
                {
                    if (*(str + i + z) != spl[j][z])
                        break;

                    if (z == spl.size() - 1)
                        pos = i;
                }

                if (pos != -1)
                    break;
            }

            if (pos != -1)
                break;
        }

        return pos;
    }

    void SplitString(const std::string&str, const std::string&spl, std::vector<std::string>&sAstr)
    {
        if (str.empty())
            return;

        SplitString(str.c_str(), str.size(), spl, sAstr);
    }

    void SplitString(const char* str, int size, const std::string&spl, std::vector<std::string>&sAstr, bool bEmpty)
    {
        if (size == 0)
            return;

        int pos = FindString(str, size, spl);
        int off = 0;
        while (1)
        {
            if (pos == -1)
                break;

            if (bEmpty == true || pos - off != 0)
                sAstr.push_back(std::string(str + off, pos - off));

            off = pos + spl.size();
            pos = FindString(str + off, size - off, spl);

            if (pos == -1)
                break;

            pos = pos + off;
        }

        if (pos == -1 && size - off != 0)
            sAstr.push_back(std::string(str + off, size - off));

    }

	void SplitStringHasEmpty(const std::string&str, const std::string&spl, std::vector<std::string>&sAstr)
	{
		if (str.empty())
		{
			sAstr.push_back(str);
			return;
		}
		
        SplitString(str.c_str(), str.size(), spl, sAstr, true);
	}

	void SplitString(const std::string&str, const std::vector<std::string>&spl, std::vector<std::string>&sAstr)
	{
		if (str.empty())
			return;
		
        SplitString(str.c_str(), str.size(), spl, sAstr);
	}

    void SplitString(const char* str, int size, const std::vector<std::string>&spl, std::vector<std::string>&sAstr, bool bEmpty)
    {
        if (size == 0)
            return;

        int pos = FindString(str, size, spl);
        int off = 0;
        while (1)
        {
            if (bEmpty == true || pos - off != 0)
                sAstr.push_back(std::string(str + off, pos - off));

            off = pos + spl.size();
            pos = FindString(str + off, size - off, spl);

            if (pos == -1)
                break;

            pos = pos + off;
        }

        if (pos == -1 && size - off != 0)
            sAstr.push_back(std::string(str + off, size - off));

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

	//-------------------------------------------------------------------------------
	std::string Helper::Timet2String(time_t t)
	{
		char szTime[20] = { 0 };
#ifdef _WIN32
		struct tm timeinfo;
		localtime_s(&timeinfo, &t);
		struct tm* ptimeinfo = &timeinfo;
#else
		struct tm* ptimeinfo = localtime(&t);
#endif
		strftime(szTime, 20, "%Y-%m-%d %H:%M:%S", ptimeinfo);

		return szTime;
	}


	//-------------------------------------------------------------------------------
	time_t Helper::String2Timet(const std::string& str)
	{
		struct tm t;
		memset((char*)&t, 0, sizeof(tm));

#ifdef _WIN32
		sscanf_s(str.c_str(), "%04d-%02d-%02d %02d:%02d:%02d",
			&t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
#else
		sscanf(str.c_str(), "%04d-%02d-%02d %02d:%02d:%02d",
			&t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
#endif
		t.tm_year -= 1900;
		t.tm_mon -= 1;

		time_t tmRet = mktime(&t);

		return tmRet;
	}

	std::string chartohex(unsigned char p)
	{
		char a[3] = { 0 };
		sprintf_s(a, "%X", p);
		return a;
	}

	bool hextoint64(const std::string& _In_str, long long&& _out_num)
	{
		if (_In_str.size() > 16)
			return false;
		_out_num = 0;
		for (int i = 0; i < (int)_In_str.size(); i++)
		{
			_out_num = _out_num << 8;

			int num = 0;
			if (_In_str[i] >= '0'&& _In_str[i] <= '9')
			{
				num = _In_str[i] - '0';
			}
			else if (_In_str[i] >= 'a'&& _In_str[i] <= 'f')
			{
				num = _In_str[i] - 'a' + 10;
			}
			else if (_In_str[i] >= 'A'&& _In_str[i] <= 'F')
			{
				num = _In_str[i] - 'A' + 10;
			}
			else
				return false;

			_out_num += num;
		}

		return true;
	}

	bool hextoint32(const std::string& _In_str,int&& _out_num)
	{
		if (_In_str.size() > 8)
			return false;

		return hextoint64(_In_str, (long long)_out_num);
	}

}