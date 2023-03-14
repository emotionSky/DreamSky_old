#include "WebServer.h"
#include <goahead.h>
#include <string>

void test_get_action(Webs* wp)
{
	std::string response = "test test get get.";
	websSetStatus(wp, 200);
	websWriteHeaders(wp, response.size(), 0);
	websWriteEndHeaders(wp);
	websWrite(wp, "%s", response.c_str());
	websDone(wp);
}

void test_post_action(Webs* wp)
{
	size_t len = wp->input.endp - wp->input.buf;
	char* buf = new char[len + 1];
	memcpy(buf, wp->input.buf, len);
	buf[len] = '\0';

	printf("recv post:\n%s\n", buf);
	delete[] buf;

	std::string response = "test test post post.";
	websSetStatus(wp, 200);
	websWriteHeaders(wp, response.size(), 0);
	websWriteEndHeaders(wp);
	websWrite(wp, "%s", response.c_str());
	websDone(wp);
}

bool StartWebServer(int port)
{
	if (websOpen("./web", "route.txt") < 0)
	{
		return false;
	}

	WebsHash hash = websGetRoles();
	websAddRole("administrator", hash);

	char buf[256];
	sprintf(buf, "Admin:example.com:%s", "admin");
	char* temp = websMD5(buf);
	static char password[256];
	strcpy(password, temp);
	wfree(temp);

	websAddUser("Admin", password, "administrator");

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "https://:%d", port);
	if (websListen(buf) < 0)
	{
		return false;
	}

	websSetIndex((char*)"index.html");
	websDefineAction("test_get", (void*)test_get_action);
	websDefineAction("test_post", (void*)test_post_action);
	return true;
}

void StopWebServer()
{
	websClose();
}

void WebThread(int* finish)
{
	websServiceEvents(finish);
}