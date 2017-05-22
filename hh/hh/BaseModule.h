#pragma once
#include <string>
class Player;

class BaseModule
{
public:
	BaseModule(const std::string& name, Player* pOwner)
	{
		m_ModuleName = name;
		m_pOwner = pOwner;
		bInit = false;
		init_time = 0;
	}
	virtual ~BaseModule(void){};
	virtual void registmessage() = 0;
	virtual void InitFunc() = 0;
	void Init(){ InitFunc(); init_time = time(NULL); }
	int64 GetInitTime(){ return init_time; }
	bool IsInitOK(){ return bInit; }
	void SetInitOK(){ bInit = true; registmessage(); }

public:
	std::string GetModuleName(){ return m_ModuleName; }

	Player* GetOwner(){ return m_pOwner; }

	virtual void OnLogin(){ return; }
	virtual void OnLogout(){ return; }

protected:

protected:
	std::string m_ModuleName;
	Player* m_pOwner;
	bool bInit;
	int64 init_time;
};

class ModuleFactory
{
public:
	ModuleFactory(const std::string& name);

	/**
	* @brief		Desconstructor
	* @details		注意，析构函数必须为虚函数
	* @remarks
	*/
	virtual ~ModuleFactory(void);

public:
	virtual BaseModule* CreateModule(Player* pOwner) = 0;

	std::string GetModuleName(){ return m_ModuleName; }

protected:
	std::string m_ModuleName;
};
