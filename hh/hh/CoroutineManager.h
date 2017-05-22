#pragma once
#include <boost/coroutine/all.hpp>
#include <list>
class CoroutineManager : public Singleton<CoroutineManager>
{
public:
	struct CoroutineItem
	{
		boost::coroutines::symmetric_coroutine<void*>::call_type* _coroutine;
		boost::function<void(boost::coroutines::symmetric_coroutine<void*>::yield_type&)>* _fn;
		bool bregist;
	};

	CoroutineManager();
	~CoroutineManager();

	void update();

	int64 Alloc();
	bool RegistFunc(int64 dbid, const boost::function<void(boost::coroutines::symmetric_coroutine<void*>::yield_type&)>& fn);
	void Free(int64 dbid);

	boost::coroutines::symmetric_coroutine<void*>::call_type* GetCoroutine(int64 dbid);
protected:

private:
	std::list<CoroutineItem> _pool;
	std::map<int32, CoroutineItem> _used;
	std::vector<CoroutineItem> _frees;
	int64 _dbid;
};