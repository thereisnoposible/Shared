#include "stdafx.h"
#include "CoroutineManager.h"

CoroutineManager* Singleton<CoroutineManager>::single = nullptr;

CoroutineManager::CoroutineManager()
{
	_dbid = 0;
	for (int32 i = 0; i < 64; i++)
	{
		CoroutineItem item;
		item._coroutine = new boost::coroutines::symmetric_coroutine<void*>::call_type;
		item._fn = new boost::function<void(boost::coroutines::symmetric_coroutine<void*>::yield_type&)>;
		item.bregist = false;

		_pool.push_back(item);
	}
}

CoroutineManager::~CoroutineManager()
{
	for (auto it = _pool.begin(); it != _pool.end(); ++it)
	{
		delete it->_coroutine;
		delete it->_fn;
	}
	_pool.clear();

	for (auto it = _used.begin(); it != _used.end(); ++it)
	{
		delete it->second._coroutine;
		delete it->second._fn;
	}
	_used.clear();
}

int64 CoroutineManager::Alloc()
{
	auto p = _pool.front();
	_dbid++;
	_pool.pop_front();
	_used[_dbid] = p;

	if (_pool.size() == 0)
	{
		int32 count = _used.size() * 7;
		if (_used.size() >= 512)
		{
			count = _used.size();
			for (int32 i = 0; i < count; i++)
			{
				CoroutineItem item;
				item._coroutine = new boost::coroutines::symmetric_coroutine<void*>::call_type;
				item._fn = new boost::function<void(boost::coroutines::symmetric_coroutine<void*>::yield_type&)>;
				item.bregist = false;

				_pool.push_back(item);
			}
		}
	}

	_dbid++;
	_pool.pop_front();
	_used[_dbid] = p;
	return _dbid;
}

bool CoroutineManager::RegistFunc(int64 dbid, const boost::function<void(boost::coroutines::symmetric_coroutine<void*>::yield_type&)>& fn)
{
	auto it = _used.find(dbid);
	if (it == _used.end())
		return false;

	CoroutineItem& temp = it->second;
	if (temp.bregist)
		return false;

	temp.bregist = true;
	new(temp._fn) boost::function<void(boost::coroutines::symmetric_coroutine<void*>::yield_type&)>(fn);
	new(temp._coroutine)boost::coroutines::symmetric_coroutine<void*>::call_type(*temp._fn);

	if (temp._coroutine->operator bool())
	{
		int32 i = 0;
	}

	return true;
}

void CoroutineManager::update()
{
	for (int32 i = 0; i < _frees.size(); i++)
	{
		_frees[i]._coroutine->~symmetric_coroutine_call();
		_frees[i]._fn->~function();
		_frees[i].bregist = false;
		_pool.push_back(_frees[i]);
	}

	_frees.clear();
}

void CoroutineManager::Free(int64 dbid)
{
	auto it = _used.find(dbid);
	if (it == _used.end())
		return;

	_frees.push_back(it->second);
	_used.erase(it);

}

boost::coroutines::symmetric_coroutine<void*>::call_type* CoroutineManager::GetCoroutine(int64 dbid)
{
	auto it = _used.find(dbid);
	if (it == _used.end())
		return nullptr;

	if (it->second.bregist == false)
		return nullptr;

	return it->second._coroutine;
}