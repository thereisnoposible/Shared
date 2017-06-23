
#include "../include/mysql2/CustParament.h"

using namespace xlib;
//----------------------------------------------------------------------------------------------
CustParament::CustParament()
{

}

//----------------------------------------------------------------------------------------------
CustParament::~CustParament()
{

}

//----------------------------------------------------------------------------------------------
bool  CustParament::addValue(const std::string& key, const std::string& val)
{
	if(hasKey(key))
	{
		return false;
	}

	m_Paramnets.insert(std::make_pair(key,val));
	return true;
}

//----------------------------------------------------------------------------------------------
bool  CustParament::getValue(const std::string& key, std::string& val)
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
bool CustParament::hasKey(const std::string& key)
{
	ParamentMap::iterator it=m_Paramnets.find(key);
	if(it==m_Paramnets.end())
	{
		return false;
	}
	return true;

}