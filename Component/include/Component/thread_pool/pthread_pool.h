#ifndef __PTHREAD_POOL_H__
#define __PTHREAD_POOL_H__

#include "common_type.h"
#include <vector>
#include <list>
#include <atomic>
#include <pthread.h>

class DREAMSKY_API PthreadPool
{
public:
	PthreadPool();
	~PthreadPool();

	bool Create(int threadNum); //�������̳߳��е������߳�
	void StopAll(); //ʹ�̳߳��е������߳��˳�
	void Call();//�������ˣ���һ���̳߳��е��߳������ɻ� 

	//�����Ǹ�m_data������ݵģ���Ӧ����Ҫ����Ϊʵ�ʵĴ���
	void InData(int num); //�յ�һ��������Ϣ������Ϣ���У��������̳߳����߳����������Ϣ

private:
	static void* ThreadFunc(void* threadData); //�̺߳���

	//����������m_data�е����ݵģ���Ӧ����Ҫ����Ϊʵ�ʵĴ���
	void ClearData();

private:
	//�����߳̽ṹ������ͳ���߳���Ϣ
	struct ThreadContext
	{
	public:
		ThreadContext(PthreadPool* pPool) :_pPool(pPool), _bRunning(false) {}
		~ThreadContext() {}

		bool         _bRunning; //����Ƿ���ʽ�������������������󣬲��������StopAll()���ͷ�	
		pthread_t    _handle;   //�߳̾��
		PthreadPool* _pPool;  //��¼�̳߳ص�ָ��	
	};

private:
	static pthread_mutex_t       m_mutex;      //�߳�ͬ��������/Ҳ���߳�ͬ����
	static pthread_cond_t        m_cond;       //�߳�ͬ����������
	static bool                  m_bShutdown;  //�߳��˳���־��false���˳���true�˳�
	int                          m_threadNum;  //Ҫ�������߳�����
	std::atomic<int>             m_runningNum; //�߳���, �����е��߳�����ԭ�Ӳ���
	std::vector<ThreadContext*>  m_vThreads;   //�߳� ��������������Ǹ����߳���

	//�Զ���������أ����������Ϊ��ʵ���̳߳ص�ʾ������
	std::list<int>               m_data;
};

//���Ѷ�ʧ���⣬sem_t sem_write;
//�ο��ź������������https://blog.csdn.net/yusiguyuan/article/details/20215591  linux���̱߳��--�ź������������� ���Ѷ�ʧ�¼�

#endif //!__PTHREAD_POOL_H__