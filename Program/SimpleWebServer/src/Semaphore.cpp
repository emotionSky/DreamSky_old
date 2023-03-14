#include "Semaphore.h"

Semaphore::Semaphore(long count) : 
	m_count(count)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::Post()
{
	std::unique_lock<std::mutex> ul(m_mutex);
	++m_count;

	if (m_count <= 0)
	{
		m_cond.notify_one();
	}
}

void Semaphore::Wait()
{
	std::unique_lock<std::mutex> ul(m_mutex);
	--m_count;

	if (m_count < 0)
	{
		m_cond.wait(ul);
	}
}