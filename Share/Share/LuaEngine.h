#pragma once

struct lua_State;
class Eluna
{
private:
	static bool initialized;
	static Eluna* GEluna;
	Eluna();
	~Eluna();

	void OpenLua();

	static void Initialize();
	static void Uninitialize();

	static bool IsInitialized(){ return initialized; }

	static void LoadScriptPaths();

	lua_State* L;
};