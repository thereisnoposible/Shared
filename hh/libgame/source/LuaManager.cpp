#include "pch.h"
#include "LuaManager.h"
#include "helper.h"


template<> LuaManager* Singleton<LuaManager>::s_pInstance = NULL;

//---------------------------------------------------------------------------------
LuaManager::LuaManager(InitLuaFun* pFun)
{
	assert(pFun);

#ifdef LUA_VERSION_NUM /* lua 5.1 */
	m_pLua = luaL_newstate();
	luaL_openlibs(m_pLua);

#else
	m_pLua = lua_open();
	luaopen_base(m_pLua);
	luaopen_io(m_pLua);
	luaopen_string(m_pLua);
	luaopen_table(m_pLua);
	luaopen_math(m_pLua);
	luaopen_debug(m_pLua);
#endif

	lua_pushstring(m_pLua,TOLUA_VERSION);
	lua_setglobal(m_pLua,"TOLUA_VERSION");

	lua_pushstring(m_pLua,LUA_VERSION);
	lua_setglobal(m_pLua,"TOLUA_LUA_VERSION");


	(pFun)(m_pLua);
}


//---------------------------------------------------------------------------------
LuaManager::~LuaManager(void)
{
	lua_close(m_pLua);
}


//---------------------------------------------------------------------------------
bool LuaManager::CallLuaFunction(const xstring& func, const char* format,...)
{
	if(NULL == format) return false;

	va_list vl;

	xstring csFormat = format;
	xvector<xstring> saTmp;
	Helper::SplitString(csFormat,":",saTmp);

	int32 incount = 0;
	int32 outcount = 0;
	xvector<xstring> inlist;
	xvector<xstring> outlist;
	if(saTmp.size()!=0)
	{
		xstring sInParam = saTmp[0];
		Helper::SplitString(sInParam,"%",inlist);
		incount = inlist.size();

		if(saTmp.size()>=2)
		{
			xstring sOutParam = saTmp[1];

			Helper::SplitString(sOutParam,"%",outlist);
			outcount = outlist.size();
		}
	}

	int top = lua_gettop(m_pLua);

	try
	{
		va_start(vl,format);

		lua_getglobal(m_pLua, func.c_str());
		if(!lua_isfunction(m_pLua,-1))
		{//不存在该函数,清理栈
			lua_settop(m_pLua,top);
			LogService::GetSingleton().LogMessage("Lua can't find function : %s",func.c_str());
			va_end(vl);
			return false;
		}

		for(int32 i=0; i<incount; i++)
		{//传入参数
			if(inlist[i] == "s")
			{
				lua_pushstring(m_pLua,va_arg(vl,char*));
			}
			else if(inlist[i] == "d")
			{
				lua_pushnumber(m_pLua,va_arg(vl,int32));
			}
			else if(inlist[i] == "f")
			{
				lua_pushnumber(m_pLua,va_arg(vl,float));
			}
			else if(inlist[i] == "l")
			{
				lua_pushnumber(m_pLua,lua_Number(va_arg(vl,int64)));
			}
			else
			{//自定义数据类型
				tolua_pushusertype(m_pLua,va_arg(vl,void*),inlist[i].c_str());
			}

			lua_checkstack(m_pLua,1);
		}

		if(lua_pcall(m_pLua,incount,outcount,0) !=0 )
		{
			xstring errMsg = lua_tostring(m_pLua,-1);
			LogService::GetSingleton().LogMessage("Lua Call function %s error : %s",func.c_str(),errMsg.c_str());
			lua_settop(m_pLua,top);
			va_end(vl);

			return false;
		}

		//处理输出参数
		int32 outstack = -outcount;
		for(int32 i=0; i<outcount; i++)
		{
			if(outlist[i] == "s")
			{
				if (!lua_isstring(m_pLua, outstack))
				{
					lua_settop(m_pLua, top);
					va_end(vl);
					LogService::GetSingleton().LogMessage("Lua Call Function %s,can't find param",func.c_str());
					return false;
				}

				*va_arg(vl, const char **) = lua_tostring(m_pLua, outstack);
			}
			else if(outlist[i] == "d")
			{
				if (!lua_isnumber(m_pLua, outstack))
				{
					lua_settop(m_pLua, top);
					va_end(vl);
					LogService::GetSingleton().LogMessage("Lua Call Function %s,can't find param",func.c_str());
					return false;
				}

				*va_arg(vl, int32*) = (int32)lua_tonumber(m_pLua, outstack);
			}
			else if(outlist[i] == "f")
			{
				if (!lua_isnumber(m_pLua, outstack))
				{
					lua_settop(m_pLua, top);
					va_end(vl);
					LogService::GetSingleton().LogMessage("Lua Call Function %s,can't find param",func.c_str());
					return false;
				}

				*va_arg(vl, float *) = (float)lua_tonumber(m_pLua, outstack);
			}
			else if(outlist[i] == "l")
			{
				if (!lua_isnumber(m_pLua, outstack))
				{
					lua_settop(m_pLua, top);
					va_end(vl);
					LogService::GetSingleton().LogMessage("Lua Call Function %s,can't find param",func.c_str());
					return false;
				}

				*va_arg(vl, int64 *) = (int64)lua_tonumber(m_pLua, outstack);
			}
			else
			{//自定义数据类型
				tolua_Error tolua_err;
				if (!tolua_isusertype(m_pLua,outstack,outlist[i].c_str(),0,&tolua_err))
				{
					lua_settop(m_pLua, top);
					va_end(vl);
					LogService::GetSingleton().LogMessage("Lua Call Function %s,can't find param",func.c_str());
					return false;
				}
				*va_arg(vl, const void **)= tolua_tousertype(m_pLua,outstack,0);
			}

			++outstack;
		}

		va_end(vl);
		lua_settop(m_pLua, top);
		return true;
	}
	catch (...)
	{
		va_end(vl);
		LogService::GetSingleton().LogMessage("Lua Call Function %s : unknown error",func.c_str());
		return false;
	}

	return true;
}


//---------------------------------------------------------------------------------
bool LuaManager::LoadLuaFile(const xstring& luafile)
{
	int top=lua_gettop(m_pLua);

	if (luaL_loadfile(m_pLua, luafile.c_str())!=0||lua_pcall(m_pLua, 0, 0, 0)!=0 )
	{
		std::string errMsg = lua_tostring(m_pLua,-1);
		lua_settop(m_pLua,top);

		LogService::GetSingleton().LogMessage("LoadLuaFile error,file %s,%s",luafile.c_str(),errMsg.c_str());
		return false;
	}

	return true;
}


//---------------------------------------------------------------------------------
bool LuaManager::LoadLuaPath(const xstring& luapath)
{
	xvector<xstring> filelist;
	Helper::GetFileList(luapath,"*.lua",filelist);
	for(int32 i=0; i<(int32)filelist.size(); i++)
	{
		xstring& luafile = filelist[i];
		LoadLuaFile(luafile);
	}

	return true;
}


//---------------------------------------------------------------------------------
bool LuaManager::GetInt(const xstring& field, int *ret)
{
	int top = lua_gettop(m_pLua);
	lua_getglobal(m_pLua, field.c_str());
	if (lua_isnumber(m_pLua, -1))
	{
		*ret = (int)lua_tonumber(m_pLua, -1);
		lua_settop(m_pLua, top);

		return true;
	}
	else
	{
		lua_settop(m_pLua, top);

		return false;
	}
}


//---------------------------------------------------------------------------------
bool LuaManager::GetFloat(const xstring& field, float *ret)
{
	int top = lua_gettop(m_pLua);
	lua_getglobal(m_pLua, field.c_str());
	if (lua_isnumber(m_pLua, -1))
	{
		*ret = (float)lua_tonumber(m_pLua, -1);
		lua_settop(m_pLua, top);

		return true;
	}
	else
	{
		lua_settop(m_pLua, top);

		return false;
	}
}


//---------------------------------------------------------------------------------
bool LuaManager::GetString(const xstring& field, const char **ret)
{
	int top = lua_gettop(m_pLua);
	lua_getglobal(m_pLua, field.c_str());
	if (lua_isstring(m_pLua, -1))
	{
		*ret = lua_tostring(m_pLua, -1);
		lua_settop(m_pLua, top);

		return true;
	}
	else
	{
		lua_settop(m_pLua, top);

		return false;
	}
}
