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
	int great_num;			//�½���ϷС��ͨ����
	int video_award;
	int activity_award;
	int id;

	std::map<int,Explore> explores;
};

