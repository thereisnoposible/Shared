#include "stdafx.h"
#include "BaseModule.h"
#include "ModuleManager.h"
#include "Player.h"

ModuleFactory::ModuleFactory(const std::string& name) : m_ModuleName(name)
{
	//ModuleManager::getInstance().registerFactory(this);
}

ModuleFactory::~ModuleFactory(void)
{

}