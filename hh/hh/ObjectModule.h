#pragma once
#define  OBJECT "Object"

#include "../new/proto/protobuf/object.pb.h"
#include "entity/object_entity.h"

struct ModuleInit
{
	ModuleInit(const std::string& n)
	{
		name = n;
	}

	std::string name;
};

class ObjectModule : public BaseModule
{
public:
	ObjectModule(std::string modulename, Player* pOwner);
	~ObjectModule();

	void InitFunc();
	void registmessage();

	void OnDataResponse(pm_object_data_response_db& response);
private:
	void Insert(objectItem& temp);
	void Update(objectItem& temp);
	void Delete(objectItem& temp);
	std::unordered_map<int64, objectItem*> _props;
	std::unordered_map<int32, objectItem*> _append_props;
};

class ObjectFactory : public ModuleFactory
{
public:
	ObjectFactory(void) : ModuleFactory(OBJECT)
	{

	}

	virtual ~ObjectFactory(void)
	{

	}

	BaseModule* CreateModule(Player* pOwner)
	{
		BaseModule* pModule = new ObjectModule(GetModuleName(), pOwner);

		return pModule;
	}
};