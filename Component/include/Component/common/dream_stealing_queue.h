/**
 * \file dream_stealing_queue.h
 * \date 2023/05/19 22:08
 *
 * \author 凌佚
 * Contact: bitdreamsky@126.com
 *
 * \brief 包含盗取功能的安全队列
 *
 * TODO: 逐步进行完善，根据阅读的越来越多的开源库进行完善，
 *       每一次阅读新的开源库都会有新的体会。
 *       那么，都需要将其化为己用！
 *
 * \note 参考的开源项目
 * CThreadPool: https://github.com/ChunelFeng/CThreadPool.git
 */

#ifndef __DREAM_STEALING_QUEUE_H__
#define __DREAM_STEALING_QUEUE_H__

#if 0
#include <queue>
#include <deque>
#include <Component/thread/lock/dream_mutex.h>
#include <Component/thread/lock/dream_spin_lock.h>
#include "dream_define.h"
#include "dream_ptr.h"

DREAMSKY_NAMESPACE_BEGIN

template<typename T>
class DStealingQUeue
{
public:
	DStealingQUeue() = default;
	~DStealingQUeue() = default;

	DREAMSKY_AVOID_COPY(DStealingQUeue)

private:

};

DREAMSKY_NAMESPACE_END
#endif

#endif //!__DREAM_STEALING_QUEUE_H__