#pragma once

#include "Application.h"
namespace TABLE{
class type_prop
{
public:
	int32		id;
	xstring		name;
	xstring		desc;
	int32		type;
	int32		sub_type;
	int32		append;


//exparam

};

class type_propTable : public Singleton<type_propTable>
{
public:
	////////////////////////////////////////////////////////////////////////////
	void Release()
	{
		for(size_t i = 0; i < data_.size(); i++)
		{
			delete data_[i];
		}

		data_.clear();
		mappings_1.clear();
	}
	////////////////////////////////////////////////////////////////////////////
	~type_propTable()
	{
		this->Release();
	}
	bool Load()
	{
		std::string sql = "select * from type_prop";
		StmtSyncResult result;
		sStmt.SyncExecuteQueryVariable(sql, nullptr, result);
		while (!result.pResult.eof())
		{
			type_prop* temp = new type_prop;
			temp->id = result.pResult.GetInt32("id");
			temp->name = result.pResult.GetString("name");
			temp->desc = result.pResult.GetString("desc");
			temp->type = result.pResult.GetInt32("type");
			temp->sub_type = result.pResult.GetInt32("sub_type");
			temp->append = result.pResult.GetInt32("append");
			data_.push_back(temp);

			mappings_1[temp->id] = temp;
			result.pResult.nextRow();
		}
		return result.pResult.GetResult();
	}
	////////////////////////////////////////////////////////////////////////////
	type_prop* Get1(int32 id)
	{
		auto pData = &mappings_1;
		auto it = pData->find(id);
		if(it == pData->end())
			return nullptr;

		return it->second;
	}
	//////////////////////////////////////////////////////////////////////////
	type_prop* At(int index)
	{
		return data_[index];
	}
	//////////////////////////////////////////////////////////////////////////
	int GetSize(void)
	{
		return data_.size();
	}
	//////////////////////////////////////////////////////////////////////////
	std::vector<type_prop*> &GetData(void)
	{
		return data_;
	}
	//////////////////////////////////////////////////////////////////////////
	std::map<int32, type_prop*> &GetMappings1(void)
	{
		return mappings_1;
	}
private:
	std::vector<type_prop*>		 data_;
	std::map<int32, type_prop*>		mappings_1;
};
}
TABLE::type_propTable* Singleton<TABLE::type_propTable>::single = nullptr;
#define s_type_propTable TABLE::type_propTable::getInstance()
