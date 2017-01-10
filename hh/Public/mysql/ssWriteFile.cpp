#include "ssWriteFile.h"
#include "../ssAssert.h"

namespace StarSeeker
{

ssWriteFile::ssWriteFile()
{
	m_brief = true;
	m_path.clear();
	m_isBinary = false;
}

ssWriteFile::~ssWriteFile()
{
	CloseFile();
}

bool ssWriteFile::Init(const char* path, bool isBinary, bool brief)
{
	m_path = path;
	m_brief = brief;
	m_isBinary = isBinary;
	if (m_fstream.is_open())
	{
		CloseFile();
	}
	if (!m_brief)
	{
		if (!OpenFile())
		{
			return false;
		}
	}
	return true;
}

bool ssWriteFile::WriteString(const char* str)
{
	if (m_brief)
	{
		OpenFile();
	}
	if (!m_fstream.is_open())
	{
		return false;
	}

	m_fstream<<str;

	if (m_brief)
	{
		CloseFile();
	}
	return true;
}

bool ssWriteFile::WriteStringWithTime(const char* str)
{
	if (m_brief)
	{
		OpenFile();
	}
	if (!m_fstream.is_open())
	{
		return false;
	}

	char t[SS_TIME_STR_LEN] = {0};
	ssTime::GetLocalTime(t);
	m_tmpStr = "[";
	m_tmpStr += t;
	m_tmpStr += "]";
	m_tmpStr += str;

	m_fstream<<m_tmpStr;

	if (m_brief)
	{
		CloseFile();
	}
	return true;
}

bool ssWriteFile::OpenFile()
{
	if (m_fstream.is_open())
	{
		return true;
	}
	if (m_isBinary)
	{
		m_fstream.open(m_path.c_str(), std::ios::app|std::ios::binary);
		//m_pfile = ::fopen(m_path.c_str(), "wb");
	}
	else
	{
		m_fstream.open(m_path.c_str(), std::ios::app);
		//m_pfile = ::fopen(m_path.c_str(), "a");
	}
	return m_fstream.is_open();
}

void ssWriteFile::CloseFile()
{
	//::fclose(m_pfile);
	m_fstream.close();
}

bool ssWriteFile::WriteBytes( const char* str, u32 len )
{
	if (m_brief)
	{
		OpenFile();
	}
	if (!m_fstream.is_open())
	{
		return false;
	}

	//::fwrite(str, len, 1, m_pfile);
	m_fstream.write(str, len);

	if (m_brief)
	{
		CloseFile();
	}
	return true;
}

void ssWriteFile::WriteTime()
{
	char t[SS_TIME_STR_LEN] = {0};
	ssTime::GetLocalTime(t);
	m_tmpStr = "[";
	m_tmpStr += t;
	m_tmpStr += "]";

	m_fstream<<m_tmpStr;
}

}
