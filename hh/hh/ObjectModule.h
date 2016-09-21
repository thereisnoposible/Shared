#pragma once
#define  OBJECT "Object"
class ObjectModule : public BaseModule
{
public:
	ObjectModule(std::string modulename, Player* pOwner);
	~ObjectModule();

	void addPropItem(PropItem& item);

protected:
	void registmessage();
	void unregistmessage();

	void processUse(PackPtr& pPack);

private:
	bool Init();

	std::hash_map<int, std::vector<PropItem>> _props;		//-1表示穿着装备层
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