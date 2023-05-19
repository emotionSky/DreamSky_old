/**
 * \file dream_ring_buffer_queue.h
 * \date 2023/05/19 22:08
 *
 * \author 凌佚
 * Contact: bitdreamsky@126.com
 *
 * \brief 使用vector实现的环形缓冲队列，仅支持单入单出
 *
 * TODO: 逐步进行完善，根据阅读的越来越多的开源库进行完善，
 *       每一次阅读新的开源库都会有新的体会。
 *       那么，都需要将其化为己用！
 *
 * \note 参考的开源项目
 * CThreadPool: https://github.com/ChunelFeng/CThreadPool.git
 */

#ifndef __DREAM_RING_BUFFER_QUEUE_H__
#define __DREAM_RING_BUFFER_QUEUE_H__

#include <memory> // unique_ptr
#include <vector>
#include <Component/thread/lock/dream_mutex.h>
#include "dream_define.h"
#include "dream_ptr.h"

DREAMSKY_NAMESPACE_BEGIN

template<typename T, uint32_t capacity = DREAMSKY_RINGBUFFER_SIZE>
class DRingBufferQUeue
{
public:
	explicit DRingBufferQUeue() :
		m_head(0), m_tail(0), m_capacity(capacity)
	{
		m_queue.resize(m_capacity);
	}

	~DRingBufferQUeue()
	{
		Clear();
	}

	DREAMSKY_AVOID_COPY(DRingBufferQUeue)

	/**
	 * \brief 重置队列的容量
	 * \details 如果已经进行了弹入操作，谨慎使用此接口，避免丢失数据
	 * 
	 * \param[in] size 新的队列容量
	 */
	void ResetCapacity(uint32_t size)
	{
		m_capacity = size;
		m_queue.resize(m_capacity);
	}

	/**
	 * \brief 获取队列的容量
	 * 
	 * \return 队列容量
	 */
	[[nodiscard]] GetCapacity() const
	{
		return m_capacity;
	}

	/**
	 * \brief 清空队列中所有的数据
	 * 
	 */
	void Clear()
	{
		m_queue.resize(0);
		m_head = 0;
		m_tail = 0;
	}

	/**
	 * \brief 弹入数据
	 * \details 使用条件变量来进行时序控制
	 * 
	 * \param[in] value 需要弹入的数据
	 */
	void Push(T& value)
	{
		{
			DLockGuard lg(m_mutex);
			if (IsFull())
			{
				//必须等到队列不满的时候才可以进行push的操作
				m_pushCv.wait(lg, [this]() {return !IsFull(); });
			}

			m_queue[m_tail] = std::move(d_make_unique<T>(value));
			//m_queue[m_tail] = std::move(std::make_unique(value));
			m_tail = (m_tail + 1) % m_capacity;
		}
		m_popCv.notify_one();
	}

	/**
	 * \brief 等待弹出数据
	 * \details 使用条件变量来进行时序控制
	 *
	 * \param[out] value 弹出的数据
	 */
	void WaitPop(T& value)
	{
		{
			DLockGuard lg(m_mutex);
			if (IsEmpty())
			{
				m_popCv.wait(lg, [this]() {return !IsEmpty(); });
			}

			value = *m_queue[m_head];
			m_queue[m_head] = {};
			m_head = (m_head + 1) % m_capacity;
		}
		m_pushCv.notify_one();
	}

protected:
	/**
	 * \brief 判断是否已经满了
	 * \details 如果只剩下一个位置了，那么不允许写入，即满了
	 * 
	 * \return 满了则返回true，否则返回false
	 */
	bool IsFull() const
	{
		return m_head == (m_tail + 1) % m_capacity;
	}

	/**
	 * \brief 判断是否为空
	 * 
	 * \return 队列为空则返回true，否则返回false
	 */
	bool IsEmpty() const
	{
		return m_head == m_tail;
	}

private:
	uint32_t m_head; ///< 头节点位置
	uint32_t m_tail; ///< 尾节点位置
	uint32_t m_capacity; ///< 环形缓冲的容量大小
	DMutex m_mutex;
	DConditionVariable m_popCv;
	DConditionVariable m_pushCv;
	std::vector<std::unique_ptr<T>> m_queue;
};

DREAMSKY_NAMESPACE_END

#endif //!__DREAM_RING_BUFFER_QUEUE_H__