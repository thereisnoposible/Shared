
/**********************************************
�����࣬һ�㳣�õĺ������������
************************************************/

#pragma  once

#include <time.h>


class XClass Helper
{

public:
	Helper(){}

	~Helper(){}


	/**���õ�ǰ����Ŀ¼Ϊ����Ŀ¼*/
	static void  setCurrentWorkPath();

	/**
	* @brief		����������������ʱ�䣬���뾫��
	* @return		double : ����
	* @remarks
	*/
	static double GetFrequencyTime();

	/**
	* @brief		���debugview������Ϣ
	* @details		֧�ֿɱ���������������Ϣ��debugview����windows����
	* @param[in]	format : ��ʽ���ַ���
	* @return		void
	* @remarks
	*/
	static void dbgTrace(char* format,...);

	/**
	* @brief		time_tתʱ���ַ���
	* @details
	* @param[in]	t[time_t] : ʱ��
	* @return		xstring : ʱ���ַ���
	* @remarks
	*/
	static xstring Timet2String(time_t t);

	/**
	* @brief		ʱ���ַ���תʱ��
	* @param[in]	str[xstring] : ʱ���ַ���
	* @return		time_t
	* @remarks
	*/
	static time_t String2Timet(const xstring& str);

    /**
    * @brief		ʱ����תʱ��
    * @details
    * @param[in]	xstring : hour:min:sec
    * @return		t[int32] : ʱ��
    * @remarks
    */
    static int32 String2DayTime(xstring t);
    /**
    * @brief		ʱ��תʱ����
    * @details
    * @param[in]	t[int32] : ʱ��
    * @return		xstring : hour:min:sec
    * @remarks
    */
    static xstring DayTime2String(int32 t);


	/**
	* @brief		��time_tת����������ʱ����
	* @details
	* @param[in]	t[time_t] : ʱ��
	* @param[in]	year[int32&] : ��
	* @param[in]	mon[int32&] : ��
	* @param[in]	day[int32&] : ��
	* @param[in]	hour[int32&] : ʱ
	* @param[in]	min[int32&] : ��
	* @param[in]	sec[int32&] : ��
	* @return		void
	* @remarks
	*/
	static void SplitTimet(time_t t,int32& year,int32& mon,int32& day,int32& hour,int32& min,int32& sec);


	/**
	* @brief		�ж�ʱ���Ƿ�任
	* @details		��ǰʱ���Ƿ��Ѿ�����ָ����ʱ���������hour,min,sec
	* @param[in]	t : ����ʱ��
	* @param[in]	hour,min,sec : ʱ���л��� ʱ �� ��
	* @return		bool : true=ʱ�����л� false=ʱ�����л�
	* @remarks
	*/
	static bool IsTimeChanged(time_t t,int32 hour,int32 min,int32 sec);

	static bool IsTimeChanged(time_t tLast, time_t tCurr, int32 hour, int32 min, int32 sec);

	/**
	* @brief		��ȡʱ��������ľ���������
	*/
	static int32 GetDaySeconds(time_t t);


	/**
	* @brief		32λ����ת��Ϊ�ַ���
	* @details
	* @param[in]	i[int32] : ��ת������
	* @return		xstring : �����ַ���
	* @remarks
	*/
	static xstring Int32ToString(int32 i);


	/**
	* @brief		�ַ���תInt
	* @details
	* @param[in]	str[xstring] : ��ת���ַ���
	* @return		int32 : ת�����
	* @remarks
	*/
	static int32 StringToInt32(const xstring& str );


	/**
	* @brief		64λ����ת��Ϊ�ַ���
	* @details
	* @param[in]	i[int64] : ��ת������
	* @return		xstring : �����ַ���
	* @remarks
	*/
	static xstring Int64ToString(int64 i);


	/**
	* @brief		�ַ���תInt64
	* @details
	* @param[in]	str[xstring] : ��ת���ַ���
	* @return		int64 : ת�����
	* @remarks
	*/
	static int64 StringToInt64(const xstring& str );


	/**
	* @brief		16����ת��Ϊ�ַ���
	*/
	static xstring HexToString(const unsigned char* pdata, int32 len);


	/**
	* @brief		GB2312תUTF8����
	*/
	static xstring Gb2312ToUTF8( const xstring& gb2312);

	/**
	* @brief		UTF8ת����GB2312
	*/
	static xstring UTF8ToGb2312(const  xstring& uft8);

	/**
	* @brief		UTF8ת����unicode
	*/
	static std::wstring  UTF8ToUnicode(const xstring& utf8);

	/**
	* @brief		unicodeת����gb2312
	*/
	static std::string UnicodeToGb2312(const std::wstring unico);

	static std::wstring Gb2312ToUnicode(const std::string gb2312);

	/**
	* @brief		�ж��ַ����Ƿ���utf8
	*/
	static bool IsTextUTF8(const char* str, int32 length);


	/**
	* @brief		ָ���ַת�ַ���
	* @details		32λ��64λ��������
	* @param[in]	p[PVOID] : ��ַ
	* @return		xstring : ��ַת�����ַ���
	* @remarks
	*/
	static xstring PVOIDToString(PVOID p);


	/**
	* @brief		�ַ���תָ��
	* @details
	* @param[in]	str[xstring] : PVOID string
	* @return		void*
	* @remarks
	*/
	static void* StringToPVOID(const xstring& str);

	/**
	* @brief		�ָ��ַ���
	* @details
	* @param[in]	str[xstring] : ���ָ��ַ���
	* @param[in]	delims[xstring] : �ָ����
	* @param[out]	ret : �ָ���
	* @return
	* @remarks
	*/
	static void SplitString(const xstring& str,const xstring& delims, std::vector<xstring>& ret,bool allowempty=false);

	/**
	* @brief		ȥ��ǰ����ַ�
	*/
	static void Trim( xstring& str , const xstring& chars="\r\t " );


	/**
	* @brief		��ȡ·���µ�ָ���ļ������б�
	* @param[in]	dir[xstring] : ·��
	* @param[in]	filter[xstring] : ָ���ļ���׺
	* @param[out]	filelist[xvector] : �ļ��б�
	* @param[in]	recursion[bool] : �Ƿ�������ļ���
	* @remarks
	*/
	static void GetFileList(const xstring& dir,const xstring& filter,xvector<xstring>& filelist,bool recursion=true);

	/**
	* @brief		��ȡ����ַ���
	* @param[in]	len[int32] : ����ĳ���
	* @return
	* @remarks
	*/
	static xstring GetRandStr(int32 len);
	/**
	* @brief		��ȡ����ַ���
	* @param[in]	len[int32] : ����ĳ���
	* @return
	* @remarks
	*/
	static xstring GetRandStr(const char* str, int32 len);
	/**
	* @brief		��ȡ�����
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
