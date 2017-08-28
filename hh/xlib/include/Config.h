#pragma once
#include <stdio.h>
#include <memory.h>
#include "helper/Helper.h"
#include <sys/stat.h>
#include <vector>
#include <string>
#include <hash_map>

namespace xlib
{
	class Config
	{
	public:
		int LoadConfig(char* src)
		{
			memset(buf, 0, sizeof(buf));
			fopen_s(&pFile, src, "r");
			if (pFile == NULL)
				return -1;


#if defined(_MSC_VER)
			auto descriptor = _fileno(pFile);
#else
			auto descriptor = fileno(fp);
#endif
			struct stat statBuf;
			if (fstat(descriptor, &statBuf) == -1) {
				fclose(pFile);
				return 0;
			}
			size_t num = statBuf.st_size;

			//file.seekg(0, ios::end);
			//int num = file.tellg();
			//file.seekg(0, ios::beg);
			//str.resize(num);

			size_t readsize = fread(buf, 1, num, pFile);
			//fclose(file);
			

			//fseek(pFile, 0, SEEK_SET);

			//fread(buf, 4096, 1, pFile);
			std::vector<std::string> sAstr;
			Helper::SplitString(buf, "[", sAstr);
			for (int i = 0; i < (int)sAstr.size(); i++)
			{
				if (sAstr[i].empty())
					continue;
				std::vector<std::string> temp;
				Helper::SplitString(sAstr[i], "]", temp);
				if (temp.size() != 2)
					continue;
				std::string key = temp[0];
				std::vector<std::string> sub;
				std::vector<std::string> subn;
				subn.push_back("\n");
				subn.push_back("\r");
				Helper::SplitString(temp[1], subn, sub);
				config[key] = sub;
			}

			fclose(pFile);
			return 0;
		}

		std::string GetStringValue(const std::string str, const std::string key)
		{
			std::string result;
			auto it = config.find(str);
			if (it == config.end())
				return result;

			std::vector<std::string>& temp = it->second;
			for (int i = 0; i < (int)temp.size(); i++)
			{
				int bFind = temp[i].find(key, 0);
				if (bFind == std::string::npos)
					continue;
				result.assign(temp[i].begin() + bFind + strlen(key.c_str()) + 1, temp[i].end());
			}
			return result;
		}
	private:
		FILE* pFile;
		char buf[4096];
		std::hash_map<std::string, std::vector<std::string>> config;
	};
}
