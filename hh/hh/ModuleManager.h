#pragma once

#include "BaseModule.h"
#include "Initialer.h"
#include "singleton/Singleton.h"

class ModuleManager : public Singleton<ModuleManager>, Initialer
{
public:
	ModuleManager();
	~ModuleManager();
	
	void CreateRoleModule(Player* pOwner);
private:
	void registerFactory(ModuleFactory* pFactory);
	std::vector<ModuleFactory*> m_pFactory;
};