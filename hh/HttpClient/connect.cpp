#include "connect.h"
#include "connect_manager.h"
#include <boost/asio.hpp>
#include <memory>
#include "Helper.h"
#include <winsock2.h>
#include <vector>

namespace http
{
	void split(const std::string&str, const std::string&spl, std::vector<std::string>&sAstr)
	{
		if (str.empty())
			return;
		int pos = 0;
		pos = str.find(spl, pos);
		if (pos != std::string::npos)
		{
			if (pos != 0)
				sAstr.push_back(std::string().assign(str.begin(), str.begin() + pos));
			split(str.c_str() + pos + spl.length(), spl, sAstr);
		}
		else
			sAstr.push_back(str);
	}

	bool find(char*read_buf, uint32 read_size, char*buf, uint32 buf_size)
	{
		for (uint32 i = 0; i <= read_size - buf_size; i++)
		{
			for (uint32 j = 0; j < buf_size; j++)
			{
				if (*(read_buf+i+j) != *(buf+j))
					break;

				if (j == buf_size - 1)
					return true;
			}
		}
		return false;
	}

	connect::connect(uint32 id,std::shared_ptr<boost::asio::ip::tcp::socket>& skt, 
		http_request& request, std::function<void(int32)> func)
	{
		_id = id;
		_request = request;
		_callback = func;
		_skt = skt;
	}

	void connect::connectto()
	{
		std::string addr;
		std::string port;
		std::string hp = "http://";
		if (_request.host.find(hp) != std::string::npos)
		{
			_request.host.erase(_request.host.find(hp), hp.size());
		}
		int32 pos = _request.host.find(":");
		if (pos == std::string::npos)
		{
			addr = _request.host.substr(0, pos);
			port = "80";
			//_request.host += ":80";
		}
		else
		{
			addr = _request.host.substr(0, pos);
			port = _request.host.substr(pos + 1);
		}
		std::string ip;
		hostent* ht = gethostbyname(addr.c_str());
		if (ht != NULL)
		{	
			struct in_addr **addr_list = (struct in_addr **) ht->h_addr_list;
			in_addr &ip_addr = *addr_list[0];
			ip = inet_ntoa(ip_addr);
		}
		else
		{
			ip = addr;
		}
		
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), atoi(port.c_str()));
		boost::system::error_code error;

		//_skt->connect(endpoint, error);
		_skt->async_connect(endpoint, boost::bind(&connect::on_connect, shared_from_this(), _1));
	}

	void connect::on_connect(boost::system::error_code ec)
	{
		if (ec)
		{
			_skt->get_io_service().post(boost::bind(&connect::close_socket, shared_from_this()));
			_callback(_id);
			return;
		}

		off = 0;
		memset(_data, 0, sizeof(_data));
		boost::asio::buffer(_data, off);
		_skt->async_read_some(boost::asio::buffer(_data + off, sizeof(_data) - off),
			std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		makedata();
	}
	void connect::on_read(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			parse_response();
			_skt->get_io_service().post(boost::bind(&connect::close_socket, shared_from_this()));
			_callback(_id);
			return;
		}

		off += bytes_transferred;

		if (_request.pend_flag == NULL)
		{
			parse_response();
			_skt->get_io_service().post(boost::bind(&connect::close_socket, shared_from_this()));
			_callback(_id);
			return;
		}

		if (_request.pend_flag != NULL && find(_data, off, _request.pend_flag, strlen(_request.pend_flag)))
		{
			//memcmp(_data + off - strlen(_request.pend_flag), _request.pend_flag, strlen(_request.pend_flag)) == 0
			
			parse_response();
			_skt->get_io_service().post(boost::bind(&connect::close_socket, shared_from_this()));
			_callback(_id);
			return;
		}

		_skt->async_read_some(boost::asio::buffer(_data + off, sizeof(_data) - off), 
			std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void connect::makedata()
	{
		std::stringstream ss;
		ss << _request.type << _request.url << _request.vesion << "\r\n";
		std::unordered_map<std::string, std::string>::iterator it = _request.head.find("Host");
		if (it == _request.head.end())
		{
			_request.head["Host"] = _request.host;
		}
		it = _request.head.find("Content-Length");
		if (it == _request.head.end())
		{
			std::stringstream temp;
			temp << _request.body.size();
			_request.head["Content-Length"] = temp.str();
		}
		it = _request.head.begin();
		for (; it != _request.head.end(); ++it)
		{
			ss << it->first << ":" << it->second << "\r\n";
		}
		ss << "\r\n";
		if (_request.head.size() == 0)
			ss << "\r\n";

		ss << _request.body;
		std::shared_ptr<std::string> str (new std::string(ss.str()));
		_skt->async_write_some(boost::asio::buffer(str->c_str(), str->size()), 
			std::bind(&connect::on_write, shared_from_this(), str, std::placeholders::_1, std::placeholders::_2));
	}
	void connect::on_write(std::shared_ptr<std::string>& str, const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			_skt->get_io_service().post(boost::bind(&connect::close_socket, shared_from_this()));
			_callback(_id);
			return;
		}

	}
	void connect::close_socket()
	{
		try{
			boost::system::error_code ignored_ec;
			_skt->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			_skt->close();
			_skt.reset();
		}
		catch (...)
		{
			//Helper::dbgTrace("socket error when close socket");
		}
	}
	void connect::parse_response()
	{
		std::string res(_data, off);
		std::vector<std::string> res_vec;
		int32 pos = res.find("\r\n\r\n");

		if (res_vec.size() == std::string::npos)
			return;
		res_vec.push_back(res.substr(0, pos));
		res_vec.push_back(res.substr(pos + sizeof("\r\n\r\n") - 1));
		std::vector<std::string> head_vec;
		split(res_vec[0], "\r\n", head_vec);
		if (head_vec.size() == 0)
			return;
		std::vector<std::string> head_res;
		split(head_vec[0], " ", head_res);
		if (head_res.size() < 3)
			return;

		_response.code = atoi(head_res[1].c_str());

		for (int32 i = 1; i < (int32)head_vec.size(); i++)
		{
			int32 pos = head_vec[i].find(":");
			if (pos == std::string::npos)
				continue;

			_response.head[head_vec[i].substr(0, pos)] = head_vec[i].substr(pos + 1);
		}
		for (int32 i = 1; i < (int32)res_vec.size(); i++)
		{
			_response.body += res_vec[i];
		}
	}
}

