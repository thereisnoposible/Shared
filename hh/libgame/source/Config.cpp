#include "pch.h"
#include "Config.h"

#ifdef _WIN32
#else
#define sprintf_s sprintf
#define sscanf_s sscanf
#endif // _WIN32

template<> Config* Singleton<Config>::s_pInstance = NULL;

//---------------------------------------------------------------------------
Config::Config(const xstring& filename)
{
	m_IniFile.Load(filename);
}


//---------------------------------------------------------------------------
Config::~Config(void)
{

}


//---------------------------------------------------------------------------
int32 Config::GetIntValue(const xstring& section,const xstring& key,int32 defaultv)
{
	char szTmp[10];
	sprintf_s(szTmp,"%d",defaultv);
	xstring sValue = GetStringValue(section,key,szTmp);

	int32 iValue = defaultv;
	sscanf_s(sValue.c_str(),"%d",&iValue);

	return iValue;
}


//---------------------------------------------------------------------------
float Config::GetFloatValue(const xstring& section,const xstring& key,float defaultv)
{
	char szTmp[10];
	sprintf_s(szTmp,"%f",defaultv);
	xstring sValue = GetStringValue(section,key,szTmp);

	float fValue = defaultv;
	sscanf_s(sValue.c_str(),"%f",&fValue);

	return fValue;
}


//---------------------------------------------------------------------------
xstring Config::GetStringValue(const xstring& section,const xstring& key,const xstring& defaultv)
{
	CIniSectionA* pSection = m_IniFile.GetSection(section);
	if(pSection==NULL) return defaultv;

	CIniKeyA* pKey = pSection->GetKey(key);
	if(pKey==NULL) return defaultv;

	xstring sValue = pKey->GetValue();

	return sValue;
}


//---------------------------------------------------------------------------
xstring Config::GetUrlPath(const xstring& section, const xstring& key, const xstring& defaultv)
{
	xstring sValue = GetStringValue(section, key, defaultv);

	if (sValue[sValue.length() - 1] != '/')
	{
		sValue += '/';
	}

	return sValue;
}

