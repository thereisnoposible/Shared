#ifndef __SS_SAVE_FILE_H__
#define __SS_SAVE_FILE_H__
#include <string>
#include <cstdio>
#include <cstdarg>
#include "../time/ssTime.h"
#include <fstream>


namespace StarSeeker
{

class ssWriteFile
{
public:
	ssWriteFile();
	~ssWriteFile();

	bool Init(const char* path, bool isBinary=false, bool brief=true);
	bool WriteString(const char* str);
	bool WriteStringWithTime(const char* str);
	bool WriteBytes(const char* str, u32 len);
	bool OpenFile();
	void CloseFile();
	//自定义文件流
	void WriteTime();
	std::ofstream & Stream() {return m_fstream;}

private:
	bool			m_brief;	//写完立即关闭文件句柄
	std::string		m_path;		//文件路径
	std::ofstream	m_fstream;	//文件流
	std::string		m_tmpStr;
	bool			m_isBinary;	//二进制文件
};

}
#endif
