#include "connect_manager.h"
#include "connect.h"
#include <boost/asio.hpp>

namespace http
{
	using boost::asio::ip::tcp;

	connect_manager::connect_manager() :_io_service_pool(4), id(0)
	{
		_io_service_pool.run();
	}
	connect_manager::~connect_manager()
	{
		_io_service_pool.stop();
	}
	void connect_manager::update()
	{
		//if (1)
		std::vector<int32> temp_finish;
		{
			std::lock_guard<std::mutex> connlock(_mutex);
			temp_finish = _finish_connid;
			_finish_connid.clear();
		}

		for (int32 i = 0; i < (int32)temp_finish.size(); i++)
		{
			std::unordered_map<int32, str_conn>::iterator it = _post_conn.find(temp_finish[i]);
			if (it == _post_conn.end())
				continue;
			if (!it->second.callback._Empty())
				it->second.callback(it->second.conn->_response);
			_post_conn.erase(it);
		}
	}
	void connect_manager::post(http_request& request, std::function<void(http_response&)> func)
	{		
		std::shared_ptr<tcp::socket> skt(new tcp::socket(_io_service_pool.get_io_service()));
		std::shared_ptr<connect> conn(new connect(id, skt, request, std::bind(&connect_manager::on_finish, this, std::placeholders::_1)));
		str_conn str;
		str.conn = conn;
		str.callback = func;
		_post_conn.insert(std::make_pair(id, str));
		conn->connectto();

		id++;
	}
	void connect_manager::on_finish(int32 id)
	{
		std::lock_guard<std::mutex> connlock(_mutex);
		_finish_connid.push_back(id);
	}
}