#include <Component/thread_pool/std_thread_pool.h>
#include <Component/general/dream_time.h>
#include <cstdio>

//静态成员初始化
std::mutex StdThreadPool::m_mutex;
std::condition_variable StdThreadPool::m_cond;
bool StdThreadPool::m_bShutdown = false;    //刚开始标记整个线程池的线程是不退出的 

StdThreadPool::StdThreadPool()
{
	m_runningNum = 0; //原子对象可以直接赋值为0，这里就可以直接当成int变量进行使用
}

StdThreadPool::~StdThreadPool()
{
	ClearData();
}

bool StdThreadPool::Create(int threadNum)
{
	ThreadContext* pNew;
	m_threadNum = threadNum; //保存要创建的线程数量    

	for (int i = 0; i < m_threadNum; ++i)
	{
		m_vThreads.push_back(pNew = new ThreadContext(this)); //创建 一个新线程对象 并入到容器中         
		pNew->_handle = new std::thread(ThreadFunc, pNew);
		if (pNew->_handle == nullptr)
		{
			//创建线程有错
			return false;
		}
		else
		{
			//创建线程成功
		}
	}

	//我们必须保证每个线程都启动并运行到pthread_cond_wait()，本函数才返回，只有这样，这几个线程才能进行后续的正常工作 
	std::vector<ThreadContext*>::iterator iter;
lblfor:
	for (iter = m_vThreads.begin(); iter != m_vThreads.end(); ++iter)
	{
		if ((*iter)->_bRunning == false) //这个条件保证所有线程完全启动起来，以保证整个线程池中的线程正常工作；
		{
			//这说明有没有启动完全的线程
			SleepMs(100);
			goto lblfor;
		}
	}
	return true;
}

void StdThreadPool::StopAll()
{
	//(1)已经调用过，就不要重复调用了
	if (m_bShutdown)
	{
		return;
	}
	m_bShutdown = true;

	//(2)唤醒等待该条件的所有线程，一定要在改变条件状态以后再给线程发信号
	m_cond.notify_all();

	//(3)等等线程，让线程真返回    
	for (auto iter = m_vThreads.begin(); iter != m_vThreads.end(); iter++)
	{
		//注意这里的join用法
		(*iter)->_handle->join(); //等待一个线程终止
	}

	//(4)释放一下new出来的ThreadItem【线程池中的线程】    
	for (auto iter = m_vThreads.begin(); iter != m_vThreads.end(); iter++)
	{
		if (*iter)
		{
			delete* iter;
		}
	}
	m_vThreads.clear();
	return;
}

void StdThreadPool::InData(int num)
{
	m_mutex.lock();
	m_data.push_back(num);
	m_mutex.unlock();

	//可以激发一个线程来干活了
	Call();
	return;
}

void StdThreadPool::Call()
{
	m_cond.notify_one();

	if (m_threadNum == m_runningNum) //线程池中线程总量，跟当前正在干活的线程数量一样，说明所有线程都忙碌起来，线程不够用了
	{
		//线程不够用了，是否需要进行报告呢？
		//如果报告太频繁，也可以考虑加时间进行判断，比如线程不够用，每10秒进行一次报警？？？
	}

	return;
}

void StdThreadPool::ThreadFunc(void* threadData)
{
	//这个是静态成员函数，是不存在this指针的；
	ThreadContext* pThread = static_cast<ThreadContext*>(threadData);
	StdThreadPool* pPool = pThread->_pPool;
	std::thread::id tid = std::this_thread::get_id();

	while (true)
	{
		std::unique_lock<std::mutex> sbguard1(m_mutex); //临界进去
		//wait()用于等一个东西
		//如果wait()第二个参数的lambda表达式返回的是true，wait就直接返回
		//如果wait()第二个参数的lambda表达式返回的是false,那么wait()将解锁互斥量,并堵塞到这行，那堵到什么时候为止呢？堵到其他某个线程调用notify_one()通知为止
		//如果wait()不用第二个参数，那跟第二个参数为lambda表达式并且返回false效果 一样（解锁互斥量,并堵塞到这行，堵到其他某个线程调用notify_one()通知为止）
		m_cond.wait(sbguard1, [&] {
			if (!pPool->m_data.empty())
				return true;
			return false;
			});

		if (m_bShutdown)
		{
			sbguard1.unlock(); //解锁互斥量
			break;
		}

		//现在互斥量是锁着的，流程走下来意味着msgRecvQueue队列里必然有数据
		int tmp = pPool->m_data.front();     //返回第一个元素但不检查元素存在与否
		pPool->m_data.pop_front();           //移除第一个元素但不返回	
		sbguard1.unlock(); //因为unique_lock的灵活性，我们可以随时unlock解锁，以免锁住太长时间

		++pPool->m_runningNum; //原子操作

		//这一行代码会根据实际的需求进行处理，即换成真正的线程处理任务，同样的m_data这个数据也对应更换为需要处理的东西……
		printf("do work!, data = %d\n", tmp);

		--pPool->m_runningNum; //线程到这里实际上就已经执行完了本次任务，那么他又可以作为一个空闲线程被调用了

	} //end while
}

void StdThreadPool::ClearData()
{
	m_data.clear();
}