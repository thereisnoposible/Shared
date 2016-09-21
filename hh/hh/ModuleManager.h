#pragma once

#include "BaseModule.h"
class ModuleManager : public Singleton<ModuleManager> 
{
public:
	ModuleManager();
	~ModuleManager();

	void registerFactory(ModuleFactory* pFactory);
	void CreateRoleModule(Player* pOwner);
private:
	std::vector<ModuleFactory*> m_pFactory;
};