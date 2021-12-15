// Milan, copied from older files on 7th December 2021
//

#pragma once
// This code were originally found in:
//	"C++ Concurrency in Action, 2nd Edition", written by Anthony Williams

#include <memory>
#include <atomic>
#include <thread>
#include <future>
#include <tuple>
#include <vector>
#include "EngineUtility.h"

BEGIN_ENGINE_NAMESPACE

template <class _T>
class concurrent_queue
{
public:
	concurrent_queue();
	~concurrent_queue();

	void push(const _T& value);

	std::unique_ptr<_T> pop();
	size_t size() const;

private:
	struct node;

	struct node_counter
	{
		unsigned internal_references : 30;
		unsigned external_counters : 2;
	}; // end struct node_counter

	struct counted_node_ptr
	{
		int		m_external_references;
		node* m_ptr;
	}; // end struct counted_node_ptr

	struct node
	{
		node();
		~node();
		void release_ref();

		std::atomic<_T*>				m_Data;
		std::atomic<node_counter>		m_References;
		std::atomic<counted_node_ptr>	m_Next;
	}; // end struct node declaration

	void update_external_reference(std::atomic<counted_node_ptr>& dest, counted_node_ptr& source) const;
	void remove_external_reference(std::atomic<counted_node_ptr>& dest, counted_node_ptr& source) const;
	void free_external_counter(counted_node_ptr& dest) const;
	inline void set_new_tail(counted_node_ptr& old_tail, const counted_node_ptr& new_tail);

	std::atomic<counted_node_ptr> m_head;
	std::atomic<counted_node_ptr> m_tail;
	std::atomic<size_t> m_entries;

	concurrent_queue(const concurrent_queue& copy) = delete;
	concurrent_queue& operator=(const concurrent_queue& right) = delete;
}; // end template class concurrent_queue definition

class ThreadPool
{
public:
	explicit ThreadPool(unsigned int maxThreadNumber = 4, bool limitNumThreadsToNumberOfLogicCores = true);
	~ThreadPool();

	template <class FunctionType, class... _Args>
	constexpr auto SubmitTask(FunctionType&& f, _Args&&... args)
	{
		typedef typename std::invoke_result<FunctionType, _Args...>::type result_type;
		std::packaged_task<result_type(_Args...)> task(std::move(f));
		std::future<result_type> result = task.get_future(); // std::future<result_type>
		this->m_PendingWorks.push(FunctionWrapper(std::move(task), std::forward<_Args>(args)...));

		// Notify waiting threads that there's something to do
		this->m_TaskSubmitted.notify_one();
		return result;
	} // end template method SubmitTask

private:
	static ThreadPool* s_pThis;
	std::atomic_bool m_Stop;
	std::mutex m_Mutex;
	std::condition_variable m_TaskSubmitted;

	concurrent_queue<FunctionWrapper> m_PendingWorks;
	std::vector<std::thread> m_Threads;
	void WorkerThread();

	ThreadPool(const ThreadPool& t) = delete;
	ThreadPool& operator=(const ThreadPool& rht) = delete;
}; // end class ThreadPool declaration


// Class concurrent_queue::node implementation
template <class _T>
concurrent_queue<_T>::node::node()
	: m_Data(nullptr)
{
	node_counter new_count;
	new_count.internal_references = 0;
	new_count.external_counters = 2;

	this->m_References.store(new_count);
	this->m_Next.store({ 0, nullptr });
} // end class node constructor

template <class _T>
concurrent_queue<_T>::node::~node()
{
	_T* old_data = this->m_Data.exchange(nullptr);
	if (old_data)
		delete old_data;
} // end class node destructor

template <class _T>
void concurrent_queue<_T>::node::release_ref()
{
	node_counter old_counter = this->m_References.load(std::memory_order_relaxed);
	node_counter new_counter;
	do
	{
		new_counter = old_counter;
		--new_counter.internal_references;
	} while (!this->m_References.compare_exchange_strong(old_counter, new_counter,
		std::memory_order_acquire, std::memory_order_relaxed));

	if (new_counter.internal_references == 0 && new_counter.external_counters == 0)
	{
		delete this;
	} // end if
} // end method release_ref

	// Class template concurrent_queue implementation
	template <class _T>
	concurrent_queue<_T>::concurrent_queue()
		: m_head({ 0, new node }), m_tail(m_head.load()),
		m_entries(0)
	{
	} // end class queue constructor

template <class _T>
concurrent_queue<_T>::~concurrent_queue()
{
	while (node* const old_head = this->m_head.load().m_ptr)
	{
		this->m_head.store(old_head->m_Next);
		delete old_head;
	} // end while
} // end class queue constructor

template <class _T>
size_t concurrent_queue<_T>::size() const
{
	return this->m_entries.load(std::memory_order_acquire);
} // end method size

template <class _T>
void concurrent_queue<_T>::push(const _T& value)
{
	std::unique_ptr<_T> new_data(new _T(value));
	counted_node_ptr new_next;
	new_next.m_ptr = new node; // dummy node
	new_next.m_external_references = 1; // each new node it's referenced from tail->next
	counted_node_ptr old_tail = this->m_tail.load();

	for (;;)
	{
		this->update_external_reference(this->m_tail, old_tail);

		_T* old_data = nullptr;
		if (old_tail.m_ptr->m_Data.compare_exchange_strong(old_data, new_data.get()))
		{
			// Now old_tail.m_ptr->m_Data points to the value to push in the queue
			counted_node_ptr old_next = { 0 };
			if (!old_tail.m_ptr->m_Next.compare_exchange_strong(old_next, new_next))
			{
				// If old_tail.m_ptr->m_Next is not null, another thread is helping this one
				// use this as the new node's next pointer
				delete new_next.m_ptr;
				new_next = old_next;
			} // end if

			this->set_new_tail(old_tail, new_next);
			new_data.release();
			break;
		} // end if
		else
		{
			// Another thread has updated old_tail.m_ptr->m_Data meanwhile,
			counted_node_ptr old_next = { 0 };
			if (old_tail.m_ptr->m_Next.compare_exchange_strong(old_next, new_next))
			{
				// Help him terminate its push execution by creating the new dummy node
				// but only if it hasn't been created yet by other threads
				old_next = new_next;
				new_next.m_ptr = new node;
			} // end if

			this->set_new_tail(old_tail, old_next);
		} // end else
	} // end for
	this->m_entries.fetch_add(1, std::memory_order_release);
} // end method push

template <class _T>
std::unique_ptr<_T> concurrent_queue<_T>::pop()
{
	counted_node_ptr old_head = this->m_head.load(std::memory_order_relaxed);
	for (;;)
	{
		//this->increase_external_count(this->head, old_head);
		this->update_external_reference(this->m_head, old_head);

		node* to_remove = old_head.m_ptr;
		if (to_remove == this->m_tail.load().m_ptr) // head == tail, nothing to pop
		{
			// remove one external reference from head
			this->remove_external_reference(this->m_head, old_head);
			return std::unique_ptr<_T>(nullptr);
		} // end if

		counted_node_ptr future_head = to_remove->m_Next.load();
		if (this->m_head.compare_exchange_strong(old_head, future_head))
		{
			_T* res = to_remove->m_Data.exchange(nullptr);
			this->remove_external_reference(this->m_head, future_head);
			future_head.m_ptr->release_ref();
			this->free_external_counter(old_head);

			this->m_entries.fetch_sub(1, std::memory_order_release);
			return std::unique_ptr<_T>(res);
		} // end if

		to_remove->release_ref();
	} // end for
} // end method pop

template <class _T>
void concurrent_queue<_T>::update_external_reference(std::atomic<counted_node_ptr>& dest, counted_node_ptr& source) const
{
	counted_node_ptr new_node;
	do
	{
		// add one to head's external reference count
		new_node = source;
		++new_node.m_external_references;
	} while (!dest.compare_exchange_strong(source, new_node,
		std::memory_order_acquire, std::memory_order_relaxed));

	source.m_external_references = new_node.m_external_references;
} // end method update_external_reference

template <class _T>
void concurrent_queue<_T>::free_external_counter(counted_node_ptr& dest) const
{
	node* ptr = dest.m_ptr;
	const int count_increase = dest.m_external_references - 1;// -2;
	node_counter old_counter = ptr->m_References.load(std::memory_order_relaxed);
	node_counter new_counter;
	do
	{
		new_counter = old_counter;
		--new_counter.external_counters;
		new_counter.internal_references += count_increase;
	} while (!ptr->m_References.compare_exchange_strong(old_counter, new_counter,
		std::memory_order_acquire, std::memory_order_relaxed));

	if (new_counter.internal_references == 0 && new_counter.external_counters == 0)
	{
		delete ptr;
		ptr = nullptr;
	} // end if
} // end method free_external_counter

template <class _T>
void concurrent_queue<_T>::set_new_tail(counted_node_ptr& old_tail, const counted_node_ptr& new_tail)
{
	node* const current_tail_node = old_tail.m_ptr;

	while (!this->m_tail.compare_exchange_weak(old_tail, new_tail) && old_tail.m_ptr == current_tail_node);
	if (old_tail.m_ptr == current_tail_node)
		free_external_counter(old_tail);
	else
		current_tail_node->release_ref();
} // end method set_new_tail

template <class _T>
void concurrent_queue<_T>::remove_external_reference(std::atomic<counted_node_ptr>& dest, counted_node_ptr& source) const
{
	counted_node_ptr new_node;
	do
	{
		// add one to head's external reference count
		new_node = source;
		--new_node.m_external_references;
	} while (!dest.compare_exchange_strong(source, new_node,
		std::memory_order_acquire, std::memory_order_relaxed));

	source.m_external_references = new_node.m_external_references;
} // end method remove_external_reference

END_ENGINE_NAMESPACE