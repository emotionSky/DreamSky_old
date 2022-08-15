 #include "Component/thread_pool/pthread_pool.h"
#include "Component/time/dtime.h"

#include <cstdio>

//��̬��Ա��ʼ��
pthread_mutex_t PthreadPool::m_mutex = PTHREAD_MUTEX_INITIALIZER;  //#define PTHREAD_MUTEX_INITIALIZER ((pthread_mutex_t) -1)
pthread_cond_t PthreadPool::m_cond = PTHREAD_COND_INITIALIZER;     //#define PTHREAD_COND_INITIALIZER ((pthread_cond_t) -1)
bool PthreadPool::m_bShutdown = false;    //�տ�ʼ��������̳߳ص��߳��ǲ��˳��� 

PthreadPool::PthreadPool()
{
    m_runningNum = 0; //ԭ�Ӷ������ֱ�Ӹ�ֵΪ0������Ϳ���ֱ�ӵ���int��������ʹ��
}

PthreadPool::~PthreadPool()
{
    ClearData();
}

bool PthreadPool::Create(int threadNum)
{
	ThreadContext* pNew;
	int err;

	m_threadNum = threadNum; //����Ҫ�������߳�����    

	for (int i = 0; i < m_threadNum; ++i)
	{
		m_vThreads.push_back(pNew = new ThreadContext(this));             //���� һ�����̶߳��� ���뵽������         
		err = pthread_create(&pNew->_handle, NULL, ThreadFunc, pNew);      //�����̣߳����󲻷��ص�errno��һ�㷵�ش�����
		if (err != 0)
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

void PthreadPool::StopAll()
{
	//(1)�Ѿ����ù����Ͳ�Ҫ�ظ�������
	if (m_bShutdown)
	{
		return;
	}
	m_bShutdown = true;

	//(2)���ѵȴ�������������pthread_cond_wait()�ġ��������̣߳�һ��Ҫ�ڸı�����״̬�Ժ��ٸ��̷߳��ź�
	int err = pthread_cond_broadcast(&m_cond);
	if (err != 0)
	{
		//��϶��������⣬��������
		return;
	}

	//(3)�ȵ��̣߳����߳��淵��    
	for (auto iter = m_vThreads.begin(); iter != m_vThreads.end(); iter++)
	{
		//ע�������join�÷�
		pthread_join((*iter)->_handle, NULL); //�ȴ�һ���߳���ֹ
	}

	//�����ߵ������ô���е��̳߳��е��߳̿϶��������ˣ�
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_cond);

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

void PthreadPool::InData(int num)
{
	int err = pthread_mutex_lock(&m_mutex);
	if (err != 0)
	{
		//����ʧ����δ�����
	}

	m_data.push_back(num);

	err = pthread_mutex_unlock(&m_mutex);
	if (err != 0)
	{
		//����ʧ����δ���
	}

	//���Լ���һ���߳����ɻ���
	Call();
	return;
}

void PthreadPool::Call()
{
    int err = pthread_cond_signal(&m_cond); //����һ���ȴ����������̣߳�Ҳ���ǿ��Ի��ѿ���pthread_cond_wait()���߳�
    if (err != 0)
    {
        //�������ʧ����ô����
        //������Լ�һ�����ڴ����������ʧ�ܳ������ٴΣ�˵��ȷʵ�޷����ѣ���ô��Ӧ���д���Ҳ����
    }

    if (m_threadNum == m_runningNum) //�̳߳����߳�����������ǰ���ڸɻ���߳�����һ����˵�������̶߳�æµ�������̲߳�������
    {
        //�̲߳������ˣ��Ƿ���Ҫ���б����أ�
        //�������̫Ƶ����Ҳ���Կ��Ǽ�ʱ������жϣ������̲߳����ã�ÿ10�����һ�α���������
    }

    return;
}

void* PthreadPool::ThreadFunc(void* threadData)
{
    //����Ǿ�̬��Ա�������ǲ�����thisָ��ģ�
    ThreadContext* pThread = static_cast<ThreadContext*>(threadData);
    PthreadPool* pPool = pThread->_pPool;
    int err;

    pthread_t tid = pthread_self(); //��ȡ�߳�����id���Է�����Դ�ӡ��Ϣ��    
    while (true)
    {
        //�߳���pthread_mutex_lock()����ȥ����ָ����mutex����������mutex�Ѿ�������һ���߳������ˣ��õ��ý��������߳�ֱ��mutex��������  
        err = pthread_mutex_lock(&m_mutex);
        if (err != 0)
        {
            //�߳�������ʧ�ܣ���Ϣ����
        }


        // �������г���д������ʮ����Ҫ������Ҫ��while����д����
        // ��Ϊ��pthread_cond_wait()�Ǹ�ֵ��ע��ĺ���������һ��pthread_cond_signal()���ܻỽ�Ѷ������Ⱥ��
        //���ٷ������� ����һ��/pthread_cond_signal �ڶദ�����Ͽ���ͬʱ���Ѷ���̡߳�
        // ��C++11��thread�е���ٻ�����һ�����
        // C++11thread�к�pthread�е�����������غ����Ķ�Ӧ��ϵ
        // wait()       -- pthread_cond_wait()
        // nority_one() -- pthread_cond_signal()
        // notify_all() -- pthread_cond_broadcast()
        // pthread_cond_wait()���������ֻ��һ����Ϣ �����������̸߳ɻ��ô������һ���߳��ò�����Ϣ��
        // ���������whileд���ͻ�����⣬���Ա����Ѻ�����ٴ���while����Ϣ���õ�����������
        while ((pPool->m_data.size() == 0) && m_bShutdown == false)
        {
            //������pthread_cond_wait�����ѡ������Ѻ����ִ�����������ߵ�ǰ�����õ�����--�ٷ���pthread_cond_wait()����ʱ���������ٴα���ס����
             //��ô�������ٴ�ִ�� ������̴߳����Ǹ������������m_data����������ݵĺ���������õ���һ��NULL�������������wait��();
            if (pThread->_bRunning == false)
                pThread->_bRunning = true; //���Ϊtrue�˲��������StopAll()�������з������Create()��StopAll()�����ŵ��ã��ͻᵼ���̻߳��ң�����ÿ���̱߳���ִ�е��������Ϊ�������ɹ��ˣ�

            //�տ�ʼִ��pthread_cond_wait()��ʱ�򣬻Ῠ���������m_mutex�ᱻ�ͷŵ���
            pthread_cond_wait(&m_cond, &m_mutex); //��������������ճ�ʼ����ʱ�������̱߳�Ȼ�ǿ�������ȴ��ģ�
        }

        //���������ģ���Ȼ���õ���������Ҫ��������ݣ����� m_bShutdown == true
        //�ߵ�����ʱ�̣��������϶������ŵġ�����������

        //���ж��߳��˳��������
        if (m_bShutdown)
        {
            pthread_mutex_unlock(&m_mutex); //����������
            break;
        }

        //�ߵ��������ȡ�����ݽ��д����ˡ�m_data��size��Ȼ����0��
        //��ʱ��������Ȼ����������
        int tmp = pPool->m_data.front();     //���ص�һ��Ԫ�ص������Ԫ�ش������
        pPool->m_data.pop_front();           //�Ƴ���һ��Ԫ�ص�������	

        //���Խ�����������
        err = pthread_mutex_unlock(&m_mutex);
        if (err != 0)
        {
            //�������ʧ������ô����
        }

        ++pPool->m_runningNum; //ԭ�Ӳ���


        //��һ�д�������ʵ�ʵ�������д����������������̴߳�������ͬ����m_data�������Ҳ��Ӧ����Ϊ��Ҫ����Ķ�������
        printf("do work!, data = %d\n", tmp);

        --pPool->m_runningNum; //�̵߳�����ʵ���Ͼ��Ѿ�ִ�����˱���������ô���ֿ�����Ϊһ�������̱߳�������

    } //end while(true)

    //���߳�����ʾ��������Ҫ����������ô�ж������̶߳�������
    return (void*)0;
}

void PthreadPool::ClearData()
{
    m_data.clear();
}