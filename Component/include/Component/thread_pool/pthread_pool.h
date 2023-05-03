#if 0
#ifndef __PTHREAD_POOL_H__
#define __PTHREAD_POOL_H__

#include <Component/dream_component.h>
#include <vector>
#include <list>
#include <atomic>
#include <pthread.h>

class DREAMSKY_API PthreadPool
{
public:
	PthreadPool();
	~PthreadPool();

	bool Create(int threadNum); //创建该线程池中的所有线程
	void StopAll(); //使线程池中的所有线程退出
	void Call();//来任务了，调一个线程池中的线程下来干活 

	//这里是给m_data添加数据的，对应的需要更换为实际的处理
	void InData(int num); //收到一个完整消息后，入消息队列，并触发线程池中线程来处理该消息

private:
	static void* ThreadFunc(void* threadData); //线程函数

	//这里是清理m_data中的数据的，对应的需要更换为实际的处理
	void ClearData();

private:
	//定义线程结构，用来统计线程信息
	struct ThreadContext
	{
	public:
		ThreadContext(PthreadPool* pPool) :_pPool(pPool), _bRunning(false) {}
		~ThreadContext() {}

		bool         _bRunning; //标记是否正式启动起来，启动起来后，才允许调用StopAll()来释放	
		pthread_t    _handle;   //线程句柄
		PthreadPool* _pPool;  //记录线程池的指针	
	};

private:
	static pthread_mutex_t       m_mutex;      //线程同步互斥量/也叫线程同步锁
	static pthread_cond_t        m_cond;       //线程同步条件变量
	static bool                  m_bShutdown;  //线程退出标志，false不退出，true退出
	int                          m_threadNum;  //要创建的线程数量
	std::atomic<int>             m_runningNum; //线程数, 运行中的线程数，原子操作
	std::vector<ThreadContext*>  m_vThreads;   //线程 容器，容器里就是各个线程了

	//自定义数据相关，这里仅仅是为了实现线程池的示例程序
	std::list<int>               m_data;
};

//唤醒丢失问题，sem_t sem_write;
//参考信号量解决方案：https://blog.csdn.net/yusiguyuan/article/details/20215591  linux多线程编程--信号量和条件变量 唤醒丢失事件

#endif //!__PTHREAD_POOL_H__
#endif
