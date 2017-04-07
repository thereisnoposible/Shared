#include "connect.h"
#include "connect_manager.h"
#include <boost/asio.hpp>
#include <memory>
#include "helper/Helper.h"
#include <winsock2.h>
#include <vector>

using namespace xlib;

namespace http
{
	int FindString(const char* str, int size, const std::string&spl)
	{
		int pos = -1;
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < (int)spl.size(); j++)
			{
				if (*(str + i + j) != spl[j])
					break;

				if (j == spl.size() - 1)
					pos = i;
			}

			if (pos != -1)
				break;
		}

		return pos;
	}

	void split(const char* str, int size, const std::string&spl, std::vector<std::string>&sAstr)
	{
		if (size == 0)
			return;

		int pos = FindString(str, size, spl);
		int off = 0;
		while (1)
		{
			if (pos == -1)
				break;

			if (pos - off != 0)
			{
				std::string strt;
				strt.resize(pos - off);
				if (pos - off > 0)
					memcpy(&*strt.begin(), str + off, pos - off);
				sAstr.push_back(strt);

				//sAstr.push_back(std::string(str + off, pos - off));
			}

			off = pos + spl.size();
			pos = FindString(str + off, size - off, spl);

			if (pos == -1)
				break;

			pos = pos + off;
		}

		if (pos == -1 && size - off != 0)
		{
			std::string strt;
			strt.resize(size - off);
			if (size - off > 0)
				memcpy(&*strt.begin(), str + off, size - off);
			sAstr.push_back(strt);
			//sAstr.push_back(std::string(str + off, size - off));
		}
	}

	void split_has_empty(const char* str, int size, const std::string&spl, std::vector<std::string>&sAstr)
	{
		if (size == 0)
			return;

		int pos = FindString(str, size, spl);
		int off = 0;
		while (1)
		{
			if (pos == -1)
				break;

			std::string strt;
			strt.resize(pos - off);
			if (pos - off > 0)
				memcpy(&*strt.begin(), str + off, pos - off);

			//sAstr.push_back(std::string(str + off, pos - off));
			sAstr.push_back(strt);
			off = pos + spl.size();
			pos = FindString(str + off, size - off, spl);

			if (pos == -1)
				break;

			pos = pos + off;
		}

		sAstr.push_back(std::string(str + off, size - off));
	}

	bool find(char*read_buf, uint32 read_size, char*buf, uint32 buf_size)
	{
		for (uint32 i = 0; i <= read_size - buf_size; i++)
		{
			for (uint32 j = 0; j < buf_size; j++)
			{
				if (*(read_buf + i + j) != *(buf + j))
					break;

				if (j == buf_size - 1)
					return true;
			}
		}
		return false;
	}

	bool rfind(char*read_buf, uint32 read_size, char*buf, uint32 buf_size)
	{
		for (uint32 i = read_size - buf_size; i >= 0; i--)
		{
			for (uint32 j = buf_size - 1; j >=0; j--)
			{
				if (*(read_buf+i+j) != *(buf+j))
					break;

				if (j == buf_size - 1)
					return true;
			}
		}
		return false;
	}

	connect::connect(uint32 id, boost::asio::io_service& io_s,
		http_request& request, std::function<void(int32)> func, bool is_ssl) : 
		is_ssl_(is_ssl)
		, _ssl_skt(nullptr)
		, _time_out(io_s)
	{
		_id = id;
		_request = request;
		_callback = func;
		_data.resize(1000, 0);

		if (is_ssl_)
		{
			boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
			ctx.set_default_verify_paths();
			boost::system::error_code ec;
			ctx.use_private_key(boost::asio::const_buffer("ca.pem", sizeof("ca.pem")), boost::asio::ssl::context::pem, ec);
			if (ec)
			{
				std::cout << ec.message() << "\n";
			}
			//ctx.load_verify_file("ar.cer");
			_ssl_skt = std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
				(new boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(io_s, ctx));

			_ssl_skt->set_verify_mode(boost::asio::ssl::verify_none);

			//_ssl_skt->set_verify_callback(boost::asio::ssl::rfc2818_verification("www.baidu.com"));
			_ssl_skt->set_verify_callback(boost::bind(&connect::verify_certificate, this, _1, _2));
		}
		else
		{
			_skt = std::shared_ptr<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket(io_s));
		}
	}

	bool connect::verify_certificate(bool preverified,
		boost::asio::ssl::verify_context& ctx)
	{
		// The verify callback can be used to check whether the certificate that is  
		// being presented is valid for the peer. For example, RFC 2818 describes  
		// the steps involved in doing this for HTTPS. Consult the OpenSSL  
		// documentation for more details. Note that the callback is called once  
		// for each certificate in the certificate chain, starting from the root  
		// certificate authority.  

		// In this example we will simply print the certificate's subject name.  
		//if (!preverified)
		//	return true;

		char subject_name[256];
		X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
		X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
		std::cout << "Verifying " << subject_name << "\n";
		std::cout << preverified << "\n\n";

		return true;
	}

	void connect::connectto()
	{
		std::string addr;
		std::string port;
		std::string hp;
		if (_skt) 
			hp = "http://";
		if (_ssl_skt)
			hp = "https://";
		if (_request.host.find(hp) != std::string::npos)
		{
			_request.host.erase(_request.host.find(hp), hp.size());
		}
		int32 pos = _request.host.find(":");
		if (pos == std::string::npos)
		{
			addr = _request.host.substr(0, pos);
			if (_skt)
				port = "80";
			if (_ssl_skt)
				port = "443";
			//_request.host += ":80";
		}
		else
		{
			addr = _request.host.substr(0, pos);
			port = _request.host.substr(pos + 1);
		}

		if (!proxy_ip.empty())
		{
			addr = proxy_ip;
			port = proxy_port;
		}

		if (_skt)
		{
			boost::asio::ip::tcp::resolver resolver(_skt->get_io_service());
			boost::asio::ip::tcp::resolver::query query(addr.c_str(), port.c_str());
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

			boost::asio::ip::tcp::resolver::iterator end;

			if (endpoint_iterator == end)
			{
				_skt->close();
				_skt.reset();
				return;
			}

			//boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), atoi(port.c_str()));
			//boost::system::error_code error;
			if (_request.head.find("c_time_out") != _request.head.end())
			{
				_time_out.expires_from_now(boost::posix_time::milliseconds(atoi(_request.head["c_time_out"].c_str())));
				_time_out.async_wait(std::bind(&connect::on_connect_time_out, shared_from_this(),std::placeholders::_1));
			}
			//_skt->connect(endpoint, error);
			_skt->async_connect(*endpoint_iterator, boost::bind(&connect::on_connect, shared_from_this(), _1));
		}

		if (_ssl_skt)
		{
			boost::asio::ip::tcp::resolver resolver((_ssl_skt->lowest_layer()).get_io_service());
			boost::asio::ip::tcp::resolver::query query(addr.c_str(), port.c_str());
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

			boost::asio::ip::tcp::resolver::iterator end;

			if (endpoint_iterator == end)
			{
				_ssl_skt->lowest_layer().close();
				_ssl_skt.reset();
				return;
			}

			if (_request.head.find("c_time_out") != _request.head.end())
			{
				_time_out.expires_from_now(boost::posix_time::milliseconds(atoi(_request.head["c_time_out"].c_str())));
				_time_out.async_wait(std::bind(&connect::on_connect_time_out, shared_from_this(), std::placeholders::_1));
			}
			_ssl_skt->lowest_layer().async_connect(*endpoint_iterator, boost::bind(&connect::on_connect_ssl, shared_from_this(), _1));
		}
	}

	void connect::on_connect_ssl(boost::system::error_code ec)
	{
		if (ec)
		{
			close();
			return;
		}

		_ssl_skt->async_handshake(boost::asio::ssl::stream_base::client,
			boost::bind(&connect::on_handshake, this,
			boost::asio::placeholders::error));
	}

	void connect::on_handshake(boost::system::error_code ec)
	{
		if (ec)
		{
			std::cout << ec.message() << "\n";
			close();
			return;
		}

		_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin(), _data.size()),
			std::bind(&connect::on_read_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		makedata();
	}

	void connect::on_connect_time_out(const boost::system::error_code& err)
	{
		if (err)
			return;

		//_time_out.cancel();
		close();
	}

	void connect::on_connect(boost::system::error_code ec)
	{
		if (ec)
		{
			//std::cout << ec.message() << "\n";
			static int i = 0;
			i++;
			if (i % 5000 == 0)
			{
				std::cout << time(NULL) << "\n";
			}
			close();
			_callback(_id);
			return;
		}

		off = 0;
		//memset(_data, 0, sizeof(_data));
		//boost::asio::buffer(_data, off);
		//_skt->async_read_some(boost::asio::buffer(_data + off, sizeof(_data) - off),
		//	std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

		_skt->async_read_some(boost::asio::buffer(&*_data.begin(), _data.size()),
			std::bind(&connect::on_read_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

		//boost::asio::read_until(_skt,"")
		makedata();

	}

	void on_connect_ssl(boost::system::error_code ec);

	void connect::on_read_head(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			close();
			_callback(_id);
			return;
		}

		off += bytes_transferred;

		if (!is_receive_head())
		{
			if (_skt)
				_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
					std::bind(&connect::on_read_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

			if (_ssl_skt)
			{
				if (off >= _data.size())
				{
					_data.resize(_data.size() + 1000);
				}
				_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
					std::bind(&connect::on_read_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
			}
			return;
		}

		int32 length = get_length();
		if (length != -1)
		{
			if (head_off + length <= off)
			{
				_response.body = std::string(&*_data.begin(), length);
				_callback(_id);
				return;
			}

			_data.resize(length);
			off = off - head_off;
			if (_skt)
				_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
					std::bind(&connect::on_read_body_has_length, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
			if (_ssl_skt)
				_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read_body_has_length, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		}
		else
		{
			std::string temp = std::string(&*_data.begin(), off - head_off);

			int pos = temp.find("\r\n");
			if (pos == std::string::npos)
			{
				off = temp.size();
				if (_skt)
					_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
					std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
				if (_ssl_skt)
					_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
					std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
				return;
			}

			std::vector<std::string> sAtemp;
			split_has_empty(temp.c_str(), temp.size(), "\r\n", sAtemp);
			for (size_t i = 0; i < sAtemp.size(); i += 2)
			{
				if (sAtemp[i].size() > 4)
				{
					close();
					_callback(_id);
					return;
				}

				int32 siz = 0;
				Helper::hextoint32(sAtemp[i], siz);
				if (siz == 0)
				{
					_callback(_id);
					return;
				}
				
				if (i + 1 == sAtemp.size())
				{
					_data.resize(6);

					for (size_t z = 0; z < sAtemp[i].size(); z++)
					{
						_data[z] = sAtemp[i][z];
					}
					off = sAtemp[i].size();
					if (_skt)
					_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
						std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
					if (_ssl_skt)
						_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
						std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
				}
				else
				{
					if ((int32)sAtemp[i + 1].size() >= siz)
						_response.body += sAtemp[i + 1];
					else
					{
						_data.resize(sAtemp[i].size() + 2 + siz + 2);

						for (size_t z = 0; z < sAtemp[i].size(); z++)
						{
							_data[z] = sAtemp[i][z];
						}
						off = sAtemp[i].size();
						_data[off] = '\r';
						_data[off + 1] = '\n';
						off += 2;

						for (size_t z = 0; z < sAtemp[i+1].size(); z++)
						{
							_data[z + off] = sAtemp[i + 1][z];
						}

						off += sAtemp[i + 1].size();
						if (_skt)
							_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
							std::bind(&connect::on_read_body_no_length_body, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
						if (_ssl_skt)
							_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
							std::bind(&connect::on_read_body_no_length_body, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
					}
				}
			}
		}

	}

	void connect::on_read_body_has_length(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			close();
			_callback(_id);
			return;
		}

		off += bytes_transferred;

		if (off < _data.size())
		{
			if (_skt)
				_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
			if (_ssl_skt)
				_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
			return;
		}

		_response.body = std::string(&*_data.begin(), off);
		_callback(_id);

	}

	void connect::on_read_body_no_length_head(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			close();
			_callback(_id);
			return;
		}

		off += bytes_transferred;
		std::string str(&*_data.begin(), off);

		int pos = str.find("\r\n");
		if (pos == std::string::npos)
		{
			if (_skt)
				_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
			if (_ssl_skt)
				_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
			return;
		}

		std::string sizstr = str.substr(0, pos);
		int32 siz;
		Helper::hextoint32(sizstr, siz);
		if (siz == 0)
		{
			_callback(_id);
			return;
		}

		if ((int32)_data.size() < pos + 2 + siz)
		{
			_data.resize(pos + 2 + siz + 2);
		}

		if (_skt)
		_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
			std::bind(&connect::on_read_body_no_length_body, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		if (_ssl_skt)
			_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
			std::bind(&connect::on_read_body_no_length_body, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		
	}

	void connect::on_read_body_no_length_body(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			close();
			_callback(_id);
			return;
		}

		off += bytes_transferred;

		if (off < _data.size())
		{
			if (_skt)
			_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read_body_no_length_body, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

			if (_ssl_skt)
				_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read_body_no_length_body, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
			return;
		}


		std::string str(&*_data.begin(), off);
		int pos = str.find("\r\n");
		if (pos == std::string::npos)
		{
			if (_skt)
				_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

			if (_ssl_skt)
				_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
			return;
		}

		_response.body += str.substr(pos + 2, str.size() - pos - 4);
		off = 0;

		_data.resize(6);
		if (_skt)
		_skt->async_read_some(boost::asio::buffer(&*_data.begin(), _data.size()),
			std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

		if (_ssl_skt)
			_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin(), _data.size()),
			std::bind(&connect::on_read_body_no_length_head, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void connect::on_read(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{
			close();
			_callback(_id);
			return;
		}

		off += bytes_transferred;

		if (!is_receive_head())
		{
			if (_skt)
			_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off), 
				std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

			if (_ssl_skt)
				_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
				std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

			return;
		}

		size_t leng = get_length();
		if (leng != -1)
		{
			if (_data.size() < leng)
			{
				_data.resize(leng);
			}

			if (off - head_off < leng)
			{
				if (_skt)
				_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
					std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

				if (_ssl_skt)
					_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
					std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
				return;
			}

			_response.body = std::string(&*_data.begin() + head_off, off - head_off);
			_callback(_id);
		}
		else
		{


			if (!rfind(&*_data.begin(), off, "0\r\n\r\n", strlen("0\r\n\r\n")))
			{
				if (_skt)
				_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
					std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

				if (_ssl_skt)
					_ssl_skt->async_read_some(boost::asio::buffer(&*_data.begin() + off, _data.size() - off),
					std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

				return;
			}
			//memcmp(_data + off - strlen(_request.pend_flag), _request.pend_flag, strlen(_request.pend_flag)) == 0

			parse_body();
			_callback(_id);			
		}

		if (sizeof(_data) == off)
		{
			std::cout << "full stack\n";
			return;
		}

		//_skt->async_read_some(boost::asio::buffer(_data + off, sizeof(_data) - off), 
		//	std::bind(&connect::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
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
			ss << it->first << ": " << it->second << "\r\n";
		}
		ss << "\r\n";
		if (_request.head.size() == 0)
			ss << "\r\n";

		ss << _request.body;
		std::shared_ptr<std::string> str (new std::string(ss.str()));
		if (_skt)
		_skt->async_write_some(boost::asio::buffer(str->c_str(), str->size()), 
			std::bind(&connect::on_write, shared_from_this(), str, std::placeholders::_1, std::placeholders::_2));

		if (_ssl_skt)
			_ssl_skt->async_write_some(boost::asio::buffer(str->c_str(), str->size()),
			std::bind(&connect::on_write, shared_from_this(), str, std::placeholders::_1, std::placeholders::_2));
	}
	void connect::on_write(std::shared_ptr<std::string>& str, const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (e)
		{		
			close();
			_callback(_id);
			return;
		}

	}

	void connect::close()
	{
		if (_skt && !bclose)
		{
			_time_out.cancel();
			bclose = true;
			_skt->get_io_service().post(boost::bind(&connect::close_socket, shared_from_this()));
		}

		if (_ssl_skt && !bclose)
		{
			_time_out.cancel();
			bclose = true;
			_ssl_skt->get_io_service().post(boost::bind(&connect::close_socket, shared_from_this()));
		}
	}

	void connect::close_socket()
	{
		try{
			boost::system::error_code ignored_ec;
			if (_skt)
			{
				_skt->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
				_skt->close();
				_skt.reset();
			}

			if (_ssl_skt)
			{
				_ssl_skt->lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
				_ssl_skt->lowest_layer().close();
				_ssl_skt.reset();
			}
		}
		catch (...)
		{
			//Helper::dbgTrace("socket error when close socket");
		}
	}

	void connect::set_proxy(const std::string& ip, const std::string& port)
	{
		proxy_ip = ip;
		proxy_port = port;
	}

	bool connect::is_receive_head()
	{
		std::string res(&*_data.begin(), off);
		std::vector<std::string> res_vec;
		int32 pos = res.find("\r\n\r\n");

		if (pos == std::string::npos)
			return false;

		res_vec.push_back(res.substr(0, pos));
		head_off = pos + sizeof("\r\n\r\n") - 1;
		std::vector<std::string> head_vec;
		split(res_vec[0].c_str(), res_vec[0].size(), "\r\n", head_vec);
		if (head_vec.size() == 0)
			return false;
		std::vector<std::string> head_res;
		split(head_vec[0].c_str(), head_vec[0].size(), " ", head_res);
		if (head_res.size() < 3)
			return false;

		if (!head_ready)
		{
			head_ready = true;
			_response = http_response();
			_response.code = atoi(head_res[1].c_str());

			for (int32 i = 1; i < (int32)head_vec.size(); i++)
			{
				int32 pos = head_vec[i].find(":");
				if (pos == std::string::npos)
					continue;

				_response.head[head_vec[i].substr(0, pos)] = head_vec[i].substr(pos + 1);
			}

			_data.erase(_data.begin(), _data.begin() + head_off);
			if (_data.size() < 6)
			{
				_data.resize(6);
			}
		}

		return true;
	}

	size_t connect::get_length()
	{
		auto it = _response.head.find("Content-Length");
		if (it != _response.head.end())
			return atoi(it->second.c_str());

		it = _response.head.find("Content - Length");
		if (it != _response.head.end())
			return atoi(it->second.c_str());

		return -1;
	}

	void connect::parse_body()
	{
		std::string res(&*_data.begin() + head_off, off);
		_response.body = res;
	}
}

