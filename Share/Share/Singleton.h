#pragma once
template<class T>

class  SingleThreaded
{
public:

	struct Lock
	{
		Lock()
		{
		}

		Lock(const T&)
		{
		}

		Lock(const SingleThreaded<T>&)
		{
		}
	};
};

template<class T>
class  OperatorNew
{
public:

	static T* Create()
	{
		return (new T);
	}

	static void Destroy(T* obj)
	{
		delete obj;
	}
};

void  at_exit(void(*func)());

template<class T>
/**
* @brief
*
*/
class  ObjectLifeTime
{
public:

	static void ScheduleCall(void(*destroyer)())
	{
		at_exit(destroyer);
	}

	static void OnDeadReference()
	{
	throw std::runtime_error("Dead Reference");
	}
};

template
<
typename T,
class ThreadingModel = SingleThreaded<T>,
class CreatePolicy = OperatorNew<T>,
class LifeTimePolicy = ObjectLifeTime<T>
>
class  Singleton
{
public:
	static T& Instance();

protected:
	Singleton()
	{
	}

private:

	Singleton(const Singleton&);

	Singleton& operator=(const Singleton&);

	static void DestroySingleton();

	typedef typename ThreadingModel::Lock Guard;
	static T* si_instance; /**< TODO */
	static bool si_destroyed; /**< TODO */
};

template<typename T, class ThreadingModel, class CreatePolicy, class LifeTimePolicy>
T* Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy>::si_instance = nullptr; /**< TODO */

template<typename T, class ThreadingModel, class CreatePolicy, class LifeTimePolicy>
bool Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy>::si_destroyed = false; /**< TODO */

#define INSTANTIATE_SINGLETON_1(TYPE) \
	template class  Singleton<TYPE, SingleThreaded<TYPE>, OperatorNew<TYPE>, ObjectLifeTime<TYPE> >;

#define INSTANTIATE_SINGLETON_2(TYPE, THREADINGMODEL) \
	template class  Singleton<TYPE, THREADINGMODEL, OperatorNew<TYPE>, ObjectLifeTime<TYPE> >;

#define INSTANTIATE_SINGLETON_3(TYPE, THREADINGMODEL, CREATIONPOLICY ) \
	template class  Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, ObjectLifeTime<TYPE> >;

#define INSTANTIATE_SINGLETON_4(TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME) \
	template class  Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >;