
/**********************************************
工具类，一般常用的函数放这个类里
************************************************/

#pragma  once

#include <time.h>


class XClass Helper
{

public:
	Helper(){}

	~Helper(){}


	/**设置当前运行目录为工作目录*/
	static void  setCurrentWorkPath();

	/**
	* @brief		从启动起所经历的时间，毫秒精度
	* @return		double : 秒数
	* @remarks
	*/
	static double GetFrequencyTime();

	/**
	* @brief		输出debugview调试信息
	* @details		支持可变参数，输出调试信息到debugview，仅windows可用
	* @param[in]	format : 格式化字符串
	* @return		void
	* @remarks
	*/
	static void dbgTrace(char* format,...);

	/**
	* @brief		time_t转时间字符串
	* @details
	* @param[in]	t[time_t] : 时间
	* @return		xstring : 时间字符串
	* @remarks
	*/
	static xstring Timet2String(time_t t);

	/**
	* @brief		时间字符串转时间
	* @param[in]	str[xstring] : 时间字符串
	* @return		time_t
	* @remarks
	*/
	static time_t String2Timet(const xstring& str);

    /**
    * @brief		时分秒转时间
    * @details
    * @param[in]	xstring : hour:min:sec
    * @return		t[int32] : 时间
    * @remarks
    */
    static int32 String2DayTime(xstring t);
    /**
    * @brief		时间转时分秒
    * @details
    * @param[in]	t[int32] : 时间
    * @return		xstring : hour:min:sec
    * @remarks
    */
    static xstring DayTime2String(int32 t);


	/**
	* @brief		把time_t转换成年月日时分秒
	* @details
	* @param[in]	t[time_t] : 时间
	* @param[in]	year[int32&] : 年
	* @param[in]	mon[int32&] : 月
	* @param[in]	day[int32&] : 日
	* @param[in]	hour[int32&] : 时
	* @param[in]	min[int32&] : 分
	* @param[in]	sec[int32&] : 秒
	* @return		void
	* @remarks
	*/
	static void SplitTimet(time_t t,int32& year,int32& mon,int32& day,int32& hour,int32& min,int32& sec);


	/**
	* @brief		判断时间是否变换
	* @details		当前时间是否已经过掉指定的时间所在天的hour,min,sec
	* @param[in]	t : 参照时间
	* @param[in]	hour,min,sec : 时间切换的 时 分 秒
	* @return		bool : true=时间有切换 false=时间无切换
	* @remarks
	*/
	static bool IsTimeChanged(time_t t,int32 hour,int32 min,int32 sec);

	static bool IsTimeChanged(time_t tLast, time_t tCurr, int32 hour, int32 min, int32 sec);

	/**
	* @brief		获取时间所在天的经过的秒数
	*/
	static int32 GetDaySeconds(time_t t);


	/**
	* @brief		32位整型转换为字符串
	* @details
	* @param[in]	i[int32] : 待转换整型
	* @return		xstring : 返回字符串
	* @remarks
	*/
	static xstring Int32ToString(int32 i);


	/**
	* @brief		字符串转Int
	* @details
	* @param[in]	str[xstring] : 待转换字符串
	* @return		int32 : 转换结果
	* @remarks
	*/
	static int32 StringToInt32(const xstring& str );


	/**
	* @brief		64位整型转换为字符串
	* @details
	* @param[in]	i[int64] : 待转换整型
	* @return		xstring : 返回字符串
	* @remarks
	*/
	static xstring Int64ToString(int64 i);


	/**
	* @brief		字符串转Int64
	* @details
	* @param[in]	str[xstring] : 待转换字符串
	* @return		int64 : 转换结果
	* @remarks
	*/
	static int64 StringToInt64(const xstring& str );


	/**
	* @brief		16进制转换为字符串
	*/
	static xstring HexToString(const unsigned char* pdata, int32 len);


	/**
	* @brief		GB2312转UTF8编码
	*/
	static xstring Gb2312ToUTF8( const xstring& gb2312);

	/**
	* @brief		UTF8转换成GB2312
	*/
	static xstring UTF8ToGb2312(const  xstring& uft8);

	/**
	* @brief		UTF8转换成unicode
	*/
	static std::wstring  UTF8ToUnicode(const xstring& utf8);

	/**
	* @brief		unicode转换成gb2312
	*/
	static std::string UnicodeToGb2312(const std::wstring unico);

	static std::wstring Gb2312ToUnicode(const std::string gb2312);

	/**
	* @brief		判断字符串是否是utf8
	*/
	static bool IsTextUTF8(const char* str, int32 length);


	/**
	* @brief		指针地址转字符串
	* @details		32位与64位存在区别
	* @param[in]	p[PVOID] : 地址
	* @return		xstring : 地址转换的字符串
	* @remarks
	*/
	static xstring PVOIDToString(PVOID p);


	/**
	* @brief		字符串转指针
	* @details
	* @param[in]	str[xstring] : PVOID string
	* @return		void*
	* @remarks
	*/
	static void* StringToPVOID(const xstring& str);

	/**
	* @brief		分割字符串
	* @details
	* @param[in]	str[xstring] : 待分割字符串
	* @param[in]	delims[xstring] : 分割符号
	* @param[out]	ret : 分割结果
	* @return
	* @remarks
	*/
	static void SplitString(const xstring& str,const xstring& delims, std::vector<xstring>& ret,bool allowempty=false);

	/**
	* @brief		去除前后的字符
	*/
	static void Trim( xstring& str , const xstring& chars="\r\t " );


	/**
	* @brief		获取路径下的指定文件类型列表
	* @param[in]	dir[xstring] : 路径
	* @param[in]	filter[xstring] : 指定文件后缀
	* @param[out]	filelist[xvector] : 文件列表
	* @param[in]	recursion[bool] : 是否遍历子文件夹
	* @remarks
	*/
	static void GetFileList(const xstring& dir,const xstring& filter,xvector<xstring>& filelist,bool recursion=true);

	/**
	* @brief		获取随机字符串
	* @param[in]	len[int32] : 需求的长度
	* @return
	* @remarks
	*/
	static xstring GetRandStr(int32 len);
	/**
	* @brief		获取随机字符串
	* @param[in]	len[int32] : 需求的长度
	* @return
	* @remarks
	*/
	static xstring GetRandStr(const char* str, int32 len);
	/**
	* @brief		获取随机数
	*/
	static int32 GetRandom(int32 begin,int32 end);

	static xstring toupper(xstring str);
	static xstring tolower(xstring str);

	static void EnableMemLeakCheck();

	static xstring GetAddressIP(const xstring& addr);

	static int32 GetAddressPort(const xstring& addr);

	template < typename _Type >
	static void ChaosVector(xvector<_Type>& vac, int32 n)
	{
		if (n > static_cast<int32>(vac.size()) )
		{
			n = vac.size();
		}
		int32 index = 0;
		for (int32 i = 0; i < n; ++i)
		{
			index = GetRandom(i, vac.size() - 1);
			std::swap(vac[i], vac[index]);
		}
		
	}

	static size_t DJBHashCode(const xstring& str);


};
