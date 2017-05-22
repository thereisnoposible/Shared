#include "stdafx.h"
#include <iostream>
#include <memory>
#include "google/protobuf/stubs/common.h"

#include "Application.h"

#include "crashReport/crashReport.h"

#ifdef _DEBUG
#include "Visual Leak Detector/include/vld.h"
#endif // DEBUG

bool bExit = false;
void loop()
{
	while (!bExit)
	{
		std::string cmd;
		std::cin >> cmd;
		if (cmd == "exit")
		{
			bExit = true;
		}
	}
}
#include <boost/coroutine/all.hpp>

std::vector<int> merge(const std::vector<int>& a, const std::vector<int>& b){
	std::vector<int> c;
	std::size_t idx_a = 0, idx_b = 0;
	boost::coroutines::symmetric_coroutine<void>::call_type *other_a = 0, *other_b = 0;

	boost::coroutines::symmetric_coroutine<void>::call_type coro_a(
		[&](boost::coroutines::symmetric_coroutine<void>::yield_type& yield){
		while (idx_a < a.size()){
			if (b[idx_b] < a[idx_a])    // test if element in array b is less than in array a
				yield(*other_b);     // yield to coroutine coro_b
			c.push_back(a[idx_a++]); // add element to final array
		}
		// add remaining elements of array b
		while (idx_b < b.size())
			c.push_back(b[idx_b++]);
	});

	boost::coroutines::symmetric_coroutine<void>::call_type coro_b(
		[&](boost::coroutines::symmetric_coroutine<void>::yield_type& yield){
		yield();
		while (idx_b < b.size()){
			if (a[idx_a] < b[idx_b])    // test if element in array a is less than in array b
				yield(*other_a);     // yield to coroutine coro_a
			c.push_back(b[idx_b++]); // add element to final array
		}
		// add remaining elements of array a
		while (idx_a < a.size())
			c.push_back(a[idx_a++]);
	});


	other_a = &coro_a;
	other_b = &coro_b;

	coro_a(); // enter coroutine-fn of coro_a

	while (coro_a)
	{
		coro_a();
	}

	return c;
}

class Actor
{
public:
	struct struct_message
	{
		struct_message(int32 mess, boost::coroutines::symmetric_coroutine<void*>::call_type* ot,void* par,bool is) 
			:message(mess), other(ot), param(par), isSystem(is){}
		int32 message;
		boost::coroutines::symmetric_coroutine<void*>::call_type* other;
		void* param;
		bool isSystem;
	};

	Actor() 
	{
		_lock = nullptr;
		_messages.push_back(struct_message(0, 0, 0, true));
	}
	~Actor(){}
	void update()
	{
		auto it = _messages.begin();
		while ( it != _messages.end())
		{
			struct_message& temp = *it;
			if (temp.message == 0 && temp.isSystem)
			{
				if (_messages.size() <= 1)
					break;

				it = _messages.erase(it);
				continue;
			}

			if (temp.isSystem)
			{
				switch (temp.message)
				{
				case 1: //lock
					_lock = temp.other;
				case 2: //unlock
					_lock = nullptr;
				default:
					break;
				}

				it = _messages.erase(it);
				continue;
			}

			if (_lock && _lock != temp.other)
			{
				++it;
				continue;
			}

			auto sit = _message_functions.find(temp.message);
			if (sit == _message_functions.end())
			{
				it = _messages.erase(it);
				continue;
			}

			sit->second(temp.other, temp.param);
			it = _messages.erase(it);
		}
	}

	void regist_function(int32 id, void(*p)(boost::coroutines::symmetric_coroutine<void*>::call_type*, void* param))
	{
		_message_functions[id] = p;
	}

	void post_message(int32 message, boost::coroutines::symmetric_coroutine<void*>::call_type* other,void* param)
	{
		if (_messages.size() == 1 && _messages.begin()->message == 0)
		{
			_messages.begin()->message = message;
			_messages.begin()->other = other;
			_messages.begin()->param = param;
			_messages.begin()->isSystem = false;
			return;
		}
		_messages.push_back(struct_message(message, other, param, false));
	}

	void post_message_and_lock(int32 message, boost::coroutines::symmetric_coroutine<void*>::call_type* other, void* param)
	{
		post_message(message, other, param);

		_messages.push_back(struct_message(1, other, param, true));
	}

	void un_lock(boost::coroutines::symmetric_coroutine<void*>::call_type* other)
	{
		_messages.push_back(struct_message(2, other, 0, true));
	}

private:
	boost::coroutines::symmetric_coroutine<void*>::call_type* _lock;
	std::list<struct_message> _messages;
	xmap<int32, void(*)(boost::coroutines::symmetric_coroutine<void*>::call_type*,void*)> _message_functions;
};

class PrintActor :public Actor
{
public:
	PrintActor()
	{
		regist_function(1, [](boost::coroutines::symmetric_coroutine<void*>::call_type* source, void* param){
			char* strr = static_cast<char *>(param);

			std::cout << "this is " << strr << "\n";

			source->operator()("over");
		});
	}	
};

class sssss
{
public:
	void aa(boost::function<void(boost::coroutines::symmetric_coroutine<void*>::yield_type&)>* fn)
	{
		coro_b = new boost::coroutines::symmetric_coroutine<void*>::call_type(*fn);
	}
	boost::coroutines::symmetric_coroutine<void*>::call_type* coro_b;
};

boost::coroutines::symmetric_coroutine<void*>::call_type* iiii()
{
	boost::coroutines::symmetric_coroutine<void*>::call_type* p = new boost::coroutines::symmetric_coroutine<void*>::call_type([&](boost::coroutines::symmetric_coroutine<void*>::yield_type& yield){
		std::cout << static_cast<char*>(yield.get()) << "\n";
		std::cout << "print over\n";
	});
	return p;
}

Application* getApplication()
{
	boost::coroutines::symmetric_coroutine<void*>::call_type* ppp = iiii();
	ppp->operator()("jj");
	
	PrintActor pra;
	new(&pra) PrintActor();

	//pra.PrintActor();
	//std::thread thrr(&PrintActor::update, &pra);
	//pra.post_message_and_lock(1, &coro_b, "hello");
	////pra.update();

	//typedef boost::coroutines::asymmetric_coroutine< int >::pull_type pull_coro_t;
	//typedef boost::coroutines::asymmetric_coroutine< int >::push_type push_coro_t;

	//push_coro_t sink([](pull_coro_t & source){
	//	std::cout << source.get();
	//	source();
	//	std::cout << source.get();
	//	source();
	//	std::cout << source.get();
	//	source();
	//	std::cout << source.get();
	//});

	//pull_coro_t pll([](push_coro_t& source){
	//});

	//if (pll)
	//{
	//	pll.get();
	//}

	//int i = 0;
	//while (sink)
	//{
	//	++i;
	//	sink(i);
	//	std::cout << "-";
	//}

	//xvector<int32>aaa;
	//aaa.push_back(1);
	//aaa.push_back(3);
	//aaa.push_back(5);
	//xvector<int32>bbb;
	//bbb.push_back(2);
	//bbb.push_back(4);
	//bbb.push_back(6);
	//merge(aaa, bbb);


    Application* p = new Application;
    return p;
}

void aaa(Application* app)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::chrono::steady_clock::time_point fLast = std::chrono::steady_clock::now();
    while (!bExit)
    {
        std::chrono::steady_clock::time_point fCurr = std::chrono::steady_clock::now();
        std::chrono::duration<double> usetime = std::chrono::duration_cast<std::chrono::duration<double>>(fCurr - fLast);
        //在这里控制帧率
        if (usetime.count() < 0.00000000001)
        {
            Sleep(1);
            continue;
        }

        __try
        {
            app->update(usetime.count());
        }
        __except (CrashHandler(GetExceptionInformation()))
        {

        }
       
        fLast = fCurr;
    }

    google::protobuf::ShutdownProtobufLibrary();
}

boost::thread* getThread()
{
    boost::thread* p = new boost::thread(&loop);
    return p;
}

int main()
{
    Application* app = nullptr;
    boost::thread* thr = nullptr;

    __try
    {
        app = getApplication();
        thr = getThread();
    }
	__except (CrashHandler(GetExceptionInformation()))
    {
        return 0;
    }

    __try
    {
        aaa(app);
    }
	__except (CrashHandler(GetExceptionInformation()))
    {

    }

    delete app;
    delete thr;

	return 0;
}