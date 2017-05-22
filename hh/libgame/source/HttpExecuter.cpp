#include "pch.h"
#include "HttpExecuter.h"

template<>  HttpExecuter* Singleton<HttpExecuter>::s_pInstance = NULL;

//-------------------------------------------------------------------------------------------
HttpExecuter::HttpExecuter(int32 threadcnt) :m_bQuit(false)
{
	//m_pThread = new boost::thread(boost::bind(&HttpExecuter::workThreadFun, this));
	m_HttpClientCount = threadcnt;		//默认有两个线程进行处理
	if (m_HttpClientCount <= 0)
	{
		m_HttpClientCount = 1;
	}
}

//-------------------------------------------------------------------------------------------
HttpExecuter::~HttpExecuter()
{
	//关闭连接
	//close();

	//等待线程退出
	//m_pThread->join();

	//SafeDelete(m_pThread);
	m_HttpClientCollect.clear();
}

//---------------------------------------------------------------------------------------------
void HttpExecuter::start()
{
	for (int32 i = 0; i < m_HttpClientCount; i++)
	{
		HTTPClientPtr pHttpClient = HTTPClientPtr(new HTTPClient);
		m_HttpClientCollect.push_back(pHttpClient);
	}
}

//-------------------------------------------------------------------
void HttpExecuter::close()
{
	m_bQuit = true;

	m_Condition.notify_all();
}


//-------------------------------------------------------------------------------------------
void HttpExecuter::workThreadFun()
{
	//循环处理队列
	while (!m_bQuit)
	{
		boost::mutex::scoped_lock oLock(m_PackMutex);
		size_t size = _httpCommands.size();
		if (0 == size)
		{
			m_Condition.wait(oLock);
			continue;
		}

		//有数据,取出一条数据，并进行操作
		HttpCommandPtr pCommand = _httpCommands.front();
		_httpCommands.pop_front();
		oLock.unlock();

		time_t tmCurrent = time(NULL);
		CHECKERRORANDCONTINUE(tmCurrent - pCommand->tmCreateTime <= 10);

		HTTPClient http;
		//已经取到一条命令，执行它
		switch (pCommand->_type)
		{
		case enGet:
			break;
		case enPost:
			http.post(pCommand->req, pCommand->res, pCommand->data, pCommand->result);
			break;
		default:
			break;
		}
		if (pCommand->res.code != HTTPRESPONSE_000)
		{
			pushResult(pCommand);
		}
	}
}

//-------------------------------------------------------------------------------------------
void HttpExecuter::get(const xstring& url, HttpCallBack cb)
{
	HttpCommandPtr cmd = HttpCommandPtr(new HttpCommand());
	cmd->callb = cb;
	cmd->_type = enGet;
	cmd->req.url = url;

	addCommand(cmd);
}

//-------------------------------------------------------------------------------------------
void HttpExecuter::post(const xstring& url, const xstring& data, HttpCallBack cb)
{
	HttpCommandPtr cmd = HttpCommandPtr(new HttpCommand());
	cmd->callb = cb;
	cmd->_type = enPost;
	cmd->req.url = url;
	cmd->data = data;
	std::stringstream ss;
	ss << data.length();
	cmd->req.m_headMap["Content-Length"] =ss.str();

	addCommand(cmd);
}

//-------------------------------------------------------------------------------------------
void HttpExecuter::addCommand(HttpCommandPtr& pCommand)
{
	boost::mutex::scoped_lock oLock(m_PackMutex);
	if (_httpCommands.size() >= 100000)
	{
		for (int32 i = 0; i < 1000; i++)
		{//删除1000个
			_httpCommands.pop_front();
		}
	}
	_httpCommands.push_back(pCommand);
	oLock.unlock();

	////通知处理线程处理命令
	//m_Condition.notify_all();
}


//-------------------------------------------------------------------
bool HttpExecuter::popCommand(HttpCommandPtr& pCommand)
{
	bool bGet = false;
	boost::mutex::scoped_lock oLock(m_PackMutex);
	if (_httpCommands.size() > 0)
	{
		bGet = true;
		pCommand = _httpCommands.front();
		_httpCommands.pop_front();
	}
	
	oLock.unlock();

	return bGet;
}

//-------------------------------------------------------------------------------------------
void HttpExecuter::pushResult(HttpCommandPtr& pCommand)
{
	boost::mutex::scoped_lock oLock(m_ResultMutex);
	_resultCommands.push_back(pCommand);
}

//-------------------------------------------------------------------------------------------
void HttpExecuter::update()
{
	HttpCommandPtr pCommand;
	boost::mutex::scoped_lock oLock(m_ResultMutex);
	HttpCommandVector tmpResult = _resultCommands;
	_resultCommands.clear();
	oLock.unlock();

	HttpCommandVector::iterator it = tmpResult.begin();
	for (; it!=tmpResult.end(); it++)
	{
		pCommand = *it;

		if (pCommand.get() != NULL)
		{//取到内容
			pCommand->callb(pCommand);
		}
	}
}
