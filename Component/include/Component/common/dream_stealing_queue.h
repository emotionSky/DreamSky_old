/**
 * \file dream_stealing_queue.h
 * \date 2023/05/19 22:08
 *
 * \author ����
 * Contact: bitdreamsky@126.com
 *
 * \brief ������ȡ���ܵİ�ȫ����
 *
 * TODO: �𲽽������ƣ������Ķ���Խ��Խ��Ŀ�Դ��������ƣ�
 *       ÿһ���Ķ��µĿ�Դ�ⶼ�����µ���ᡣ
 *       ��ô������Ҫ���仯Ϊ���ã�
 *
 * \note �ο��Ŀ�Դ��Ŀ
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