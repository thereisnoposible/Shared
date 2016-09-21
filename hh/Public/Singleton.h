#pragma once
template<typename T>
class Singleton
{
public:
	static T& getInstance()
	{
		return *single;
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