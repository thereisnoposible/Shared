#pragma once

struct playerItem
{
	int32				id;
	int32				jinbi;
	xstring				name;
	xstring				account;


	void fromPBMessage(const pm_playerdata &proto_item)
	{
		id = proto_item.id();
		jinbi = proto_item.jinbi();
		name = proto_item.name();
		account = proto_item.account();
	}
	void toPBMessage(pm_playerdata &proto_item) const
	{
		proto_item.set_id(id); 
		proto_item.set_jinbi(jinbi); 
		proto_item.set_name(name); 
		proto_item.set_account(account); 
	}
};
