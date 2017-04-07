#pragma once
#define  OBJECT "Object"

#include "../new/proto/protobuf/object.pb.h"
#include "entity/object_entity.h"

class ObjectModule : public BaseModule
{
public:
	ObjectModule(std::string modulename, Player* pOwner);
	~ObjectModule();

	bool Init();
	void OnDataResponse(pm_object_data_response_db& response);

	std::unordered_map<int64, objectItem*>& GetMaps(){ return _props; }

	void AddObject(int32 id, int32 num, int32 cause);
	void DestroyObject(int64 dbid, int32 num, int32 cause);
	int32 GetCount(int64 dbid);
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