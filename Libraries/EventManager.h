// Milan, copied from older files on 7th December 2021
//

#pragma once

#include <list>
#include <map>
#include "ThirdParty/FastDelegate/FastDelegate.h"
#include "EngineUtility.h"
#include "Multicore.h"
	
BEGIN_ENGINE_NAMESPACE
	
typedef fastdelegate::FastDelegate1<const IEventPtr&, void> EventListener;

class EventManager
{
public:
	explicit EventManager(bool setAsGlobal);
	~EventManager();

	// Fire off event NOW. This bypasses the queue entirely and
	// immediately calls all delegate functions registered for the event		
	void TriggerEvent(const IEventPtr& pEvent) const;

	// Fire off event. This uses the queue and will call the delegate
	// function on the next call to VUpdate(), assuming there's enough time
	void PostEvent(const IEventPtr& pEvent);

	// Find the next-available instance of the named event type and remove it from the processing queue.
	// This may be done up to the point that it is actively being processed
	// (i.e.: it's still safe to happen during event processing itself)
	bool AbortEvent(const EventType& type, bool allOfType = false);

	// Getter for the main global event manager. This is the event manager that is used by the majority of the 
	// engine, although you are free to define your own as long as you instantiate it with setAsGlobal set to false.
	// It is not valid to have more than one global event manager
	static EventManager* GetGlobal();

	// Registers a delegate function that will get called when the event type
	// is triggered. Returns true if successful, false if not
	bool AddListener(const EventListener& eventDelegate, const EventType& type);

	// Removes a delegate/event type pairing from the internal tables.
	// Returns false if the pairing was not found
	bool RemoveListener(const EventListener& eventDelegate, const EventType& type);

	// Allow for processing of any queued messages, optionally specify a processing time limit so that the event 
	// processing does not take too long. Note the danger of using this artificial limiter is that all messages 
	// may not in fact get processed. Return the number of events processed
	int Update(nanoseconds maxNanoSeconds = kINFINITE);

	size_t GetEventsToProcessNumber() const;
private:
	constexpr static int QueueNumber = 2;
	void QueueEvent(const IEventPtr& pEvent);

	typedef std::list<EventListener>				EventListenersList;
	typedef std::map<EventType, EventListenersList>	EventListenersMap;
	typedef std::list<IEventPtr>					EventList;
	typedef concurrent_queue<IEventPtr>				ThreadSafeEventQueue;

	EventListenersMap m_AllListeners;
	EventList m_EventQueues[QueueNumber];
	int m_ActiveQueue;

	ThreadSafeEventQueue m_RealTimeEventQueue;
}; // end class EventManager declaration	
	
END_ENGINE_NAMESPACE