#pragma once

struct objectItem
{
	int64				dbid;
	int32				playerid;
	int32				prop_id;
	int32				prop_num;


	void fromPBMessage(const pm_object &proto_item)
	{
		dbid = proto_item.dbid();
		playerid = proto_item.playerid();
		prop_id = proto_item.prop_id();
		prop_num = proto_item.prop_num();
	}
	void toPBMessage(pm_object &proto_item) const
	{
		proto_item.set_dbid(dbid); 
		proto_item.set_playerid(playerid); 
		proto_item.set_prop_id(prop_id); 
		proto_item.set_prop_num(prop_num); 
	}
};
