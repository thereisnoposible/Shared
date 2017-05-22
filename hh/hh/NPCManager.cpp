#include "stdafx.h"
#include "NPCManager.h"


NPCManager* Singleton<NPCManager>::single = NULL;
NPCManager::NPCManager()
{

}

void NPCManager::load_type_npc()
{
	std::string sql = "select * from type_npc_talk";
	StmtSyncResult result;
	sStmt.SyncExecuteQueryVariable(sql, nullptr, result);
	while (!result.pResult.eof())
	{
		NPC temp;
		temp.npc_id = result.pResult.GetInt32("npc_id");
		temp.npc_name = result.pResult.GetString("npc_name");
		temp.content = result.pResult.GetString("content");
		temp.talk_content = result.pResult.GetString("talk_content");
		{
			std::vector<std::string> ssplit;
			Helper::SplitString(temp.talk_content, ",", ssplit);
			for (size_t i = 0; i < ssplit.size(); i++)
			{
				temp.talk_contents.push_back(Helper::StringToInt32(ssplit[i]));
			}
		}
		m_npcs[temp.npc_id] = temp;
		result.pResult.nextRow();
	}
}

void NPCManager::load_type_action()
{
	std::string sql = "select * from type_npc_action";
	StmtSyncResult result;
	sStmt.SyncExecuteQueryVariable(sql, nullptr, result);
	while (!result.pResult.eof())
	{
		Action temp;
		temp.action_id = result.pResult.GetInt32("action_id");
		temp.action_content = result.pResult.GetString("action_content");
		temp.action_type = result.pResult.GetInt32("action_type");
		init_action_type_content(temp.action_type, temp.type_content, result.pResult.GetString("type_content"));

		temp.server_action = result.pResult.GetInt32("server_action");
		init_action_server_action_param(temp.server_action, temp.server_action_param, result.pResult.GetString("server_action_param"));

		m_actions[temp.action_id] = temp;

		result.pResult.nextRow();
	}
}

void NPCManager::init_action_type_content(int32 action_type, boost::any& dest, const std::string& src)
{
	switch (action_type)
	{
	case 1://1.交谈
		dest = src;
		break;
	case 2://2.物品
		dest = Helper::StringToInt32(src);
		break;
	case 3://3.任务
	case 4://4.行为
	{
		xvector<std::string> temp;
		xvector<int32> item;
		Helper::SplitString(src, ",", temp);
		for (size_t i = 0; i < temp.size(); i++)
		{
			item.push_back(Helper::StringToInt32(temp[i]));
		}

		dest = item;
	}
		break;
	default:
		throw("unknown action type content");
		break;
	}
}

void NPCManager::init_action_server_action_param(int32 server_action, boost::any& dest, const std::string& src)
{
	switch (server_action)
	{
	case 1:	//1.取消任务
		dest = src;
		break;
	case 2:	//2.传送
		dest = src;
		break;
	default:
		throw("unknown action type content");
		break;
	}
}