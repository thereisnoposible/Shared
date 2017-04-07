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
}
