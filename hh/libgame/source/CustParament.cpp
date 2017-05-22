


#include "pch.h"
#include "CustParament.h"


//----------------------------------------------------------------------------------------------
CustParament::CustParament()
{

}

//----------------------------------------------------------------------------------------------
CustParament::~CustParament()
{

}

//----------------------------------------------------------------------------------------------
bool  CustParament::addValue(const xstring& key,const xstring& val)
{
	if(hasKey(key))
	{
		return false;
	}

	m_Paramnets.insert(std::make_pair(key,val));
	return true;
}


//----------------------------------------------------------------------------------------------
bool  CustParament::getValue(const xstring& key,xstring& val)
{

	ParamentMap::iterator it=m_Paramnets.find(key);
	if(it==m_Paramnets.end())
	{
		return false;
	}
	val=it->second;
	return true;
}






//----------------------------------------------------------------------------------------------
bool CustParament::hasKey(const xstring& key)
{
	ParamentMap::iterator it=m_Paramnets.find(key);
	if(it==m_Paramnets.end())
	{
		return false;
	}
	return true;

}