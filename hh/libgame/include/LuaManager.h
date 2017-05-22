#pragma once

typedef int (InitLuaFun)(lua_State*);

class XClass LuaManager : public Singleton<LuaManager>
{
public:
	LuaManager(InitLuaFun* pFun);
	~LuaManager(void);
	
public:
	bool CallLuaFunction(const xstring& func, const char* format,...);

	bool LoadLuaFile(const xstring& luafile);

	bool LoadLuaPath(const xstring& luapath);

	bool GetInt(const xstring& field, int *ret);

	bool GetFloat(const xstring& field, float *ret);

	bool GetString(const xstring& field, const char **ret);

private:
	lua_State* m_pLua;				/** lua接口对象指针 */
};

