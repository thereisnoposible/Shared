#pragma once

#include <boost/any.hpp>
class Action
{
public:
	int32		action_id;
	std::string		action_content;
	int32		action_type;				//1.交谈 2物品 3 任务 4 行为
	boost::any		type_content;
	//string type_1;					//交谈内容
	//vector<shangren_prop> type_2; 	//商人物品
	//vector<int32> type_3; 			//任务ID
	//vector<xuanxiang> type_4;		//行为ID Action

	int32		server_action;				//1取消任务 2传送
	boost::any		server_action_param;
	//std::string server_action_param_1;	//任务类型
	//std::string server_action_param_2;	//传送坐标
};

class NPC
{
public:
	int32		npc_id;
	std::string		npc_name;
	std::string		content;
	std::string		talk_content;
	//exparam

	xvector<int32>		talk_contents;
};

class NPCManager : public Singleton<NPCManager>
{
public:
	NPCManager();

protected:
	void load_type_npc();
	void load_type_action();
	void init_action_type_content(int32 action_type, boost::any& dest, const std::string& src);
	void init_action_server_action_param(int32 server_action, boost::any& dest, const std::string& src);
private:
	std::unordered_map<int32, NPC> m_npcs;
	std::unordered_map<int32, Action> m_actions;
};