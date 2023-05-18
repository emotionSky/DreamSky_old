/**
 * \file dream_priority_queue.h
 * \date 2023/05/18 22:08
 *
 * \author 凌佚
 * Contact: bitdreamsky@126.com
 *
 * \brief 线程安全的优先队列
 *
 * TODO: 逐步进行完善，根据阅读的越来越多的开源库进行完善，
 *       每一次阅读新的开源库都会有新的体会。
 *       那么，都需要将其化为己用！
 *
 * \note 参考的开源项目
 * CThreadPool: https://github.com/ChunelFeng/CThreadPool.git
 */

#ifndef __DREAM_PRIORITY_QUEUE_H__
#define __DREAM_PRIORITY_QUEUE_H__

#include "dream_define.h"
#include <Component/thread/lock/dream_mutex.h>
#include <queue>
#include <memory> // unique_ptr
#include <vector>

// 参考的项目中，对于这个队列还进行了Object的继承，个人感觉没有必要，
// queue的存在并不是一个Object的概念，更需要避免虚函数表等带来的额外消耗。

DREAMSKY_NAMESPACE_BEGIN

template<typename T>
class DPriorityQueue
{
public:
    DPriorityQueue() = default;
    ~DPriorityQueue() = default;

    /**
     * \brief 尝试弹出
     * 
     * \param[out] value 弹出的数据
     * \return 弹出成功则返回true，否则返回false
     */
    bool TryPop(T &value)
    {
        DLockGuard lg(m_mutex);
        if (m_queue.empty())
        {
            return false;
        }
        value = std::move(*m_queue.top());
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
     * \param[in] priority 数据(任务)优先级，数字排序
     */
    void Push(T&& value, int priority)
    {
        std:unique_ptr<T> task(make_unique<T>(std::move(value), priority));
        DLockGuard lg(m_mutex);
        m_queue.push(std::move(task));
    }

    /**
     * \brief 判断队列是否为空
     * 
     * \return 队列为空则返回true，否则返回false
     */
    [[nodiscard]] bool Empty() const
    {
        DLockGuard lg(m_mutex);
        return m_queue.empty();
    }

private:
    std::mutex m_mutex;
    std::priority_queue<std::unique_ptr<T> > m_queue;    // 优先队列信息，根据重要级别决定先后执行顺序
};

DREAMSKY_NAMESPACE_END

#endif //__DREAM_PRIORITY_QUEUE_H__
