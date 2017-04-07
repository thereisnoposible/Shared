#include <boost/asio.hpp>
#include "Client.h"
#include <thread>
#include "helper/Helper.h"
#include <boost/format.hpp>
#include <exception>  

//#include <openssl/ssl.h>
//#include <openssl/ossl_typ.h>

std::string chartohex(unsigned char p)
{
	char a[3] = { 0 };
	sprintf_s(a, "%X", p);
	return a;
}

bool bRun = true;
Client ios(std::string("ios"));
Client android(std::string("android"));
void func()
{
	while (1)
	{
		std::string cmd;
		std::string param;
		std::getline(std::cin, cmd);

		int pos = cmd.find(" ");
		if (pos != std::string::npos)
		{
			param = cmd.substr(pos + 1);
			cmd.erase(cmd.begin() + pos, cmd.end());
		}

		if (cmd == "as_login")
		{
			ios.fire(cmd, param);
			android.fire(cmd, param);
			continue;
		}

		std::string sub_param;
		pos = param.find(" ");
		if (pos != std::string::npos)
		{
			sub_param = param.substr(pos + 1);
			param.erase(param.begin() + pos, param.end());
		}

		if (cmd == "ios")
			ios.fire(param, sub_param);
		
		if (cmd == "android")
			android.fire(param, sub_param);
		if (cmd == "exit")
		{
			bRun = false;
			break;
		}
	}

}


//int getPages(const char* host_addr, const int host_port)
//{
//	WSADATA wsaData;
//	WSAStartup(MAKEWORD(2, 0), &wsaData);
//
//	SOCKET sockfd;
//	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
//	{
//		return -1;
//	}
//
//	struct sockaddr_in server_addr;
//	server_addr.sin_family = AF_INET;
//	server_addr.sin_port = htons(host_port);
//	server_addr.sin_addr.s_addr = inet_addr(host_addr);
//
//	if (connect(sockfd, (struct sockaddr *) (&server_addr), sizeof(struct sockaddr)) == -1)
//	{
//		return -1;
//	}
//
//	SSL *ssl;
//	SSL_CTX *ctx;
//	// SSL initialize
//	SSL_library_init();
//	SSL_load_error_strings();
//	ctx = SSL_CTX_new(SSLv23_client_method());
//	if (ctx == NULL)
//	{
//		return -1;
//	}
//
//	ssl = SSL_new(ctx);
//	if (ssl == NULL)
//	{
//		return -1;
//	}
//
//	// link socket & SSL
//	int ret = SSL_set_fd(ssl, sockfd);
//	if (ret == 0)
//	{
//		return -1;
//	}
//
//	//RAND_poll();
//	//while (RAND_status() == 0)
//	//{
//	//	unsigned short rand_ret = rand() % 65536;
//	//	RAND_seed(&rand_ret, sizeof(rand_ret));
//	//}
//
//	// SSL connect
//	ret = SSL_connect(ssl);
//	if (ret != 1)
//	{
//		return -1;
//	}
//
//	char getRequest[1024];
//	sprintf_s(getRequest, "GET %s HTTP/1.1\r\nHost: %s\r\nCookie: %s\r\n\r\n", "/", host_addr, "1234");
//
//	// send https request
//	int totalsend = 0;
//	int requestLen = strlen(getRequest);
//	while (totalsend < requestLen)
//	{
//		int send = SSL_write(ssl, getRequest + totalsend, requestLen - totalsend);
//		if (send == -1)
//		{
//
//		}
//		totalsend += send;
//	}
//
//	// receive https response
//	int responseLen = 0;
//	int i = 0;
//	char buffer[8];
//	memset(buffer, 0, 8);
//	char returnBuffer[1024];
//	memset(returnBuffer, 0, 1024);
//	int p = 0;
//	while ((responseLen = SSL_read(ssl, buffer, 1)) == 1 && p < 1024)
//	{
//		returnBuffer[p] = buffer[0];
//		p++;
//		if (i < 4)
//		{
//			if (buffer[0] == '\r' || buffer[0] == '\n')
//				i++;
//			else
//				i = 0;
//		}
//	}
//
//	// shutdown community 
//	ret = SSL_shutdown(ssl);
//	if (ret != 0)
//	{
//		return -1;
//	}
//	SSL_free(ssl);
//	SSL_CTX_free(ctx);
//	closesocket(sockfd);
//	WSACleanup();
//	return 0;
//}

int main()
{	
	std::thread pthread(func);

	std::chrono::steady_clock::time_point fLast = std::chrono::steady_clock::now();

	while (bRun)
	{
		std::chrono::steady_clock::time_point fCurr = std::chrono::steady_clock::now();
		std::chrono::duration<double> usetime = std::chrono::duration_cast<std::chrono::duration<double>>(fCurr - fLast);
		//在这里控制帧率
		if (usetime.count() < 0.00000000001)
		{
			Sleep(1);
			continue;
		}

		try
		{
			ios.run(usetime.count());
			android.run(usetime.count());
		}
		catch (...)
		{

		}
		
		fLast = fCurr;
	}
	pthread.join();
	return 0;
}