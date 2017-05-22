#pragma once

#include "HttpClient.h"
#include "Singleton.h"

struct HttpCommand;
typedef boost::shared_ptr<HttpCommand> HttpCommandPtr;

typedef boost::function<void(const HttpCommandPtr&)> HttpCallBack;

enum HttpRequestType
{
	enGet = 0,
	enPost,
};

struct HttpCommand
{
	HttpCommand()
	{
		tmCreateTime = time(NULL);
	}

	HttpRequestType _type;	//http��������
	HTTPRequest req;		//http����
	HTTPResponse res;		//http��Ӧ
	HttpCallBack callb;		//�ص�
	xstring data;			//post����
	xstring result;			//http���������صĵ�����

	time_t tmCreateTime;
};


typedef xlist<HttpCommandPtr> HttpCommandVector;

class XClass HttpExecuter : public Singleton<HttpExecuter>
{
public:
	HttpExecuter(int32 threadcnt);
	~HttpExecuter();

public:
	void start();

	void get(const xstring& url, HttpCallBack cb);

	void post(const xstring& url, const xstring& data, HttpCallBack cb);

	void update();

	void pushResult(HttpCommandPtr& pCommand);

	bool popCommand(HttpCommandPtr& pCommand);

protected:
	void workThreadFun();

	void addCommand(HttpCommandPtr& pCommand);

	void close();

private:
	HttpCommandVector _httpCommands;

	HttpCommandVector _resultCommands;

	boost::mutex			m_PackMutex;			/** Http���󻥳Ᵽ������ */

	boost::mutex			m_ResultMutex;			/** Http������Ᵽ������ */

	boost::condition_variable m_Condition;

	//boost::thread*		m_pThread;				/** �����߳� */

	bool m_bQuit;

	HTTPClientCollect m_HttpClientCollect;

	int32 m_HttpClientCount;
};

