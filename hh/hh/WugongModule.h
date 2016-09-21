#pragma once
#define WUGONG "Wugong"
class WugongModule : public BaseModule
{
public:
	WugongModule(std::string modulename, Player* pOwner);
	~WugongModule();

	bool Learning(Wugong& wugong);

	int GetWugongLevel(int wugongid);

	void AddWugongLevel(int wugongid);

protected:
	void registmessage();
	void unregistmessage();

	bool Init();	

private:
	std::hash_map<int, Wugong> _wugongMap;;
};

class WugongFactory : public ModuleFactory
{
public:
	WugongFactory(void) : ModuleFactory(WUGONG)
	{

	}

	virtual ~WugongFactory(void)
	{

	}	

	BaseModule* CreateModule(Player* pOwner)
	{
		BaseModule* pModule = new WugongModule(GetModuleName(), pOwner);

		return pModule;
	}
};