#ifndef __STD_THREAD_POOL_H__
#define __STD_THREAD_POOL_H__

#include "common_type.h"
#include <vector>
#include <list>
#include <atomic>
#include <thread>
#include <mutex>

class DREAMSKY_API StdThreadPool
{
public:
	StdThreadPool();
	~StdThreadPool();

	bool Create(int threadNum); //�������̳߳��е������߳�
	void StopAll(); //ʹ�̳߳��е������߳��˳�
	void Call();//�������ˣ���һ���̳߳��е��߳������ɻ� 

	//�����Ǹ�m_data������ݵģ���Ӧ����Ҫ����Ϊʵ�ʵĴ���
	void InData(int num); //�յ�һ��������Ϣ������Ϣ���У��������̳߳����߳����������Ϣ

private:
	static void ThreadFunc(void* threadData); //�̺߳���

	//����������m_data�е����ݵģ���Ӧ����Ҫ����Ϊʵ�ʵĴ���
	void ClearData();

private:
	//�����߳̽ṹ������ͳ���߳���Ϣ
	struct ThreadContext
	{
	public:
		ThreadContext(StdThreadPool* pPool) :_pPool(pPool), _handle(nullptr), _bRunning(false) {}
		~ThreadContext() {}

		bool           _bRunning; //����Ƿ���ʽ�������������������󣬲��������StopAll()���ͷ�	
		std::thread*   _handle;   //�߳̾��
		StdThreadPool* _pPool;  //��¼�̳߳ص�ָ��	
	};

private:
	static std::mutex m_mutex;//�߳�ͬ��������/Ҳ���߳�ͬ����
	static std::condition_variable m_cond; //�߳�ͬ����������
	static bool    m_bShutdown; //�߳��˳���־��false���˳���true�˳�
	int     m_threadNum;        //Ҫ�������߳�����
	std::atomic<int>           m_runningNum; //�߳���, �����е��߳�����ԭ�Ӳ���
	std::vector<ThreadContext*>  m_vThreads;//�߳� ��������������Ǹ����߳���

	//�Զ���������أ����������Ϊ��ʵ���̳߳ص�ʾ������
	std::list<int> m_data;
};


#endif //!__STD_THREAD_POOL_H__