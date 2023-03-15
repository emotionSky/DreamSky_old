#ifndef __HTTP_REQUESTER_H__
#define __HTTP_REQUESTER_H__

#include <string>
#include <map>

enum HttpRequestMethod
{
	HTTP_METHOD_GET,
	HTTP_METHOD_POST,
	HTTP_METHOD_PUT,
	HTTP_METHOD_DELETE,
};

class HttpRequester
{
public:
	HttpRequester(const std::string& url, HttpRequestMethod method = HTTP_METHOD_GET, const std::string& content = "");
	~HttpRequester();

	void AddHeader(const std::string& key, const std::string& value);

	HttpRequestMethod GetMethod() const { return m_method; }
	const std::string& GetUuid() const { return m_uuid; }
	const std::string& GetUrl() const { return m_url; }
	const std::string& GetContent() const { return m_content; }
	const std::map<std::string, std::string>& GetHeaders() const { return m_headers; }

private:
	HttpRequestMethod m_method;
	std::string m_uuid;
	std::string m_url;
	std::string m_content;
	std::map<std::string, std::string> m_headers;
};

#endif //!__HTTP_REQUESTER_H__