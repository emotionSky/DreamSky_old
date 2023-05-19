/**
 * \file dream_mutex.h
 * \date 2023/05/18 09:08
 *
 * \author 凌佚
 * Contact: bitdreamsky@126.com
 *
 * \brief 关于一些线程锁的使用
 *
 * TODO: 逐步进行完善，根据阅读的越来越多的开源库进行完善，
 *       每一次阅读新的开源库都会有新的体会。
 *       那么，都需要将其化为己用！
 *
 * \note 后续考虑使用宏定义控制使用pthread还是std::thread这种
 */

#ifndef __DREAM_MUTEX_H__
#define __DREAM_MUTEX_H__

#include <Component/common/dream_define.h>
#if __cplusplus >= 201703L
#   include <shared_mutex>
#else
#   include <mutex>
#endif

DREAMSKY_NAMESPACE_BEGIN

using DMutex = std::mutex;
using DLockGuard = std::lock_guard<std::mutex>;
using DUniqueLock = std::unique_lock<std::mutex>;
using DConditionVariable = std::condition_variable;

// C++17 才有读写锁的概念
#if __cplusplus >= 201703L
using DSharedMutex = std::shared_mutex;
using DReadLock = std::shared_lock<std::shared_mutex>;
using DWriteLock = std::unique_lock<std::shared_mutex>;
#else
using DSharedMutex = std::mutex;
using DReadLock = std::unique_lock<std::mutex>;
using DWriteLock = std::unique_lock<std::mutex>;
#endif

DREAMSKY_NAMESPACE_END

#endif //__DREAM_MUTEX_H__
