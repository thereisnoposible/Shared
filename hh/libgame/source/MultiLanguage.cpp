#include "pch.h"
#include "helper.h"
#include "MultiLanguage.h"

template<> MultiLanguage* Singleton<MultiLanguage>::s_pInstance=NULL;

//---------------------------------------------------------------------------------------
MultiLanguage::MultiLanguage()
{

}

//---------------------------------------------------------------------------------------
MultiLanguage::~MultiLanguage()
{

}

//---------------------------------------------------------------------------------------
void MultiLanguage::loadLanguageFile( const xstring& filename )
{
	std::ifstream input;

	input.open( filename.c_str() , std::ios::binary);

	if( !input.is_open() )
		return;

	std::string csRead;
	while( std::getline( input , csRead ) )
	{
		xstring::size_type pos = csRead.find_first_of('=');
		if(pos != xstring::npos)
		{//²ð·Ö³Ékey£¬value
			xstring csKey = csRead.substr(0,pos);
			xstring csVal = csRead.substr(pos+1);

			m_LanguagePack.insert(std::make_pair(csKey,csVal));
		}
	}
}


//---------------------------------------------------------------------------------------
const xstring MultiLanguage::ToLanguage( const xstring &csKey )
{
	LanguageMap::iterator it = m_LanguagePack.find(csKey);
	if(it != m_LanguagePack.end())
	{
		return it->second;
	}

	return csKey;
}

