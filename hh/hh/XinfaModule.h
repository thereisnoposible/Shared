#pragma once
#include "BaseModule.h"

class XinfaModule : public BaseModule
{
	enum CallBackType
	{
		CallBackTypeXINFA = 0,
	};
public:
	XinfaModule(const std::string& name, Player* pOwner);
	~XinfaModule();

public:
	/**
	* @brief		重载虚函数
	*/
	bool Init();

	/**
	* @brief		注册注销消息
	*/
	void registerMessage();
	void unregisterMessage();

	const Xinfa* getXinfa(std::function<void()> callback);
protected:
	void Dazuo();
	void Tiaoxi();
protected:
	void processRequestXinfa(PackPtr& pPack);
	void processRequestDazuo(PackPtr& pPack);
	void processRequestTiaoxi(PackPtr& pPack);

	void requestXinfaResponse(PackPtr& pPack);

private:
	Xinfa* m_Xinfa;

	std::unordered_map<int32, std::vector<std::function<void()>>> mv_callback;

	TMHANDLE tm_Handle;
};

class XinfaFactory : public ModuleFactory
{
public:
	XinfaFactory(void) : ModuleFactory(XINFAMODULE)
	{

	}

	virtual ~XinfaFactory(void)
	{

	}

	BaseModule* CreateModule(Player* pOwner)
	{
		BaseModule* pModule = new XinfaModule(GetModuleName(), pOwner);
		CHECKERRORANDRETURNVALUE(pModule != NULL, NULL);

		return pModule;
	}
};
