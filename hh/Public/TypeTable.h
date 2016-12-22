#pragma once

#include "Singleton.h"
#include <hash_map>
#include "Struct.h"
#include "Enum.h"
#include <unordered_map>
#include <string>

class DataBucket
{
public:
	bool PushData(std::string& name, void* data)
	{
		auto result = data_bucket.insert(std::make_pair(name, data));
		return result.second;
	}

	void* GetData(std::string&name)
	{
		void* p = nullptr;
		auto it = data_bucket.find(name);
		if (it != data_bucket.end())
			p = it->second;
		return p;
	}
private:
	std::unordered_map<std::string, void*> data_bucket;
};

class DBService;
class TypeTable : public Singleton<TypeTable>
{
public:
	TypeTable();
	~TypeTable();

	const TypeXinfa* getTypeXinfa(int32 id);

protected:
	void loadAllTable();
	void loadTypeXinfa();

	void FindAllTypeTable();

private:
	std::unordered_map<int32, TypeXinfa> _typeXinfa;
	std::unordered_map<int32, DataBucket> _typeDataBucket;
	DBService* m_pDBService;
};