#pragma once

inline bool lua_get_global_function(lua_State* L, const char function[])
{
	lua_getglobal(L, function);
	return lua_isfunction(L, -1);
}
