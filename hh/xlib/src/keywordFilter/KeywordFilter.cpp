
#include "../include/keywordFilter/KeywordFilter.h"
#include "../include/helper/Helper.h"

#include "boost/regex.hpp"
#include "boost/format.hpp"    
#include "boost/tokenizer.hpp" 
#include "boost/algorithm/string.hpp"

#include <fstream>

using namespace xlib;

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
				*(point + i) = Helper::StringToInt32(temp);
		}

		//pos = 0;
		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//province = Helper::StringToInt(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//city = Helper::StringToInt(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//county = Helper::StringToInt(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 4);
		//pos += 4;
		//year = Helper::StringToInt(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//month = Helper::StringToInt(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//day = Helper::StringToInt(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 2);
		//pos += 2;
		//code = Helper::StringToInt(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 1);
		//pos += 1;
		//sex = Helper::StringToInt(temp);

		//temp.assign(str.begin() + pos, str.begin() + pos + 1);
		//pos += 1;
		//if (temp == "X" || temp == "x")
		//	checkcode = 10;
		//else
		//	checkcode = Helper::StringToInt(temp);
	}

	bool check(KeywordFilter* filter = nullptr)
	{
		return (checkAreaCode(filter) && checkCode());
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
			total += Helper::StringToInt32(str) * Wi[i];
		}
		total = total % 11;
		return Y[total] == checkcode;
	}

	bool checkAreaCode(KeywordFilter* filter)
	{
		if (!filter)
			return true;

		return filter->IsRealAreaCode(province, city, county);
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

//-------------------------------------------------------------------------------------------
KeywordFilter::KeywordFilter()
{
	init();
}

//-------------------------------------------------------------------------------------------
KeywordFilter::~KeywordFilter()
{
	if (m_pDFATable)
	{
		delete m_pDFATable;
		m_pDFATable = nullptr;
	}
}

//-------------------------------------------------------------------------------------------
bool KeywordFilter::IsRealAreaCode(int32 province, int32 city, int32 county)
{
	std::map<int32, std::map<int32, std::set<int32>>>::iterator it = areacode.find(province);
	if (it == areacode.end())
		return false;

	//xmap<int32, xset<int32>>::iterator cityit = it->second.find(city);
	//if (cityit == it->second.end())
	//	return false;

	//if (cityit->second.find(county) == cityit->second.end())
	//	return false;

	return true;
}

//-------------------------------------------------------------------------------------------
void KeywordFilter::init()
{
	m_pDFATable = new DFANode(0);

	std::string path = "KeyWord.txt";

	m_pDFATable->loadFile(path);

	std::ifstream file;
	char p[1024] = { 0 };
	std::string code;
	file.open("Codesfortheadministrativedivisions.txt");
	while (!file.fail() && file.eof() == false)
	{
		file.getline(p, 1024);
		code = Helper::UTF8ToGb2312(p);

		std::vector<std::string> str;
		boost::split(str, code, boost::is_any_of(" "));
		std::string zero = "00";
		//排除了 台湾 香港 澳门

		int32 province = Helper::StringToInt32(str[0].substr(0, 2));
		int32 city = Helper::StringToInt32(str[0].substr(2, 2));
		int32 county = Helper::StringToInt32(str[0].substr(4, 2));

		if (city == 0 || county == 0)
			continue;

		if (areacode.find(province) == areacode.end())
		{
			std::map<int32, std::set<int32>> temp;
			temp.insert(std::make_pair(city, std::set<int32>()));
			areacode.insert(std::make_pair(province, temp));
		}
		((areacode[province])[city]).insert(county);				
		
	}
//	areacode[70][00].insert(00);	//台湾
//	areacode[81][00].insert(00);	//香港
//	areacode[82][00].insert(00);	//澳门
}

//boost::regex expression("^[a-zA-Z][a-zA-Z0-9_@]{3,15}$");
boost::regex expression("^[a-zA-Z0-9_]{6,16}$");

//-------------------------------------------------------------------------------------------
bool KeywordFilter::IsLaywerAccount(const std::string& str)
{
	return boost::regex_match(str,expression);
}

//boost::regex expressionPassword("^[a-zA-Z0-9_@]{6,16}$");
boost::regex expressionPassword("^[a-zA-Z0-9_]{6,16}$");
bool KeywordFilter::IsLayerPassword(const std::string& str)
{
	return boost::regex_match(str, expressionPassword);
}

//-------------------------------------------------------------------------------------------
bool KeywordFilter::HasFilterWord(const std::string& str)
{
	return m_pDFATable->isFileterWord(str);
}


//-------------------------------------------------------------------------------------------
bool KeywordFilter::HasFilterWordAndReplace(std::string& str, const std::string& replace)
{
	bool bTrue = false;

	if (m_pDFATable->isFileterWord(str))
	{
		bTrue = true;
		int32 nStart,nlen;
		m_pDFATable->getKeyPos(nStart, nlen);
//		wpData.replace(nStart, nlen, replace);
		UnicodePosToUTF8Pos(nStart, nlen, str, nStart, nlen);
		str.replace(nStart, nlen, replace);
	}
	return bTrue;
}

bool KeywordFilter::LoopFilterWordAndReplace(std::string& str, const std::string& replace, int max_loop_count)
{
    int count = 0;
    while (HasFilterWordAndReplace(str, "***"))
    {
        count++;

        if (count >= max_loop_count)
            break;
    }

    return count > 0;
}

void KeywordFilter::UnicodePosToUTF8Pos(int32 uniStart, int32 uniLen, std::string utfStr, int32& utfStart, int32& utfLen)
{
	int32 count = -1;
	for (size_t i = 0, len = 0; i < utfStr.length();)
	{
		unsigned char byte = (unsigned)utfStr[i];
		if (byte >= 0xFC) // lenght 6
			len = 6;
		else if (byte >= 0xF8)
			len = 5;
		else if (byte >= 0xF0)
			len = 4;
		else if (byte >= 0xE0)
			len = 3;
		else if (byte >= 0xC0)
			len = 2;
		else
			len = 1;		

		count++;
		if (count == uniStart)
			utfStart = i;

		i += len;

		if (count == uniStart + uniLen - 1)
		{
			utfLen = i - utfStart;
			return;
		}
	}
}

int32 KeywordFilter::GetUTF8StringSize(std::string utfStr)
{
	int32 count = 0;
	for (size_t i = 0, len = 0; i != utfStr.length();)
	{
		if (i > utfStr.length())
			return -1;
		unsigned char byte = (unsigned)utfStr[i];
		if (byte >= 0xFC) // lenght 6
			len = 6;
		else if (byte >= 0xF8)
			len = 5;
		else if (byte >= 0xF0)
			len = 4;
		else if (byte >= 0xE0)
			len = 3;
		else if (byte >= 0xC0)
			len = 2;
		else
			len = 1;

		i += len;
		if (len > 1)
			count += 2;
		else
			count += 1;
	}
	return count;
}

boost::regex expressionname("^[^ ]+$");

bool KeywordFilter::IsLaywerName(const std::string& str)
{
	return boost::regex_match(str, expressionname);
}

boost::regex regexidcard("^[1-9]\\d{5}[1-9]\\d{3}((0[1-9])|(1[0-2]))(([0][1-9]|[12]\\d)|3[0-1])\\d{3}([0-9]|X|x)$");

bool KeywordFilter::IsLaywerIDCard(const std::string& idcard)
{
	return (boost::regex_match(idcard, regexidcard) && IDCARD("", idcard).check());
}
