#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <mutex>
#include <condition_variable>

class Semaphore
{
public:
	Semaphore(long count = 0);
	~Semaphore();

	void Post();
	void Wait();

private:
	long                    m_count;
	std::mutex              m_mutex;
	std::condition_variable m_cond;
};

#endif //!__SEMAPHORE_H__