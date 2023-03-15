#include "HttpRequester.h"
#ifdef _WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

std::string replace_str(std::string& src_str, const std::string& to_replaced, const std::string& newchars)
{
	std::string str = src_str;
	for (std::string::size_type pos(0); pos != std::string::npos; pos += newchars.length())
	{
		pos = str.find(to_replaced, pos);
		if (pos != std::string::npos)
		{
			str.replace(pos, to_replaced.length(), newchars);
		}
		else
		{
			break;
		}
	}
	return str;
}

std::string GetSeqUuid()
{
	std::string strRes;
	char szuuid[128] = { 0 };
#ifdef _WIN32
	GUID guid;
	CoCreateGuid(&guid);
	sprintf(
		szuuid,
		"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
#else
	uuid_t uuid;
	uuid_generate(uuid);
	uuid_unparse(uuid, szuuid);
#endif
	strRes = szuuid;
	strRes = replace_str(strRes, "-", "");
	return strRes;
}

HttpRequester::HttpRequester(const std::string& url, HttpRequestMethod method, const std::string& content):
	m_method(method), m_uuid(GetSeqUuid()), m_url(url), m_content(content)
{
	
}

HttpRequester::~HttpRequester()
{
}

void HttpRequester::AddHeader(const std::string& key, const std::string& value)
{
	m_headers[key] = value;
}