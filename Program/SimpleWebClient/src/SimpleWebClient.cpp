#include "SimpleWebClient.h"

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define sleep_ms(a) Sleep(a)
#else
#include <unistd.h>
#define sleep_ms(a) usleep((a) * 1000)
#endif

#include <curl/curl.h>

EasyCurlClient::EasyCurlClient() :
	m_bRunning(false), m_pThread(nullptr)
{
	curl_global_init(CURL_GLOBAL_ALL);
}

EasyCurlClient::~EasyCurlClient()
{
	Stop();
	if (!m_requesters.empty())
	{
		for (auto& p : m_requesters)
		{
			delete p;
		}
		m_requesters.clear();
	}
	curl_global_cleanup();
}

void EasyCurlClient::Start()
{
	if (!m_pThread)
	{
		m_bRunning = true;
		m_pThread = new std::thread(&EasyCurlClient::Run, this);
	}
}

void EasyCurlClient::Stop()
{
	if (m_pThread)
	{
		m_bRunning = false;
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}

void EasyCurlClient::SendHttpRequest(HttpRequester* p)
{
	if (p)
	{
		m_mutex.lock();
		m_requesters.push_back(p);
		m_mutex.unlock();
	}	
}

void EasyCurlClient::Run()
{
	HttpRequester* p = nullptr;
	while (m_bRunning)
	{
		p = nullptr;
		m_mutex.lock();
		if (!m_requesters.empty())
		{
			p = m_requesters.front();
			m_requesters.pop_front();
		}
		m_mutex.unlock();

		if (p)
		{
			DealCurlEasy(p);
			delete p;
		}
		sleep_ms(1);
	}
}

static size_t GetUrlResponse(void* buffer, size_t size, size_t count, void* response)
{
	std::string* str = (std::string*)response;
	(*str).append((char*)buffer, size * count);

	return size * count;
}

void EasyCurlClient::DealCurlEasy(HttpRequester* p)
{
	CURL* pCurl = curl_easy_init();
	if (pCurl)
	{
		struct curl_slist* header_list = nullptr;
		std::string response;

		auto& url = p->GetUrl();
		curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
		if (url.find("https://") != url.npos)
		{
			curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, false);
			curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, false);
		}

		auto& headers = p->GetHeaders();
		if (!headers.empty())
		{
			for (auto& header : headers)
			{
				std::string tmp = header.first + ": " + header.second;
				header_list = curl_slist_append(header_list, tmp.c_str());
			}
		}

		switch (p->GetMethod())
		{
		case HTTP_METHOD_GET:
		{
			curl_easy_setopt(pCurl, CURLOPT_POST, 0);
			break;
		}
		case HTTP_METHOD_POST:
		{
			curl_easy_setopt(pCurl, CURLOPT_POST, 1);
			curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, p->GetContent().c_str());
			break;
		}
		case HTTP_METHOD_PUT:
		{
			curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, p->GetContent().c_str());
			curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, "PUT");
			break;
		}
		case HTTP_METHOD_DELETE:
		{
			curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, "DELETE");
			break;
		}
		default:
			break;
		}

		curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 0);
		curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, nullptr);
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, GetUrlResponse);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)&response);
		curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);

		curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 10);
		curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10);

		CURLcode res = curl_easy_perform(pCurl);
		if (res != CURLE_OK)
		{
			response = "failed, err_msg: ";
			response += curl_easy_strerror(res);
		}
		else
		{
			//TODO
		}

		if (header_list)
		{
			curl_slist_free_all(header_list);
		}
	}
	curl_easy_cleanup(pCurl);
}