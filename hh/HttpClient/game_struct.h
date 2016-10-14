#pragma once
#include <map>
struct Explore
{
	int id;
	int mid;
	int score;
	int times;
	int todayTimes;
	long long lastBattleTs;
	long long lastExploreTs;
};

struct Chapter
{
	int cid;
	int pres;
	int af;
	int grade_award;
	int great_num;			//章节游戏小节通关数
	int video_award;
	int activity_award;
	int id;

	std::map<int,Explore> explores;
};

