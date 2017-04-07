#pragma once

#include "Application.h"
namespace TABLE{
class type_npc_action
{
public:
	int32		action_id;
	xstring		action_content;
	int32		action_type;
	xstring		type_content;
	int32		server_action;
	xstring		server_action_param;


//exparam

};

class type_npc_actionTable : public Singleton<type_npc_actionTable>
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
	~type_npc_actionTable()
	{
		this->Release();
	}
	bool Load()
	{
		std::string sql = "select * from type_npc_action";
		StmtSyncResult result;
		sStmt.SyncExecuteQueryVariable(sql, nullptr, result);
		while (!result.pResult.eof())
		{
			type_npc_action* temp = new type_npc_action;
			temp->action_id = result.pResult.GetInt32("action_id");
			temp->action_content = result.pResult.GetString("action_content");
			temp->action_type = result.pResult.GetInt32("action_type");
			temp->type_content = result.pResult.GetString("type_content");
			temp->server_action = result.pResult.GetInt32("server_action");
			temp->server_action_param = result.pResult.GetString("server_action_param");
			data_.push_back(temp);

			mappings_1[temp->action_id] = temp;
			result.pResult.nextRow();
		}
		return result.pResult.GetResult();
	}
	////////////////////////////////////////////////////////////////////////////
	type_npc_action* Get1(int32 action_id)
	{
		auto pData = &mappings_1;
		auto it = pData->find(action_id);
		if(it == pData->end())
			return nullptr;

		return it->second;
	}
	//////////////////////////////////////////////////////////////////////////
	type_npc_action* At(int index)
	{
		return data_[index];
	}
	//////////////////////////////////////////////////////////////////////////
	int GetSize(void)
	{
		return data_.size();
	}
	//////////////////////////////////////////////////////////////////////////
	std::vector<type_npc_action*> &GetData(void)
	{
		return data_;
	}
	//////////////////////////////////////////////////////////////////////////
	std::map<int32, type_npc_action*> &GetMappings1(void)
	{
		return mappings_1;
	}
private:
	std::vector<type_npc_action*>		 data_;
	std::map<int32, type_npc_action*>		mappings_1;
};
}
TABLE::type_npc_actionTable* Singleton<TABLE::type_npc_actionTable>::single = nullptr;
#define s_type_npc_actionTable TABLE::type_npc_actionTable::getInstance()
