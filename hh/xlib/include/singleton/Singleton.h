#pragma once
namespace xlib
{
	template<typename T>
	class Singleton
	{
	public:
		static T& getInstance()
		{
			return *single;
		}

		static T* create()
		{
			if (!single)
				single = new T;

			return single;
		}

		static void destory()
		{
			if (single)
			{
				delete single;
				single = nullptr;
			}
		}

	protected:
		Singleton()
		{
			if (single == nullptr)
			{
				single = static_cast<T*>(this);
			}
		};
		virtual ~Singleton()
		{

		};

		static T* single;
	};

#define _V3_INNER(_A,_B,_C)  _A##_B##_C
#define _V4_INNER(_A,_B,_C,_D) _A##_B##_C##_D
#define _V5_INNER(_A,_B,_C,_D,_E) _A##_B##_C##_D##_E
#define _V6_INNER(_A,_B,_C,_D,_E,_F) _A##_B##_C##_D##_E##_F

#define _V3(_A,_B,_C)  _V3_INNER(_A,_B,_C)
#define _V4(_A,_B,_C,_D)  _V4_INNER(_A,_B,_C,_D)
#define _V5(_A,_B,_C,_D,_E)  _V5_INNER(_A,_B,_C,_D,_E)
#define _V6(_A,_B,_C,_D,_E,_F)  _V6_INNER(_A,_B,_C,_D,_E,_F)

#define STATIC_INIT public: static void StaticInit()

#define STATIC_INIT_RUN(_ClassName)						\
class _V4(CZS##_ClassName, _, __LINE__, StaticInitialize)	\
			{															\
public:														\
	_V4(CZS##_ClassName, _, __LINE__, StaticInitialize)()	\
				{														\
		_ClassName::StaticInit();							\
				}														\
															\
			}_V4(g_the##_ClassName, _, __LINE__,StaticInit);			\
void _ClassName::StaticInit()

	//单例模式宏
#define SINGLETON_DECLARE(_ClassName)			\
public:											\
	static _ClassName& GetInstance();			\
	static void DestroyInstance();				\
private:										\
	_ClassName(const _ClassName&);				\
	_ClassName& operator = (const _ClassName&); \
	_ClassName();								\
	static _ClassName* s_pInstance;

#define SINGLETON_DECLARE_OPERATE(_ClassName)		\
_ClassName* _ClassName::s_pInstance = NULL;			\
_ClassName& _ClassName::GetInstance()				\
			{													\
	if(s_pInstance == NULL )						\
				{												\
		s_pInstance = new _ClassName;				\
				}												\
	return *s_pInstance;							\
			}													\
void _ClassName::DestroyInstance()					\
			{													\
	if(s_pInstance != NULL )						\
				{												\
		delete s_pInstance;							\
		s_pInstance = NULL;							\
				}												\
			}													\
class _V4(CZS##_ClassName, _, __LINE__, Producer)	\
			{													\
public:												\
	_V4(CZS##_ClassName, _, __LINE__, Producer)()	\
				{												\
		_ClassName::GetInstance();					\
				}												\
													\
	~_V4(CZS##_ClassName, _, __LINE__, Producer)()\
				{												\
		_ClassName::DestroyInstance();				\
				}												\
			}_V4(g_the##_ClassName, _, __LINE__,Singleton);

#define PLUGIN_MANAGER_SINGLETON(_ClassName)			\
public:													\
	static _ClassName& GetInstance();					\
protected:												\
	_ClassName();										\
	void _Construction();								\
	static _ClassName* s_pInstanca;

#if defined( _MSC_VER ) && _MSC_VER < 1200

#define PLUGIN_MANAGER_SINGLETON_OPERATE(_ClassName)	\
	_ClassName* _ClassName::s_pInstanca = 0;			\
	_ClassName& GetInstance()							\
				{													\
		ZS_ASSERT_MSG(s_pInstanca != 0, "单例没有初始化");\
		return *s_pInstanca;							\
				}													\
	_ClassName::_ClassName()							\
				{													\
		int32 offset = (int32)(_Type*)1 - (int32)(Singleton <_Type>*)(_Type*)1;	\
		s_pInstanca = (_Type*)((int32)this + offset);		\
		_Construction();								\
				}													\
	void _ClassName::_Construction()

#else

#define PLUGIN_MANAGER_SINGLETON_OPERATE(_ClassName)	\
	_ClassName* _ClassName::s_pInstanca = 0;			\
	_ClassName& _ClassName::GetInstance()				\
				{													\
		ZS_ASSERT_MSG(s_pInstanca != 0, "单例没有初始化");\
		return *s_pInstanca;							\
				}													\
	_ClassName::_ClassName()							\
				{													\
		s_pInstanca = this;								\
		_Construction();								\
				}													\
	void _ClassName::_Construction()

#endif

//单例容器 注意 容器是单例的 类本身不单例
template < typename _Type >
class CSingleton
{
public:
	static _Type& GetInstance()
	{
		static _Type _instanca;
		return _instanca;
	}
private:
	CSingleton(){}
	CSingleton(const CSingleton&);
	CSingleton& operator = (const CSingleton&);
};
template < typename _Type >
class CAbstracSingleton
{
public:
	static _Type& GetInstance()
	{
		assert(s_pInstanca != 0);
		return *s_pInstanca;
	}
	static _Type* GetInstancePtr()
	{
		return s_pInstanca;
	}
protected:
	CAbstracSingleton()
	{
		assert(s_pInstanca == 0);
#if defined( _MSC_VER ) && _MSC_VER < 1200
		int32 offset = (int32)(_Type*)1 - (int32)(Singleton <_Type>*)(_Type*)1;
		s_pInstanca = (_Type*)((int32)this + offset);
#else
		s_pInstanca = static_cast<_Type*>(this);
#endif
	}
	static _Type* s_pInstanca;
};

//template < typename _Type >_Type* CAbstracSingleton<_Type>::s_pInstanca = 0;
	

#undef _V3_INNER
#undef _V4_INNER
#undef _V5_INNER
#undef _V6_INNER

#undef _V3
#undef _V4
#undef _V5
#undef _V6
}