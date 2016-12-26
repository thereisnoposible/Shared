#pragma once

#include "BaseModule.h"
#include "Initialer.h"
class ModuleManager : public Singleton<ModuleManager>, Initialer
{
public:
	ModuleManager();
	~ModuleManager();

	void registerFactory(ModuleFactory* pFactory);
	void CreateRoleModule(Player* pOwner);
private:
	std::vector<ModuleFactory*> m_pFactory;
};