 #include "Component/thread_pool/pthread_pool.h"
#include "Component/time/dtime.h"

#include <cstdio>

//静态成员初始化
pthread_mutex_t PthreadPool::m_mutex = PTHREAD_MUTEX_INITIALIZER;  //#define PTHREAD_MUTEX_INITIALIZER ((pthread_mutex_t) -1)
pthread_cond_t PthreadPool::m_cond = PTHREAD_COND_INITIALIZER;     //#define PTHREAD_COND_INITIALIZER ((pthread_cond_t) -1)
bool PthreadPool::m_bShutdown = false;    //刚开始标记整个线程池的线程是不退出的 

PthreadPool::PthreadPool()
{
    m_runningNum = 0; //原子对象可以直接赋值为0，这里就可以直接当成int变量进行使用
}

PthreadPool::~PthreadPool()
{
    ClearData();
}

bool PthreadPool::Create(int threadNum)
{
	ThreadContext* pNew;
	int err;

	m_threadNum = threadNum; //保存要创建的线程数量    

	for (int i = 0; i < m_threadNum; ++i)
	{
		m_vThreads.push_back(pNew = new ThreadContext(this));             //创建 一个新线程对象 并入到容器中         
		err = pthread_create(&pNew->_handle, NULL, ThreadFunc, pNew);      //创建线程，错误不返回到errno，一般返回错误码
		if (err != 0)
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

void PthreadPool::StopAll()
{
	//(1)已经调用过，就不要重复调用了
	if (m_bShutdown)
	{
		return;
	}
	m_bShutdown = true;

	//(2)唤醒等待该条件【卡在pthread_cond_wait()的】的所有线程，一定要在改变条件状态以后再给线程发信号
	int err = pthread_cond_broadcast(&m_cond);
	if (err != 0)
	{
		//这肯定是有问题，发出警告
		return;
	}

	//(3)等等线程，让线程真返回    
	for (auto iter = m_vThreads.begin(); iter != m_vThreads.end(); iter++)
	{
		//注意这里的join用法
		pthread_join((*iter)->_handle, NULL); //等待一个线程终止
	}

	//流程走到这里，那么所有的线程池中的线程肯定都返回了；
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_cond);

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

void PthreadPool::InData(int num)
{
	int err = pthread_mutex_lock(&m_mutex);
	if (err != 0)
	{
		//锁定失败如何处理……
	}

	m_data.push_back(num);

	err = pthread_mutex_unlock(&m_mutex);
	if (err != 0)
	{
		//解锁失败如何处理
	}

	//可以激发一个线程来干活了
	Call();
	return;
}

void PthreadPool::Call()
{
    int err = pthread_cond_signal(&m_cond); //唤醒一个等待该条件的线程，也就是可以唤醒卡在pthread_cond_wait()的线程
    if (err != 0)
    {
        //如果唤醒失败怎么处理
        //这里可以加一个周期处理，如果唤醒失败超过多少次，说明确实无法唤醒，那么对应进行处理也可以
    }

    if (m_threadNum == m_runningNum) //线程池中线程总量，跟当前正在干活的线程数量一样，说明所有线程都忙碌起来，线程不够用了
    {
        //线程不够用了，是否需要进行报告呢？
        //如果报告太频繁，也可以考虑加时间进行判断，比如线程不够用，每10秒进行一次报警？？？
    }

    return;
}

void* PthreadPool::ThreadFunc(void* threadData)
{
    //这个是静态成员函数，是不存在this指针的；
    ThreadContext* pThread = static_cast<ThreadContext*>(threadData);
    PthreadPool* pPool = pThread->_pPool;
    int err;

    pthread_t tid = pthread_self(); //获取线程自身id，以方便调试打印信息等    
    while (true)
    {
        //线程用pthread_mutex_lock()函数去锁定指定的mutex变量，若该mutex已经被另外一个线程锁定了，该调用将会阻塞线程直到mutex被解锁。  
        err = pthread_mutex_lock(&m_mutex);
        if (err != 0)
        {
            //线程锁上锁失败，信息报告
        }


        // 以下这行程序写法技巧十分重要，必须要用while这种写法，
        // 因为：pthread_cond_wait()是个值得注意的函数，调用一次pthread_cond_signal()可能会唤醒多个【惊群】
        //【官方描述是 至少一个/pthread_cond_signal 在多处理器上可能同时唤醒多个线程】
        // 和C++11的thread中的虚假唤醒是一个概念。
        // C++11thread中和pthread中的条件变量相关函数的对应关系
        // wait()       -- pthread_cond_wait()
        // nority_one() -- pthread_cond_signal()
        // notify_all() -- pthread_cond_broadcast()
        // pthread_cond_wait()函数，如果只有一条消息 唤醒了两个线程干活，那么其中有一个线程拿不到消息，
        // 那如果不用while写，就会出问题，所以被惊醒后必须再次用while拿消息，拿到才走下来；
        while ((pPool->m_data.size() == 0) && m_bShutdown == false)
        {
            //如果这个pthread_cond_wait被唤醒【被唤醒后程序执行流程往下走的前提是拿到了锁--官方：pthread_cond_wait()返回时，互斥量再次被锁住】，
             //那么会立即再次执行 你调用线程处的那个函数，比如给m_data进行添加数据的函数，如果拿到了一个NULL，则继续在这里wait着();
            if (pThread->_bRunning == false)
                pThread->_bRunning = true; //标记为true了才允许调用StopAll()：测试中发现如果Create()和StopAll()紧挨着调用，就会导致线程混乱，所以每个线程必须执行到这里，才认为是启动成功了；

            //刚开始执行pthread_cond_wait()的时候，会卡在这里，而且m_mutex会被释放掉；
            pthread_cond_wait(&m_cond, &m_mutex); //整个服务器程序刚初始化的时候，所有线程必然是卡在这里等待的；
        }

        //能走下来的，必然是拿到了真正的要处理的数据，或者 m_bShutdown == true
        //走到这里时刻，互斥量肯定是锁着的。。。。。。

        //先判断线程退出这个条件
        if (m_bShutdown)
        {
            pthread_mutex_unlock(&m_mutex); //解锁互斥量
            break;
        }

        //走到这里，可以取得数据进行处理了【m_data的size必然大于0】
        //此时互斥量仍然处于锁定中
        int tmp = pPool->m_data.front();     //返回第一个元素但不检查元素存在与否
        pPool->m_data.pop_front();           //移除第一个元素但不返回	

        //可以解锁互斥量了
        err = pthread_mutex_unlock(&m_mutex);
        if (err != 0)
        {
            //如果解锁失败了怎么处理
        }

        ++pPool->m_runningNum; //原子操作


        //这一行代码会根据实际的需求进行处理，即换成真正的线程处理任务，同样的m_data这个数据也对应更换为需要处理的东西……
        printf("do work!, data = %d\n", tmp);

        --pPool->m_runningNum; //线程到这里实际上就已经执行完了本次任务，那么他又可以作为一个空闲线程被调用了

    } //end while(true)

    //能走出来表示整个程序要结束啊，怎么判断所有线程都结束？
    return (void*)0;
}

void PthreadPool::ClearData()
{
    m_data.clear();
}