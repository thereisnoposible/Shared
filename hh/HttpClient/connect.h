#pragma once
#include <string>
#include <unordered_map>
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include "http_define.h"

namespace http
{
	class connect_manager;

	void split(const std::string&, const std::string&, std::vector<std::string>&);

	class connect :public std::enable_shared_from_this<connect>
	{		
	public:		
		connect(uint32 id, std::shared_ptr<boost::asio::ip::tcp::socket>& skt, 
			http_request& request, std::function<void(int32)> func);
		void connectto();
	private:
		void on_connect(boost::system::error_code ec);
		void on_read(const boost::system::error_code& e, std::size_t bytes_transferred);
		void makedata();

		void on_write(std::shared_ptr<std::string>& str, const boost::system::error_code& e, std::size_t bytes_transferred);

		void parse_response();

		void close_socket();
	public:
		uint32 _id;
		http_request _request;
		http_response _response;
	private:
		char _data[131072];
		uint32 off;
		std::function<void(int32)> _callback;
		std::shared_ptr<boost::asio::ip::tcp::socket> _skt;
	};
}