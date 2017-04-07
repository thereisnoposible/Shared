#pragma once
#include <string>
#include <unordered_map>
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include "http_define.h"
#include <boost/asio/ssl.hpp>

namespace http
{
	class connect_manager;

	void split(const std::string&, const std::string&, std::vector<std::string>&);

	class connect :public std::enable_shared_from_this<connect>
	{		
	public:		
		connect(uint32 id, boost::asio::io_service& io_s,
			http_request& request, std::function<void(int32)> func, bool is_ssl = false);
		void connectto();
		void close();

		void set_proxy(const std::string& ip, const std::string& port);
	private:
		bool connect::verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);
		void on_connect(boost::system::error_code ec);
		void on_connect_ssl(boost::system::error_code ec);
		void on_handshake(boost::system::error_code ec);
		void on_connect_time_out(const boost::system::error_code& err);
		void on_read(const boost::system::error_code& e, std::size_t bytes_transferred);

		void on_read_head(const boost::system::error_code& e, std::size_t bytes_transferred);
		void on_read_body_has_length(const boost::system::error_code& e, std::size_t bytes_transferred);
		void on_read_body_no_length_head(const boost::system::error_code& e, std::size_t bytes_transferred);
		void on_read_body_no_length_body(const boost::system::error_code& e, std::size_t bytes_transferred);
		void makedata();

		void on_write(std::shared_ptr<std::string>& str, const boost::system::error_code& e, std::size_t bytes_transferred);

		size_t get_length();
		void parse_body();

		void close_socket();

		bool is_receive_head();
	public:
		uint32 _id;
		http_request _request;
		http_response _response;
	private:
		std::vector<char> _data;
		uint32 off;
		uint32 head_off = 0;
		std::function<void(int32)> _callback;
		bool bclose = false;
		std::shared_ptr<boost::asio::ip::tcp::socket> _skt;
		std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _ssl_skt;
		boost::asio::deadline_timer _time_out;

		std::string proxy_ip;
		std::string proxy_port;

		bool head_ready = false;

		bool is_ssl_;
	};
}