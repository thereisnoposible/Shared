#pragma once

class XX_Env
{
public:
	DECLARE_LUA_CLASS(XX_Env); // ��������XX_Env��

	void Init(lua_State* L);
	int luaSay(lua_State* L);

private:
	char m_szName[32];
	int m_iLevel;
};