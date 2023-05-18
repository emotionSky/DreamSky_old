/**
 * \file dream_spin_lock.h
 * \date 2023/05/09 09:08
 *
 * \author 凌佚
 * Contact: bitdreamsky@126.com
 *
 * \brief spin_lock(自旋锁) 使用中，必须确保锁住的地方耗时极少才能使用，否则有灾难！
 *
 * TODO: 逐步进行完善，根据阅读的越来越多的开源库进行完善，
 *       每一次阅读新的开源库都会有新的体会。
 *       那么，都需要将其化为己用！
 *
 * \note 个人认为这种类结构，拒绝使用继承关系，避免虚函数表等带来的额外性能消耗
 */

#ifndef __DREAM_SPIN_LOCK_H__
#define __DREAM_SPIN_LOCK_H__

#include "../../common/dream_define.h"
#include <atomic>

DREAMSKY_NAMESPACE_BEGIN

class DSpinLock
{
public:
    /**
     * \brief 上锁
     * \return void
     */
    void Lock()
    {
        // memory_order_acquire 后面访存指令勿重排至此条指令之前
        while (m_flag.test_and_set(std::memory_order_acquire))
        {
        }
    }

    /**
     * \brief 解锁
     * \return void
     */
    void Unlock()
    {
        // memory_order_release 前面访存指令勿重排到此条指令之后
        m_flag.clear(std::memory_order_release);
    }

    /**
     * \brief 尝试上锁
     * \details 如果没有上锁，那么这里会上锁
     * \return 上锁返回 true，否则返回 false
     */
    bool TryLock()
    {
        return !m_flag.test_and_set();
    }

private:
    std::atomic_flag m_flag;
};

DREAMSKY_NAMESPACE_END

#endif //__DREAM_SPIN_LOCK_H__
