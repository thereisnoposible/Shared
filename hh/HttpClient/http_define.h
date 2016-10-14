#pragma once
#include <string>
#include <unordered_map>

namespace http
{
#define HTTP_GET "GET "
#define HTTP_POST "POST "
	typedef unsigned int uint32;
	typedef int int32;

	class http_request
	{
	public:
		const std::string vesion = " HTTP/1.1";
		std::string type;
		std::string url;
		std::string host;

		std::unordered_map<std::string, std::string> head;
		std::string body;
		char *pend_flag = NULL;
		http_request& operator =(const http_request& right)
		{
			type = right.type;
			url = right.url;
			host = right.host;
			head = right.head;
			body = right.body;
			pend_flag = right.pend_flag;
			return *this;
		}
	};

	class http_response
	{
	public:
		uint32 code;
		std::unordered_map<std::string, std::string> head;
		std::string body;
	};
}