#include "Component/time/dtime.h"

#ifdef WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#endif // WIN32

void SleepUs(unsigned usec)
{
#ifdef WIN32
	HANDLE timer;
	LARGE_INTEGER ft;

	ft.QuadPart = -(10 * (int64_t)usec);

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
#else
	usleep(usec);
#endif
}

void SleepMs(unsigned msec)
{
#ifdef WIN32
	Sleep(msec);
#else
	usleep(msec * 1000);
#endif
}

uint64_t GetCurrentMs()
{
	uint64_t time = 0;

#if defined(WIN32) || defined(_WINDLL)
	static LARGE_INTEGER freq = { 0 };
	LARGE_INTEGER count;
	if (freq.QuadPart == 0)
	{
		if (QueryPerformanceFrequency(&freq))
		{
		}
	}
	QueryPerformanceCounter(&count);
	time = (uint64_t)(count.QuadPart
		/ ((double)freq.QuadPart
			/ 1000));
#else
	struct timespec count;
	if (clock_gettime(CLOCK_MONOTONIC, &count) < 0)
	{
		return 0;
	}
	time = (uint64_t)count.tv_sec * 1000 + count.tv_nsec / 1000000.0;
#endif

	return time;
}