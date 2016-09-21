#pragma once
#define  TONGHE "Tonghe"
class TongheModule : public BaseModule
{
public:
	TongheModule(std::string modulename, Player* pOwner);
	~TongheModule();

protected:
	void registmessage();
	void unregistmessage();

	void xuexi(PackPtr& pPack);

	void processMove(PackPtr& pPack);



private:
	bool Init();
};

class TongheFactory : public ModuleFactory
{
public:
	TongheFactory(void) : ModuleFactory(TONGHE)
	{

	}

	virtual ~TongheFactory(void)
	{

	}

	BaseModule* CreateModule(Player* pOwner)
	{
		BaseModule* pModule = new TongheModule(GetModuleName(), pOwner);

		return pModule;
	}
};