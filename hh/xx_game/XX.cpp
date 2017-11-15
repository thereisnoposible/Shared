#include "pch.h"
#include "XX.h"

EXPORT_CLASS_BEGIN(XX_Env)
EXPORT_LUA_STRING(m_szName)
EXPORT_LUA_INT(m_iLevel)
EXPORT_LUA_FUNCTION(luaSay)
EXPORT_CLASS_END()

void XX_Env::Init(lua_State* L)
{
	//lua_register_class(L, this);
}

int XX_Env::luaSay(lua_State* L)
{
	// ...
	std::cout << "called\n";
	return 0;
}