#include "LuaEngine.h"
#include <assert.h>
#include <string>
extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include "lua/tolua++.h"
}
bool Eluna::initialized = false;

extern void RegisterFunctions(Eluna* E);

Eluna::Eluna()
{
	assert(IsInitialized());

	OpenLua();

	// Replace this with map insert if making multithread version
	//

	// Set event manager. Must be after setting sEluna
	// on multithread have a map of state pointers and here insert this pointer to the map and then save a pointer of that pointer to the EventMgr
	//eventMgr = new EventMgr(&Eluna::GEluna);
}
Eluna::~Eluna()
{

}

void Eluna::Initialize()
{
	assert(!IsInitialized());

	LoadScriptPaths();

	// Must be before creating GEluna
	// This is checked on Eluna creation
	initialized = true;

	// Create global eluna
	GEluna = new Eluna();
}

void Eluna::Uninitialize()
{
	assert(IsInitialized());

	delete GEluna;
	GEluna = nullptr;

	initialized = false;
}

void Eluna::LoadScriptPaths()
{

}

void Eluna::OpenLua()
{
	L = luaL_newstate();

	// open base lua libraries
	luaL_openlibs(L);

	// open additional lua libraries

	// Register methods and functions
	RegisterFunctions(this);

	// Create hidden table with weak values
	lua_newtable(L);
	lua_newtable(L);
	lua_pushstring(L, "v");
	lua_setfield(L, -2, "__mode");
	lua_setmetatable(L, -2);
	lua_setglobal(L, "Eluna Object Store");

	// Set lua require folder paths (scripts folder structure)
	lua_getglobal(L, "package");
	std::string lua_requirepath;
	lua_pushstring(L, lua_requirepath.c_str());
	lua_setfield(L, -2, "path");
	lua_pushstring(L, ""); // erase cpath
	lua_setfield(L, -2, "cpath");
	lua_pop(L, 1);
}

void RegisterFunctions(Eluna* E)
{

}