#pragma once

#include "Application.h"
namespace TABLE{
class type_npc_talk
{
public:
	int32		npc_id;
	xstring		npc_name;
	xstring		content;
	xstring		talk_content;


//exparam

	xvector<int32>		talk_contents;
};

class type_npc_talkTable : public Singleton<type_npc_talkTable>
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
	~type_npc_talkTable()
	{
		this->Release();
	}
	bool Load()
	{
		std::string sql = "select * from type_npc_talk";
		StmtSyncResult result;
		sStmt.SyncExecuteQueryVariable(sql, nullptr, result);
		while (!result.pResult.eof())
		{
			type_npc_talk* temp = new type_npc_talk;
			temp->npc_id = result.pResult.GetInt32("npc_id");
			temp->npc_name = result.pResult.GetString("npc_name");
			temp->content = result.pResult.GetString("content");
			temp->talk_content = result.pResult.GetString("talk_content");
			{
				std::vector<xstring> ssplit;
				Helper::SplitString(temp->talk_content, ",", ssplit);
				for(size_t i = 0;i < ssplit.size();i++)
				{
					temp->talk_contents.push_back(Helper::StringToInt32(ssplit[i]));
				}
			}

			data_.push_back(temp);

			mappings_1[temp->npc_id] = temp;
			result.pResult.nextRow();
		}
		return result.pResult.GetResult();
	}
	////////////////////////////////////////////////////////////////////////////
	type_npc_talk* Get1(int32 npc_id)
	{
		auto pData = &mappings_1;
		auto it = pData->find(npc_id);
		if(it == pData->end())
			return nullptr;

		return it->second;
	}
	//////////////////////////////////////////////////////////////////////////
	type_npc_talk* At(int index)
	{
		return data_[index];
	}
	//////////////////////////////////////////////////////////////////////////
	int GetSize(void)
	{
		return data_.size();
	}
	//////////////////////////////////////////////////////////////////////////
	std::vector<type_npc_talk*> &GetData(void)
	{
		return data_;
	}
	//////////////////////////////////////////////////////////////////////////
	std::map<int32, type_npc_talk*> &GetMappings1(void)
	{
		return mappings_1;
	}
private:
	std::vector<type_npc_talk*>		 data_;
	std::map<int32, type_npc_talk*>		mappings_1;
};
}
TABLE::type_npc_talkTable* Singleton<TABLE::type_npc_talkTable>::single = nullptr;
#define s_type_npc_talkTable TABLE::type_npc_talkTable::getInstance()
