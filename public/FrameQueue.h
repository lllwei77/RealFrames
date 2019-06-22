#ifndef H_FRAME_QUEUE
#define H_FRAME_QUEUE

#include <list>
#include <mutex>
#include <condition_variable>



template<typename T>
class FrameQueue
{
private:
	bool _isFull() const
	{
		return m_queue.size() == m_maxSize;
	}

	bool _isEmpty() const
	{
		return m_queue.empty();
	}

public:
	FrameQueue(int maxSize) : m_maxSize(maxSize)
	{
	}

	void put(const T& x)
	{
		std::lock_guard<std::mutex> locker(m_mutex);

		while (_isFull())
		{
			m_notFull.wait(m_mutex);
		}
		m_queue.push_back(x);
		m_notEmpty.notify_one();
	}

	void force_put(const T& x)
	{
		std::lock_guard<std::mutex> locker(m_mutex);

		while (_isFull())
		{
			m_queue.pop_front();
		}
		m_queue.push_back(x);
		m_notEmpty.notify_one();
	}

	void get(T& x)
	{
		std::lock_guard<std::mutex> locker(m_mutex);

		while (_isEmpty())
		{
			m_notEmpty.wait(m_mutex);
		}

		x = m_queue.front();
		m_queue.pop_front();
		m_notFull.notify_one();
	}

	bool isFull() const
	{
		return _isFull();
	}


private:
	std::list<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable_any m_notEmpty;
	std::condition_variable_any m_notFull;
	int m_maxSize;
};


#endif //H_FRAME_QUEUE
