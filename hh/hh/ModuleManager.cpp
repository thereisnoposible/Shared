#include "stdafx.h"
#include "ModuleManager.h"
#include "Player.h"

ModuleManager* Singleton<ModuleManager>::single = NULL;
ModuleManager::ModuleManager()
{

}
ModuleManager::~ModuleManager()
{
	for (int i = 0; i < (int)m_pFactory.size(); i++)
	{
		delete m_pFactory[i];
		m_pFactory[i] = nullptr;
	}
	m_pFactory.clear();
}

void ModuleManager::registerFactory(ModuleFactory* pFactory)
{
	m_pFactory.push_back(pFactory);
}

//--------------------------------------------------------------------------------
void ModuleManager::CreateRoleModule(Player* pOwner)
{
	assert(pOwner != NULL);

	for (int i=0; i<(int)m_pFactory.size(); i++)
	{
		BaseModule* pModule = m_pFactory[i]->CreateModule(pOwner);

		pOwner->AddModule(pModule);		//Ìí¼ÓÍæ¼Òmodule
	}

	//pOwner->Init();
}