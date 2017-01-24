#pragma once
#define  OBJECT "Object"

enum obj_usage
{
	obj_usage_nothing,
	obj_usage_medicine,
	obj_usage_weapon,
};

struct Object
{
	virtual ~Object(){}
    int64 dbid;
    int32 id;
    int32 kind;
    int32 subkind;
	int32 usage;
    int32 num;
};

class Medicine : public Object
{
public:
	int32 type;
	int32 value;
};

class ObjectModule : public BaseModule
{
public:
	ObjectModule(std::string modulename, Player* pOwner);
	~ObjectModule();

    void AddObject(int32 id, int32 num);
    void DelObject(int64 dbid, int32 num);

    Object* GetObject(int64 dbid);

protected:
	void registmessage();
	void unregistmessage();

	void processRequestUse(PackPtr& pPack);

	void UseMedicine(Object* pObj, int32 target);

private:
	bool Init();

    std::unordered_map<int64, Object> _props;
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