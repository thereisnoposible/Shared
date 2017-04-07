#pragma once

#include "Application.h"
namespace TABLE{
class npc_talk
{
public:
	int32		action_id;
	xstring		npc_name;
	xstring		content;
	xstring		talk_content;
	int64		updatetime;


//exparam

	xvector<idandnum>		talk_contents;
	idandnum		talk_contents;
};

class npc_talkTable : public Singleton<npc_talkTable>
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
		mappings_2.clear();
	}
	////////////////////////////////////////////////////////////////////////////
	~npc_talkTable()
	{
		this->Release();
	}
	bool Load()
	{
		std::string sql = "select * from npc_talk";
		StmtSyncResult result;
		sStmt.SyncExecuteQueryVariable(sql, nullptr, result);
		while (!result.pResult.eof())
		{
			npc_talk* temp = new npc_talk;
			temp->action_id = result.pResult.GetInt32("action_id");
			temp->npc_name = result.pResult.GetString("npc_name");
			temp->content = result.pResult.GetString("content");
			temp->talk_content = result.pResult.GetString("talk_content");
			temp->updatetime = result.pResult.GetDateTime("updatetime");
			{
				std::vector<xstring> ssplit;
				Helper::SplitString(temp->talk_content, ";", ssplit);
				for(size_t i = 0;i < ssplit.size();i++)
				{
					std::vector<string> sub_ssplit;
					Helper::SplitString(ssplit[i], ",", sub_ssplit);
					for(size_t j = 0;j < sub_ssplit.size();j++)
					{
						idandnum sub_temp;
						sub_temp.id.push_back(Helper::StringToInt32(sub_ssplit[j]));
						sub_temp.num.push_back(Helper::StringToInt32(sub_ssplit[j]));
						temp->talk_contents.push_back(sub_temp);
					}
				}
			}

			{
				std::vector<xstring> ssplit;
				Helper::SplitString(temp->talk_content, ";", ssplit);
				for(size_t i = 0;i < ssplit.size();i++)
				{
					temp->talk_contents.id.push_back(Helper::StringToInt32(ssplit[i]));
					temp->talk_contents.num.push_back(Helper::StringToInt32(ssplit[i]));
				}
			}

			data_.push_back(temp);

			auto map_tempit12 = mappings_1.find(temp->action_id);
			if (map_tempit12 == mappings_1.end())
			{
				auto temm = mappings_1[temp->action_id];
				map_tempit12 = mappings_1.find(temp->action_id);
			}
			auto map_tempit11 = map_tempit12->second.find(temp->npc_name);
			if (map_tempit11 == map_tempit12->second.end())
			{
				auto temm = map_tempit12->second[temp->npc_name];
				map_tempit11 = map_tempit12->second.find(temp->npc_name);
			}
			map_tempit11->second[temp->talk_content] = temp;

			mappings_2[temp->content] = temp;
			result.pResult.nextRow();
		}
		return result.pResult.GetResult();
	}
	////////////////////////////////////////////////////////////////////////////
	std::map<xstring, std::map<xstring, npc_talk*>>* Get1(int32 action_id)
	{
		auto pData = &mappings_1;
		auto it = pData->find(action_id);
		if(it == pData->end())
			return nullptr;

		return &it->second;
	}
	////////////////////////////////////////////////////////////////////////////
	std::map<xstring, npc_talk*>* Get1(int32 action_id,const xstring& npc_name)
	{
		auto pData = Get1(action_id);
		if(pData == nullptr)
			 return nullptr;

		auto it = pData->find(npc_name);
		if(it == pData->end())
			return nullptr;

		return &it->second;
	}
	////////////////////////////////////////////////////////////////////////////
	npc_talk* Get1(int32 action_id,const xstring& npc_name,const xstring& talk_content)
	{
		auto pData = Get1(action_id, npc_name);
		if(pData == nullptr)
			 return nullptr;

		auto it = pData->find(talk_content);
		if(it == pData->end())
			return nullptr;

		return it->second;
	}
	////////////////////////////////////////////////////////////////////////////
	npc_talk* Get2(const xstring& content)
	{
		auto pData = &mappings_2;
		auto it = pData->find(content);
		if(it == pData->end())
			return nullptr;

		return it->second;
	}
	//////////////////////////////////////////////////////////////////////////
	npc_talk* At(int index)
	{
		return data_[index];
	}
	//////////////////////////////////////////////////////////////////////////
	int GetSize(void)
	{
		return data_.size();
	}
	//////////////////////////////////////////////////////////////////////////
	std::vector<npc_talk*> &GetData(void)
	{
		return data_;
	}
	//////////////////////////////////////////////////////////////////////////
	std::map<int32, std::map<xstring, std::map<xstring, npc_talk*>>> &GetMappings1(void)
	{
		return mappings_1;
	}
	//////////////////////////////////////////////////////////////////////////
	std::map<xstring, npc_talk*> &GetMappings2(void)
	{
		return mappings_2;
	}
private:
	std::vector<npc_talk*>		 data_;
	std::map<int32, std::map<xstring, std::map<xstring, npc_talk*>>>		mappings_1;
	std::map<xstring, npc_talk*>		mappings_2;
};
}
TABLE::npc_talkTable* Singleton<TABLE::npc_talkTable>::single = nullptr;
#define s_npc_talkTable TABLE::npc_talkTable::getInstance()
