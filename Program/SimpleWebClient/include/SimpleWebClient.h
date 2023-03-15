#ifndef __SIMPLE_WEB_CLIENT_H__
#define __SIMPLE_WEB_CLIENT_H__

#include "HttpRequester.h"
#include <list>
#include <thread>
#include <mutex>

class EasyCurlClient
{
public:
	EasyCurlClient();
	~EasyCurlClient();

	void Start();
	void Stop();
	void SendHttpRequest(HttpRequester* p);

	void Run();

private:
	void DealCurlEasy(HttpRequester* p);

private:
	bool m_bRunning;
	std::mutex m_mutex;
	std::thread* m_pThread;
	std::list<HttpRequester*> m_requesters;
};

#endif //!__SIMPLE_WEB_CLIENT_H__