#include "stdafx.h"
#include "BaseModule.h"
#include "ModuleManager.h"
#include "Player.h"

void BaseModule::setInitOk()
{
	m_bInitOk = true;
	//m_pOwner->OnModuleInitOk();
}

ModuleFactory::ModuleFactory(const std::string& name) : m_ModuleName(name)
{
	ModuleManager::getInstance().registerFactory(this);
}

ModuleFactory::~ModuleFactory(void)
{

}