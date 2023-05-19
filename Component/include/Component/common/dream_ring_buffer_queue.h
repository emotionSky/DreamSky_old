/**
 * \file dream_ring_buffer_queue.h
 * \date 2023/05/19 22:08
 *
 * \author ����
 * Contact: bitdreamsky@126.com
 *
 * \brief ʹ��vectorʵ�ֵĻ��λ�����У���֧�ֵ��뵥��
 *
 * TODO: �𲽽������ƣ������Ķ���Խ��Խ��Ŀ�Դ��������ƣ�
 *       ÿһ���Ķ��µĿ�Դ�ⶼ�����µ���ᡣ
 *       ��ô������Ҫ���仯Ϊ���ã�
 *
 * \note �ο��Ŀ�Դ��Ŀ
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
	 * \brief ���ö��е�����
	 * \details ����Ѿ������˵������������ʹ�ô˽ӿڣ����ⶪʧ����
	 * 
	 * \param[in] size �µĶ�������
	 */
	void ResetCapacity(uint32_t size)
	{
		m_capacity = size;
		m_queue.resize(m_capacity);
	}

	/**
	 * \brief ��ȡ���е�����
	 * 
	 * \return ��������
	 */
	[[nodiscard]] GetCapacity() const
	{
		return m_capacity;
	}

	/**
	 * \brief ��ն��������е�����
	 * 
	 */
	void Clear()
	{
		m_queue.resize(0);
		m_head = 0;
		m_tail = 0;
	}

	/**
	 * \brief ��������
	 * \details ʹ����������������ʱ�����
	 * 
	 * \param[in] value ��Ҫ���������
	 */
	void Push(T& value)
	{
		{
			DLockGuard lg(m_mutex);
			if (IsFull())
			{
				//����ȵ����в�����ʱ��ſ��Խ���push�Ĳ���
				m_pushCv.wait(lg, [this]() {return !IsFull(); });
			}

			m_queue[m_tail] = std::move(d_make_unique<T>(value));
			//m_queue[m_tail] = std::move(std::make_unique(value));
			m_tail = (m_tail + 1) % m_capacity;
		}
		m_popCv.notify_one();
	}

	/**
	 * \brief �ȴ���������
	 * \details ʹ����������������ʱ�����
	 *
	 * \param[out] value ����������
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
	 * \brief �ж��Ƿ��Ѿ�����
	 * \details ���ֻʣ��һ��λ���ˣ���ô������д�룬������
	 * 
	 * \return �����򷵻�true�����򷵻�false
	 */
	bool IsFull() const
	{
		return m_head == (m_tail + 1) % m_capacity;
	}

	/**
	 * \brief �ж��Ƿ�Ϊ��
	 * 
	 * \return ����Ϊ���򷵻�true�����򷵻�false
	 */
	bool IsEmpty() const
	{
		return m_head == m_tail;
	}

private:
	uint32_t m_head; ///< ͷ�ڵ�λ��
	uint32_t m_tail; ///< β�ڵ�λ��
	uint32_t m_capacity; ///< ���λ����������С
	DMutex m_mutex;
	DConditionVariable m_popCv;
	DConditionVariable m_pushCv;
	std::vector<std::unique_ptr<T>> m_queue;
};

DREAMSKY_NAMESPACE_END

#endif //!__DREAM_RING_BUFFER_QUEUE_H__