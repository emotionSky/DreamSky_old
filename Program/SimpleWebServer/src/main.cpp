#include "WebServer.h"
#include "Semaphore.h"

static Semaphore sem;

int main(int argc, char* argv[])
{
	StartWebServer(8080);
	int finish = 0;
	std::thread th(&WebThread, &finish);

	sem.Wait();

	finish = 1;
	th.join();
	StopWebServer();

	return 0;
}
