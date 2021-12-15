// Milan, copied from older files on 7th December 2021
//

#include "Multicore.h"
#include "Logger.h"

BEGIN_ENGINE_NAMESPACE
ThreadPool* ThreadPool::s_pThis = nullptr;

// With a fixed number n of threads, if no thread is idle and a clients submits a new task,
// it will have to wait for one of the thread to finish his work; in the worst case, no thread
// will be able to complete (e.g.: they could need the result of the new task) and run the new task,
// blocking the execution of the program
ThreadPool::ThreadPool(unsigned int maxThreadNumber, bool limitNumThreadsToNumberOfLogicCores)
	: m_Stop(false)
{
	Assert(ThreadPool::s_pThis == nullptr);
	ThreadPool::s_pThis = this;

	const unsigned max_thread = limitNumThreadsToNumberOfLogicCores ?
		std::min(maxThreadNumber, std::thread::hardware_concurrency()) : maxThreadNumber;

	for (size_t i = 0; i < max_thread; ++i)
	{
		this->m_Threads.push_back(std::thread(&ThreadPool::WorkerThread, this));
	} // end for
} // end class ThreadPool constructor

ThreadPool::~ThreadPool()
{
	this->m_Stop.store(true, std::memory_order_release);
	this->m_TaskSubmitted.notify_all();

	for (size_t i = 0; i < this->m_Threads.size(); ++i)
	{
		this->m_Threads[i].join();
	} // end for
	this->m_Threads.clear();
	//this->m_Mutex.unlock();

	Assert(ThreadPool::s_pThis == this);
	ThreadPool::s_pThis = nullptr;
} // end class ThreadPool destructor

void ThreadPool::WorkerThread()
{
	while (!this->m_Stop.load(std::memory_order_acquire))
	{
		if (auto task = this->m_PendingWorks.pop())
		{
			task->operator()();
		} // end if
		else
		{
			std::unique_lock<std::mutex> l(this->m_Mutex);
			this->m_TaskSubmitted.wait(l, [this]
				{ return (this->m_PendingWorks.size() > 0 || this->m_Stop.load(std::memory_order_acquire)); });
		} // end else
	} // end while
} // end method WorkerThread

END_ENGINE_NAMESPACE