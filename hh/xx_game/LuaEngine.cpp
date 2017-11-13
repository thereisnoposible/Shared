#include "pch.h"
#include "LuaEngine.h"
#include "Application.h"
#include "lua_helper.h"

LuaEngine::LuaEngine() :lua_state_(nullptr)
{

}

bool LuaEngine::Init()
{
	if (lua_state_ != nullptr)
		return false;

	lua_state_ = luaL_newstate();  /* create state */
	if (lua_state_ == NULL) {
		return false;
	}

	luaL_openlibs(lua_state_);

	if (luaL_dofile(lua_state_, "scripts/hellolua.lua"))
		std::cout << luaL_checkstring(lua_state_, -1);

	lua_get_global_function(lua_state_, "aaaaa");
	lua_pushinteger(lua_state_, 1000);
	int32 sum = (int)lua_tonumber(lua_state_, -1);
	lua_pcall(lua_state_, 1, 1, 0);
	sum = (int)lua_tonumber(lua_state_, -1);
	lua_pop(lua_state_, 1);

	registmessage();
	return true;
}

void LuaEngine::Destroy()
{
	if (lua_state_ == nullptr)
		return;

	lua_close(lua_state_);
	lua_state_ = nullptr;
}

void LuaEngine::Reload()
{
	if (luaL_dofile(lua_state_, "scripts/hellolua.lua"))
		std::cout << luaL_checkstring(lua_state_, -1);

	lua_get_global_function(lua_state_, "aaaaa");
	lua_pushinteger(lua_state_, 8484);
	int32 sum = (int)lua_tonumber(lua_state_, -1);
	lua_pcall(lua_state_, 1, 1, 0);
	sum = (int)lua_tonumber(lua_state_, -1);
	lua_pop(lua_state_, 1);
}

void LuaEngine::registmessage()
{
	sApp.GetNetService().RegisterMessageRange(100, 10000, boost::bind(&LuaEngine::FireMessage, this, _1));
}

void LuaEngine::FireMessage(PackPtr& pPack)
{
	if (lua_state_ == nullptr)
		return;

	lua_pushinteger(lua_state_, pPack->getMessageId());
	lua_pushlstring(lua_state_, pPack->getBuffer(), pPack->getBufferSize());
	lua_pushstring(lua_state_, "func");
	lua_pcall(lua_state_, 2, 0, 0);
}