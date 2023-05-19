/**
 * \file dream_queue.h
 * \date 2023/05/19 22:08
 *
 * \author 凌佚
 * Contact: bitdreamsky@126.com
 *
 * \brief 线程安全的普通队列
 *
 * TODO: 逐步进行完善，根据阅读的越来越多的开源库进行完善，
 *       每一次阅读新的开源库都会有新的体会。
 *       那么，都需要将其化为己用！
 *
 * \note 参考的开源项目
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
	 * \brief 尝试弹出
	 *
	 * \param[out] value 弹出的数据
	 * \return 弹出成功则返回true，否则返回false
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
    * \brief 尝试弹出多个数据
    * \details 弹出的数据个数小于等于指定的弹出的数量
    *
    * \param[out] values 弹出的多个数据
    * \param[in]  maxSize 指定的弹出数量
    * \return 弹出成功则返回true，否则返回false
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
     * \brief 传入数据到队列
     *
     * \param[in] value 传入的数据
     */
    void Push(T&& value)
    {
        std:unique_ptr<T> task(d_make_unique<T>(std::move(value)));
        //std::unique_ptr<T> task(std::make_unique<T>(std::move(value)));
        DLockGuard lg(m_mutex);
        m_queue.push(std::move(task));
    }

    /**
     * \brief 判断队列是否为空
     *
     * \return 队列为空则返回true，否则返回false
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