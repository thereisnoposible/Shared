#include "pch.h"

#include "helper.h"

//-------------------------------------------------------------------------------
char charToLower(char c)
{
    return std::tolower(c);
}

//-------------------------------------------------------------------------------
void Helper::setCurrentWorkPath()
{
#ifdef WIN32
	TCHAR pBuffer[1024];
	ZeroMemory(pBuffer, 1024);
	GetModuleFileName(NULL, pBuffer, 1024);

	xstring	 dirname;
	xstring cc=pBuffer;
	xstring::size_type pos =cc.find_last_of("\\");
	dirname = cc.substr(0,pos);
	SetCurrentDirectory(dirname.c_str());
#else
    char buffer[_MAXPATH_L_]={0};
    readlink("/proc/self/exe", buffer, _MAXPATH_L_ - 1);
    xstring cc = buffer;
    xstring::size_type pos = cc.find_last_of("/");
    xstring dirname = cc.substr(0,pos);
    //getcwd(buffer,sizeof(buffer));
    chdir(dirname.c_str());
#endif
}



//-------------------------------------------------------------------------------
void Helper::dbgTrace(char* format,...)
{
#ifdef WIN32
	char szLog[1024]={0};

	va_list argptr;
	va_start(argptr, format);
	vsnprintf_s(szLog,1023 ,1023, format, argptr);
	va_end(argptr);
	OutputDebugString(szLog);
#else
    #pragma WARNING(FIXED ME : linux version)
#endif
}


//-------------------------------------------------------------------------------
double Helper::GetFrequencyTime()
{
	double t = 0;

#ifdef WIN32
	LARGE_INTEGER liFreq;
	LARGE_INTEGER liTime;

	QueryPerformanceFrequency(&liFreq);
	QueryPerformanceCounter(&liTime);

	t=(double)(liTime.QuadPart)/(double)liFreq.QuadPart;
#else
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    t = ts.tv_sec + (double)(ts.tv_nsec*1.0/1000000000);
    /*printf( "ts.tv_sec = %d\n", ts.tv_sec );
    printf( "ts.tv_nsec = %d\n", ts.tv_nsec );
    printf( "double)(ts.tv_nsec*1.0/100000000) = %lf\n", (double)(ts.tv_nsec*1.0/1000000000) );
    printf( "t = %lf\n", t );*/
#endif

	return t;
}



//-------------------------------------------------------------------------------
xstring Helper::Timet2String(time_t t)
{
    if (t < 0)
    {
        t = 0;
    }
    char szTime[TIME_LENGTH] = {0};
#ifdef _WIN32
    struct tm timeinfo;
	localtime_s(&timeinfo,&t);
	struct tm* ptimeinfo = &timeinfo;
#else
    struct tm* ptimeinfo = localtime(&t);
#endif
	strftime(szTime, TIME_LENGTH,"%Y-%m-%d %H:%M:%S",ptimeinfo);

	return szTime;
}


//-------------------------------------------------------------------------------
time_t Helper::String2Timet(const xstring& str)
{
	struct tm t;
	memset((char*)&t,0,sizeof(tm));

#ifdef _WIN32
	sscanf_s(str.c_str(),"%04d-%02d-%02d %02d:%02d:%02d",
		&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec);
#else
    sscanf(str.c_str(),"%04d-%02d-%02d %02d:%02d:%02d",
		&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec);
#endif
	t.tm_year -= 1900;
	t.tm_mon -= 1;

	time_t tmRet = mktime(&t);

	return tmRet;
}

int32 Helper::String2DayTime(xstring t)
{
    xvector<xstring> vec_t;
    Helper::SplitString(t, ":", vec_t);
    
    if (vec_t.size() != 3)
    {
        return 0;
    }

    int32 hour = Helper::StringToInt32(vec_t[0]);
    int32 min = Helper::StringToInt32(vec_t[1]);
    int32 sec = Helper::StringToInt32(vec_t[2]);
   
    int32 day = hour * 60 * 60 + min * 60 + sec;

    return day;
}

xstring Helper::DayTime2String(int32 t)
{
    if (t < 0 || t >= 86400)
        return xstring("00:00:00");

    int32 sec = t % 60;
    int32 min = (t / 60) % 60;
    int32 hour = t / 3600;

    xstring str_t;
    if (hour < 10)
        str_t += "0";
    str_t += Helper::Int32ToString(hour);
    str_t += ":";
    if (min < 10)
        str_t += "0";
    str_t += Helper::Int32ToString(min);
    str_t += ":";
    if (sec < 10)
        str_t += "0";
    str_t += Helper::Int32ToString(sec);

    return str_t;
}

//-------------------------------------------------------------------------------
void Helper::SplitTimet(time_t t,int32& year,int32& mon,int32& day,int32& hour,int32& min,int32& sec)
{
    #ifdef _WIN32
	struct tm _tm;
	localtime_s(&_tm,&t);
	struct tm* ptm = &_tm;
	#else
	struct tm* ptm = localtime(&t);
	#endif

	year = ptm->tm_year + 1900;
	mon = ptm->tm_mon + 1;
	day = ptm->tm_mday;
	hour = ptm->tm_hour;
	min = ptm->tm_min;
	sec = ptm->tm_sec;
}

//-------------------------------------------------------------------------------
int32 Helper::GetDaySeconds(time_t t)
{
	int32 total = 0;
	int32 year, mon, day, hour, min, sec;
	SplitTimet(t, year, mon, day, hour, min, sec);

	total = hour * 60 * 60 + min * 60 + sec;

	return total;
}

//-------------------------------------------------------------------------------
bool Helper::IsTimeChanged(time_t tLast, time_t tCurr, int32 hour, int32 min, int32 sec)
{
#ifdef _WIN32
	struct tm _tm;
	localtime_s(&_tm, &tLast);
	struct tm* p_tm = &_tm;
#else
	struct tm* p_tm = localtime(&tLast);
#endif
	p_tm->tm_hour = hour;
	p_tm->tm_min = min;
	p_tm->tm_sec = sec;

	time_t tSepCurDay = mktime(p_tm);
	time_t tSepNextDay = tSepCurDay + 24 * 60 * 60;

	if (tLast < tSepCurDay)
	{
		if (tCurr >= tSepCurDay)
			return true;
	}
	else
	{
		if (tCurr >= tSepNextDay)
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------
bool Helper::IsTimeChanged(time_t t,int32 hour,int32 min,int32 sec)
{
	time_t tNow = time(NULL);

	return IsTimeChanged(t, tNow, hour, min, sec);
}



//-------------------------------------------------------------------------------
xstring Helper::Int32ToString(int32 i)
{
	char buff[64];

#ifdef _WIN32
	sprintf_s(buff,64,"%d",i);
#else
    sprintf(buff,"%d",i);
#endif
	return xstring(buff);
}


//-------------------------------------------------------------------------------
int32 Helper::StringToInt32(const xstring& str )
{
	int32 i=atoi(str.c_str());

	return i;
}


//-------------------------------------------------------------------------------
xstring Helper::Int64ToString(int64 i)
{
	char buff[64];
#ifdef _WIN32
    sprintf_s(buff,64,"%I64d",i);
#else
    sprintf(buff,"%lld",i);
#endif

	return xstring(buff);
}


//-------------------------------------------------------------------------------
int64 Helper::StringToInt64(const xstring& str )
{
	int64 i = atoll(str.c_str());

	return i;
}

//-------------------------------------------------------------------------------------------
xstring Helper::HexToString(const unsigned char* pdata, int32 len)
{
	xstring str;

	if (pdata != NULL)
	{
		char buff[3] = { 0 };
		for (int32 i = 0; i < len; i++)
		{
		    #ifdef _WIN32
			sprintf_s(buff, 3,"%02x", pdata[i]);
			#else
			sprintf(buff,"%02x", pdata[i]);
			#endif
			str += buff;
		}
	}

	return str;
}


//-------------------------------------------------------------------------------
xstring Helper::PVOIDToString(PVOID p)
{
	xstring ret;

#ifdef _WIN64
	ret = Int32ToString((int32)p);
#else
	ret = Int64ToString((int64)p);
#endif

	return ret;
}


//-------------------------------------------------------------------------------
void* Helper::StringToPVOID(const xstring& str)
{
	void* p = NULL;

#ifdef _WIN64
	p = (void*)StringToInt64(str);
#else
	p = (void*)StringToInt32(str);
#endif // _WIN64

	return p;
}


//-------------------------------------------------------------------------------
void  Helper::SplitString(const xstring&str,const xstring& delims, std::vector<xstring>& ret,bool allowempty)
{
	ret.clear();

	if(str.empty() == true)
	{
		return;
	}

	size_t start, pos;
	start = 0;
	while(true)
	{
		pos = str.find_first_of(delims, start);
		if (pos == start)
		{
			if(allowempty)
				ret.push_back("");
			start = pos + delims.length();
		}
		else if (pos == xstring::npos)
		{
			xstring szTmp = str.substr(start);
			Helper::Trim(szTmp);
			ret.push_back( szTmp );
			break;	//退出循环
		}
		else
		{
			xstring szTmp = str.substr(start, pos - start);
			Helper::Trim(szTmp);
			ret.push_back( szTmp );
			start = pos + delims.length();
		}

	}


	return ;

}


//-------------------------------------------------------------------------------
void Helper::Trim( xstring& str , const xstring& chars )
{
	str.erase(str.find_last_not_of(chars)+1);
	str.erase(0, str.find_first_not_of(chars));
}


//-------------------------------------------------------------------------------
void Helper::GetFileList(const xstring& dir,const xstring& filter,xvector<xstring>& filelist,bool recursion)
{
#ifdef _WIN32
	if(dir.length()==0) return;

	xstring filter_ = filter;

	xstring::size_type pos = filter.find('.');
	if(pos != xstring::npos)
		filter_ = filter.substr(pos+1);

	//转换成小写
	boost::to_lower(filter_);

	xstring csDir = dir;
	char cEnd = csDir.at(csDir.length()-1);
	if(cEnd!='\\' && cEnd!='/')
		csDir += "/";

	xstring csFindFilter = csDir + "*.*";

	struct   _finddata_t   c_file   =   {0};
	long hFile = _findfirst(csFindFilter.c_str(),&c_file);
	if(hFile == -1)
		return;

	do
	{
		if(c_file.attrib&_A_SUBDIR && recursion)
		{
			if(strcmp(c_file.name,".") ==0 ||
				strcmp(c_file.name,"..") == 0 ||
				strcmp(c_file.name,".svn") == 0 )
			{
				continue;
			}
			xstring subdir = csDir + c_file.name;
			GetFileList(subdir,filter_,filelist,recursion);
		}
		else
		{
			if(filter_ != "*" && !filter_.empty())
			{//需要验证后缀名
				xstring csExt = c_file.name;
				xstring::size_type pos = csExt.find_last_of(".");
				if(pos==xstring::npos) continue;

				csExt = csExt.substr(pos+1);
				boost::to_lower(csExt);

				if(csExt!=filter_) continue;

			}
			filelist.push_back(csDir+c_file.name);
		}

	} while (_findnext(hFile,&c_file) == 0 );
#else
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir.c_str())) == NULL)
    {
        return;
    }

    while((entry = readdir(dp)) != NULL)
    {
        xstring fullpath = dir + "/";
        fullpath += xstring(entry->d_name);

        lstat(fullpath.c_str(),&statbuf);
        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(".",entry->d_name) == 0 ||
                strcmp("..",entry->d_name) == 0)
            {
                continue;
            }
            GetFileList(fullpath,filter,filelist);
        }
        else
        {
            filelist.push_back(fullpath);
        }
    }

    closedir(dp);

#endif
}


//-------------------------------------------------------------------------------
xstring Helper::GetRandStr(int32 len)
{
	xstring str;

	if (len<0)
	{
		return str;
	}

	for (int i=0;i<len;i++)
	{
		char a=rand()%94+33;		//可见字符，排除空格

		str+=a;
	}

	return str;
}

//-----------------------------------------------------------------------
xstring Helper::Gb2312ToUTF8( const  xstring& gb2312)
{
	if(gb2312.empty())
		return "";

	xstring tem = "";

#ifdef WIN32
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_ACP, 0, gb2312.c_str(), -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);

	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr)
		delete[] wstr;

	tem =str;

	delete[]str;
#else
	iconv_t cd;
	if( (cd = iconv_open("gb2312","utf-8")) ==0 )
	{
        return tem;
	}

    char* src = const_cast<char*>(gb2312.c_str());
    size_t src_len = gb2312.length();

    char dst[1024] = {0};
    size_t dst_len = sizeof(dst);

    char* in = src;
    char* out = dst;

    if(iconv(cd,&in,&src_len,&out,&dst_len)!=0)
    {
        tem = dst;
    }

#endif

	return tem;
}


xstring Helper::UTF8ToGb2312(const  xstring& utf8)
{
	if(utf8.empty())
		return "";

	xstring tem = "";

#ifdef WIN32

	int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr)
		delete[] wstr;
	tem = str;
	delete [] str;

#else
    iconv_t cd;
	if( (cd = iconv_open("utf-8","gb2312")) ==0 )
	{
        return tem;
	}

    char* src = const_cast<char*>(utf8.c_str());
    size_t src_len = utf8.length();

    char dst[1024] = {0};
    size_t dst_len = sizeof(dst);

    char* in = src;
    char* out = dst;

    if(iconv(cd,&in,&src_len,&out,&dst_len)!=0)
    {
        tem = dst;
    }
#endif

	return tem;
}

//-------------------------------------------------------------------------------------------
std::wstring  Helper::UTF8ToUnicode(const xstring& utf8)
{
	if (utf8.empty())
	{
		return L"";
	}

	std::wstring strUnicode = L"";

#ifdef _WIN32
    int32 dwUnicodeLen;
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
	if( (cd = iconv_open("utf-8","wchar_t")) ==0 )
	{
        return strUnicode;
	}

    char* src = const_cast<char*>(utf8.c_str());
    size_t src_len = utf8.length();

    wchar_t dst[1024] = {0};
    size_t dst_len = sizeof(dst);

    char* in = src;
    char* out = (char*)dst;

    if(iconv(cd,&in,&src_len,&out,&dst_len)!=0)
    {
        strUnicode = dst;
    }
#endif

	return strUnicode;
}

std::string Helper::UnicodeToGb2312(const std::wstring unico)
{
    std::string strTmp = "";

    #ifdef _WIN32

    #else
    iconv_t cd;
	if( (cd = iconv_open("wchat_t","gb2312")) ==0 )
	{
        return strTmp;
	}

    const wchar_t* src = unico.c_str();
    size_t src_len = unico.length();

    char dst[1024] = {0};
    size_t dst_len = sizeof(dst);

    char* in = const_cast<char*>((const char *)src);
    char* out = dst;

    if(iconv(cd,&in,&src_len,&out,&dst_len)!=0)
    {
        strTmp = dst;
    }
#endif
    return strTmp;
}

//-------------------------------------------------------------------------------------------
std::wstring Helper::Gb2312ToUnicode(const std::string gb2312)
{
    std::wstring strTmp = L"";

    #ifdef _WIN32

    #else
    iconv_t cd;
	if( (cd = iconv_open("gb2312","wchat_t")) ==0 )
	{
        return strTmp;
	}

    const char* src = gb2312.c_str();
    size_t src_len = gb2312.length();

    char dst[1024] = {0};
    size_t dst_len = sizeof(dst);

    char* in = const_cast<char*>(src);
    char* out = dst;

    if(iconv(cd,&in,&src_len,&out,&dst_len)!=0)
    {
        strTmp = (wchar_t*)dst;
    }
#endif
    return strTmp;
}

//-------------------------------------------------------------------------------------------
bool Helper::IsTextUTF8(const char* str, int32 length)
{
	int i;
	int32 nBytes = 0;					//UFT8可用1-6个字节编码,ASCII用一个字节
	UCHAR chr;
	bool bAllAscii = true;				//如果全部都是ASCII, 说明不是UTF-8
	for (i = 0; i<length; i++)
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

//-----------------------------------------------------------------------
int32 Helper::GetRandom(int32 begin,int32 end)
{
	int nField = end - begin + 1;

	int nRand = rand();

	int nValue = nRand%nField;

	nValue += begin;

	return nValue;
}

//-----------------------------------------------------------------------
xstring Helper::toupper(xstring str)
{
#ifdef _WIN32
	std::transform(str.begin(),str.end(),str.begin(),std::toupper);
#else
    std::transform(str.begin(),str.end(),str.begin(),charToLower);
#endif
	return str;
}



xstring Helper::tolower(xstring str)
{
#ifdef _WIN32
	std::transform(str.begin(),str.end(),str.begin(),std::tolower);
#else
    std::transform(str.begin(),str.end(),str.begin(),charToLower);
#endif

	return str;
}

//-------------------------------------------------------------------------------------------
void Helper::EnableMemLeakCheck()
{
#ifdef _WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
}

//-------------------------------------------------------------------------------------------
xstring Helper::GetAddressIP(const xstring& addr)
{
	xstring ip;

	StringArray saTmp;
	Helper::SplitString(addr, ":", saTmp);

	if (saTmp.size() == 2)
	{
		ip = saTmp[0];
	}

	return ip;
}

//-------------------------------------------------------------------------------------------
int32 Helper::GetAddressPort(const xstring& addr)
{
	int32 port = 0;

	StringArray saTmp;
	Helper::SplitString(addr, ":", saTmp);

	if (saTmp.size() == 2)
	{
		xstring sPort = saTmp[1];
		port = Helper::StringToInt32(sPort);
	}

	return port;
}

size_t Helper::DJBHashCode(const xstring& str)
{
	size_t hashcode = 5381;

	for (size_t i = 0; i < str.size(); ++i)
	{
		hashcode = ((hashcode << 5) + hashcode) + str[i];
	}
	return hashcode;
}

xstring Helper::GetRandStr(const char* str, int32 len)
{
	int32 slen = strlen(str);
	std::stringstream ss;
	int32 strIndex = 0;
	for (int32 i = 0; i < len; ++i)
	{

		strIndex = Helper::GetRandom(0, slen - 1);
		ss << str[strIndex];
	}
	return ss.str();
}
