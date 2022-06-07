#include "Component/thread_pool/std_thread_pool.h"
#include "Component/time/dtime.h"

#include <cstdio>

//��̬��Ա��ʼ��
std::mutex StdThreadPool::m_mutex;
std::condition_variable StdThreadPool::m_cond;
bool StdThreadPool::m_bShutdown = false;    //�տ�ʼ��������̳߳ص��߳��ǲ��˳��� 

StdThreadPool::StdThreadPool()
{
	m_runningNum = 0; //ԭ�Ӷ������ֱ�Ӹ�ֵΪ0������Ϳ���ֱ�ӵ���int��������ʹ��
}

StdThreadPool::~StdThreadPool()
{
	ClearData();
}

bool StdThreadPool::Create(int threadNum)
{
	ThreadContext* pNew;
	m_threadNum = threadNum; //����Ҫ�������߳�����    

	for (int i = 0; i < m_threadNum; ++i)
	{
		m_vThreads.push_back(pNew = new ThreadContext(this)); //���� һ�����̶߳��� ���뵽������         
		pNew->_handle = new std::thread(ThreadFunc, pNew);
		if (pNew->_handle == nullptr)
		{
			//�����߳��д�
			return false;
		}
		else
		{
			//�����̳߳ɹ�
		}
	}

	//���Ǳ��뱣֤ÿ���̶߳����������е�pthread_cond_wait()���������ŷ��أ�ֻ���������⼸���̲߳��ܽ��к������������� 
	std::vector<ThreadContext*>::iterator iter;
lblfor:
	for (iter = m_vThreads.begin(); iter != m_vThreads.end(); ++iter)
	{
		if ((*iter)->_bRunning == false) //���������֤�����߳���ȫ�����������Ա�֤�����̳߳��е��߳�����������
		{
			//��˵����û��������ȫ���߳�
			SleepMs(100);
			goto lblfor;
		}
	}
	return true;
}

void StdThreadPool::StopAll()
{
	//(1)�Ѿ����ù����Ͳ�Ҫ�ظ�������
	if (m_bShutdown)
	{
		return;
	}
	m_bShutdown = true;

	//(2)���ѵȴ��������������̣߳�һ��Ҫ�ڸı�����״̬�Ժ��ٸ��̷߳��ź�
	m_cond.notify_all();

	//(3)�ȵ��̣߳����߳��淵��    
	for (auto iter = m_vThreads.begin(); iter != m_vThreads.end(); iter++)
	{
		//ע�������join�÷�
		(*iter)->_handle->join(); //�ȴ�һ���߳���ֹ
	}

	//(4)�ͷ�һ��new������ThreadItem���̳߳��е��̡߳�    
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

	//���Լ���һ���߳����ɻ���
	Call();
	return;
}

void StdThreadPool::Call()
{
	m_cond.notify_one();

	if (m_threadNum == m_runningNum) //�̳߳����߳�����������ǰ���ڸɻ���߳�����һ����˵�������̶߳�æµ�������̲߳�������
	{
		//�̲߳������ˣ��Ƿ���Ҫ���б����أ�
		//�������̫Ƶ����Ҳ���Կ��Ǽ�ʱ������жϣ������̲߳����ã�ÿ10�����һ�α���������
	}

	return;
}

void StdThreadPool::ThreadFunc(void* threadData)
{
	//����Ǿ�̬��Ա�������ǲ�����thisָ��ģ�
	ThreadContext* pThread = static_cast<ThreadContext*>(threadData);
	StdThreadPool* pPool = pThread->_pPool;
	std::thread::id tid = std::this_thread::get_id();

	while (true)
	{
		std::unique_lock<std::mutex> sbguard1(m_mutex); //�ٽ��ȥ
		//wait()���ڵ�һ������
		//���wait()�ڶ���������lambda���ʽ���ص���true��wait��ֱ�ӷ���
		//���wait()�ڶ���������lambda���ʽ���ص���false,��ôwait()������������,�����������У��Ƕµ�ʲôʱ��Ϊֹ�أ��µ�����ĳ���̵߳���notify_one()֪ͨΪֹ
		//���wait()���õڶ����������Ǹ��ڶ�������Ϊlambda���ʽ���ҷ���falseЧ�� һ��������������,�����������У��µ�����ĳ���̵߳���notify_one()֪ͨΪֹ��
		m_cond.wait(sbguard1, [&] {
			if (!pPool->m_data.empty())
				return true;
			return false;
			});

		if (m_bShutdown)
		{
			sbguard1.unlock(); //����������
			break;
		}

		//���ڻ����������ŵģ�������������ζ��msgRecvQueue�������Ȼ������
		int tmp = pPool->m_data.front();     //���ص�һ��Ԫ�ص������Ԫ�ش������
		pPool->m_data.pop_front();           //�Ƴ���һ��Ԫ�ص�������	
		sbguard1.unlock(); //��Ϊunique_lock������ԣ����ǿ�����ʱunlock������������ס̫��ʱ��

		++pPool->m_runningNum; //ԭ�Ӳ���

		//��һ�д�������ʵ�ʵ�������д����������������̴߳�������ͬ����m_data�������Ҳ��Ӧ����Ϊ��Ҫ����Ķ�������
		printf("do work!, data = %d\n", tmp);

		--pPool->m_runningNum; //�̵߳�����ʵ���Ͼ��Ѿ�ִ�����˱���������ô���ֿ�����Ϊһ�������̱߳�������

	} //end while
}

void StdThreadPool::ClearData()
{
	m_data.clear();
}