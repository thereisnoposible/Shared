#pragma once

#include "Application.h"
namespace TABLE{
class type_sell_props
{
public:
	int32		npc_id;
	int32		id;
	std::string		name;
	int32		price;
	int32		num;


//exparam

};

class type_sell_propsTable : public Singleton<type_sell_propsTable>
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
	~type_sell_propsTable()
	{
		this->Release();
	}
	bool Load()
	{
		std::string sql = "select * from type_sell_props";
		StmtSyncResult result;
		sStmt.SyncExecuteQueryVariable(sql, nullptr, result);
		while (!result.pResult.eof())
		{
			type_sell_props* temp = new type_sell_props;
			temp->npc_id = result.pResult.GetInt32("npc_id");
			temp->id = result.pResult.GetInt32("id");
			temp->name = result.pResult.GetString("name");
			temp->price = result.pResult.GetInt32("price");
			temp->num = result.pResult.GetInt32("num");
			data_.push_back(temp);

			auto map_tempit11 = mappings_1.find(temp->npc_id);
			if (map_tempit11 == mappings_1.end())
			{
				auto temm = mappings_1[temp->npc_id];
				map_tempit11 = mappings_1.find(temp->npc_id);
			}
			map_tempit11->second[temp->id] = temp;

			result.pResult.nextRow();
		}
		return result.pResult.GetResult();
	}
	////////////////////////////////////////////////////////////////////////////
	std::map<int32, type_sell_props*>* Get1(int32 npc_id)
	{
		auto pData = &mappings_1;
		auto it = pData->find(npc_id);
		if(it == pData->end())
			return nullptr;

		return &it->second;
	}
	////////////////////////////////////////////////////////////////////////////
	type_sell_props* Get1(int32 npc_id,int32 id)
	{
		auto pData = Get1(npc_id);
		if(pData == nullptr)
			 return nullptr;

		auto it = pData->find(id);
		if(it == pData->end())
			return nullptr;

		return it->second;
	}
	//////////////////////////////////////////////////////////////////////////
	type_sell_props* At(int index)
	{
		return data_[index];
	}
	//////////////////////////////////////////////////////////////////////////
	int GetSize(void)
	{
		return data_.size();
	}
	//////////////////////////////////////////////////////////////////////////
	std::vector<type_sell_props*> &GetData(void)
	{
		return data_;
	}
	//////////////////////////////////////////////////////////////////////////
	std::map<int32, std::map<int32, type_sell_props*>> &GetMappings1(void)
	{
		return mappings_1;
	}
private:
	std::vector<type_sell_props*>		 data_;
	std::map<int32, std::map<int32, type_sell_props*>>		mappings_1;
};
}
TABLE::type_sell_propsTable* Singleton<TABLE::type_sell_propsTable>::single = nullptr;
#define s_type_sell_propsTable TABLE::type_sell_propsTable::getInstance()
