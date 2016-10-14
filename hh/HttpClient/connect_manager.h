#pragma once
#include <boost/asio.hpp>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "connect.h"
#include "io_service_pool.h"

namespace http
{
	class connect_manager
	{	
	public:
		struct str_conn
		{
			std::shared_ptr<connect> conn;
			std::function<void(http_response&)> callback;
		};

		connect_manager();
		~connect_manager();
		void update();
		void post(http_request& request, std::function<void(http_response&)> func);

	private:
		void on_finish(int32 id);

	private:
		io_service_pool _io_service_pool;	
		std::unordered_map<int32, str_conn> _post_conn;
		std::vector<str_conn> _post;
		std::vector<int32> _finish_connid;

		uint32 id;

		std::mutex _mutex;
		std::thread _pthread;

		bool bExit;
	};
}