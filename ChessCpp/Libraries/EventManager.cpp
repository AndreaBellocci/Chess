// Milan, copied from older files on 7th December 2021
//

#include "EventManager.h"
#include "Logger.h"

BEGIN_ENGINE_NAMESPACE

ObjectFactory<IEvent, EventType> g_EventFactory;
static EventManager* g_pEventManager = nullptr;

EventManager::EventManager(bool setAsGlobal)
	: m_ActiveQueue(0)
{
	if (setAsGlobal)
	{
		if (g_pEventManager)
		{
			LOG_WARNING(LOGS::L_OBJECT_ALREADY_INITIALIZED, g_pEventManager);
		} // end if
		else
		{
			g_pEventManager = this;
		} // end else
	} // end if
} // end class EventManager constructor

EventManager::~EventManager()
{
	this->m_AllListeners.clear();
	for (int i = 0; i < EventManager::QueueNumber; ++i)
		this->m_EventQueues[i].clear();

	if (g_pEventManager == this)
		g_pEventManager = nullptr;
} // end class EventManager destructor

EventManager* EventManager::GetGlobal()
{
	return g_pEventManager;
} // end static method GetGlobal

bool EventManager::AddListener(const EventListener& eventDelegate, const EventType& type)
{
	auto& eventListenerList = this->m_AllListeners[type];  // this will find or create the entry
	for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
	{
		if (eventDelegate == (*it))
			return false;
	} // end for

	eventListenerList.push_back(eventDelegate);
	return true;
} // end method AddListener

bool EventManager::RemoveListener(const EventListener& eventDelegate, const EventType& type)
{
	auto findIt = this->m_AllListeners.find(type);
	if (findIt != this->m_AllListeners.end())
	{
		auto& listeners = findIt->second;
		for (auto it = listeners.begin(); it != listeners.end(); ++it)
		{
			if (eventDelegate == (*it))
			{
				listeners.erase(it);
				return true;
			} // end if
		} // end for
	} // end if		
	return false;
} // end method RemoveListener

void EventManager::TriggerEvent(const IEventPtr& pEvent) const
{
	if (!pEvent)
		return;

	auto findIt = this->m_AllListeners.find(pEvent->VGetEventType());
	if (findIt != this->m_AllListeners.end())
	{
		const auto& eventListenerList = findIt->second;
		for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
		{
			(*it)(pEvent); // call the delegate
		} // end for
	} // end if
} // end method TriggerEvent

void EventManager::PostEvent(const IEventPtr& pEvent)
{
	this->m_RealTimeEventQueue.push(pEvent);
} // end method PostEvent

bool EventManager::AbortEvent(const EventType& inType, bool allOfType)
{
	bool success = false;
	auto findIt = this->m_AllListeners.find(inType);

	if (findIt != this->m_AllListeners.end())
	{
		auto& eventQueue = this->m_EventQueues[this->m_ActiveQueue];
		auto it = eventQueue.begin();
		while (it != eventQueue.end())
		{
			// Removing an item from the queue will invalidate the iterator, so have it point to the next member
			auto thisIt = it;
			++it;

			if ((*thisIt)->VGetEventType() == inType)
			{
				eventQueue.erase(thisIt);
				success = true;
				break;
			} // end if
		} // end while

		if (allOfType)
		{
			// Remove all events
			while (it != eventQueue.end())
			{
				auto thisIt = it;
				++it;

				if ((*thisIt)->VGetEventType() == inType)
				{
					eventQueue.erase(thisIt);
				} // end if
			} // end while
		} // end if
	} // end if

	return success;
} // end method AbortEvent

int EventManager::Update(nanoseconds maxNanoSeconds)
{
	const auto finish = std::chrono::high_resolution_clock::now() + std::chrono::nanoseconds(maxNanoSeconds);

	// Queue all messages from all threads into the active queue
	if (maxNanoSeconds == kINFINITE)
	{
		while (const auto e = this->m_RealTimeEventQueue.pop())
		{
			this->QueueEvent(*e);
		} // end while
	} // end if
	else
	{
		std::unique_ptr<IEventPtr> e = nullptr;
		while (finish > std::chrono::high_resolution_clock::now() && (e = this->m_RealTimeEventQueue.pop()))
		{
			this->QueueEvent(*e);
		} // end while
	} // end else

	// Swap active queues and clear the new queue after the swap
	const int queueToProcess = this->m_ActiveQueue;
	this->m_ActiveQueue = (this->m_ActiveQueue + 1) % EventManager::QueueNumber;
	this->m_EventQueues[this->m_ActiveQueue].clear();

	// Process the queue
	int eventProcessed = 0;
	while (!this->m_EventQueues[queueToProcess].empty())
	{
		// Pop the front of the queue	
		auto pEvent = this->m_EventQueues[queueToProcess].front();
		this->m_EventQueues[queueToProcess].pop_front();

		// Find all the delegate functions registered for this event
		auto findIt = this->m_AllListeners.find(pEvent->VGetEventType());
		if (findIt != this->m_AllListeners.end())
		{
			const auto& eventListeners = findIt->second;

			// call each listener
			for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
			{
				(*it)(pEvent);
			} // end for
		} // end if

		++eventProcessed;

		if ((maxNanoSeconds != kINFINITE) && (finish < std::chrono::high_resolution_clock::now())) //  && (!this->m_EventQueues[queueToProcess].empty())
		{
			// If we couldn't process all of the events, push the remaining events to the new active queue.
			// Note: To preserve sequencing, go back-to-front, inserting them at the head of the active queue
			IEventPtr pBackEvent = nullptr;
			while (!this->m_EventQueues[queueToProcess].empty() && (pBackEvent = this->m_EventQueues[queueToProcess].back()))
			{
				this->m_EventQueues[queueToProcess].pop_back();
				this->m_EventQueues[this->m_ActiveQueue].push_front(pBackEvent);
			} // end while

			LOG_MESSAGE(LOGS::L_TIME_EXCEEDED, "Event processing took too much time");
			return eventProcessed;
		} // end if
	} // end while
	return eventProcessed;
} // end method Update

size_t EventManager::GetEventsToProcessNumber() const
{
	size_t size = 0;
	for (size_t i = 0; i < EventManager::QueueNumber; ++i)
		size += this->m_EventQueues[i].size();
	return this->m_RealTimeEventQueue.size() + size;
} // end method GetEventsToProcessNumber

void EventManager::QueueEvent(const IEventPtr& pEvent)
{
	if (!pEvent)
		return;

	// Queue the event only if there's at least one listener to process it
	const auto findIt = this->m_AllListeners.find(pEvent->VGetEventType());
	if (findIt != this->m_AllListeners.end())
	{
		this->m_EventQueues[this->m_ActiveQueue].push_back(pEvent);
	} // end if
} // end method PostEvent

END_ENGINE_NAMESPACE