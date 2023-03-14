#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

bool StartWebServer(int port);
void StopWebServer();
void WebThread(int* finish);

#endif //!__WEB_SERVER_H__
