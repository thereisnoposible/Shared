#pragma once
#include "XX.h"

class LuaEngine
{
public:
	LuaEngine();
	bool Init();
	void Destroy();
	void Reload();

protected:
	void registmessage();
	void FireMessage(PackPtr& pPack);

private:
	
	lua_State* lua_state_;

	XX_Env* m_XX;
};