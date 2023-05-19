/**
 * \file dream_queue.h
 * \date 2023/05/19 22:08
 *
 * \author ����
 * Contact: bitdreamsky@126.com
 *
 * \brief �̰߳�ȫ����ͨ����
 *
 * TODO: �𲽽������ƣ������Ķ���Խ��Խ��Ŀ�Դ��������ƣ�
 *       ÿһ���Ķ��µĿ�Դ�ⶼ�����µ���ᡣ
 *       ��ô������Ҫ���仯Ϊ���ã�
 *
 * \note �ο��Ŀ�Դ��Ŀ
 * CThreadPool: https://github.com/ChunelFeng/CThreadPool.git
 */

#ifndef __DREAM_QUEUE_H__
#define __DREAM_QUEUE_H__

#include <queue>
#include <vector>
#include <Component/thread/lock/dream_mutex.h>
#include "dream_define.h"
#include "dream_ptr.h"

DREAMSKY_NAMESPACE_BEGIN

template<typename T>
class DQueue
{
public:
	DQueue() = default;
	~DQueue() = default;
    DREAMSKY_AVOID_COPY(DQueue)

	void WaitPop(T& value)
	{
		DLockGuard lg(m_mutex);
		m_cv.wait(lg, [this]() {return !m_queue.empty(); });
		value = std::move(*m_queue.front());
		m_queue.pop();
	}

	/**
	 * \brief ���Ե���
	 *
	 * \param[out] value ����������
	 * \return �����ɹ��򷵻�true�����򷵻�false
	 */
	bool TryPop(T& value)
	{
		DLockGuard lg(m_mutex);
		if (m_queue.empty())
		{
			return false;
		}
		value = std::move(*m_queue.front());
		m_queue.pop();
		return true;
	}

    /**
    * \brief ���Ե����������
    * \details ���������ݸ���С�ڵ���ָ���ĵ���������
    *
    * \param[out] values �����Ķ������
    * \param[in]  maxSize ָ���ĵ�������
    * \return �����ɹ��򷵻�true�����򷵻�false
    */
    bool TryPop(std::vector<T>& values, int maxSize)
    {
        if (maxSize <= 0)
        {
            return false;
        }
        DLockGuard lg(m_mutex);
        if (m_queue.empty())
        {
            return false;
        }
        while (!m_queue.empty() && maxSize--)
        {
            values.emplace_back(*m_queue.top());
            m_queue.pop();
        }
        return true;
    }

    /**
     * \brief �������ݵ�����
     *
     * \param[in] value ���������
     */
    void Push(T&& value)
    {
        std:unique_ptr<T> task(d_make_unique<T>(std::move(value)));
        //std::unique_ptr<T> task(std::make_unique<T>(std::move(value)));
        DLockGuard lg(m_mutex);
        m_queue.push(std::move(task));
    }

    /**
     * \brief �ж϶����Ƿ�Ϊ��
     *
     * \return ����Ϊ���򷵻�true�����򷵻�false
     */
    [[nodiscard]] bool IsEmpty() const
    {
        DLockGuard lg(m_mutex);
        return m_queue.empty();
    }

private:
	DMutex m_mutex;
	DConditionVariable m_cv;
	std::queue<std::unique_ptr<T>> m_queue;
};

DREAMSKY_NAMESPACE_END

#endif //!__DREAM_QUEUE_H__