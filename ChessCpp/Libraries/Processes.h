// Milan, copied from older files on 10th December 2021
//

#pragma once
#include <memory>
#include <list>
#include "EngineUtility.h"

BEGIN_ENGINE_NAMESPACE

typedef std::shared_ptr<class Process> ProcessPtr;

class Process
{
	friend class ProcessManager;
public:
	Process();
	virtual ~Process();

	// Methods to end the process
	void Succeed();
	void Fail();
	void Abort();

	// Toggle pause
	void Pause();
	void UnPause();

	// Accessors
	bool NeedInitialization() const;
	bool IsAlive() const;
	bool IsDead() const;
	bool IsRemoved() const;
	bool IsPaused() const;

	// Child methods
	void AttachChildProcess(const ProcessPtr& pChild); // Attach the passed-in process at the end of the process chain
	ProcessPtr DetachChild(); // Releases ownership of the child to the caller and remove it from the process chain
	ProcessPtr PeekChild() const;	// Return the child process of this process

protected:
	virtual void VOnInitialize();	// Called during the first update; must set the initial state of the process
	virtual void VOnUpdate(nanoseconds delta_ns) = 0;	// Called every frame
	virtual void VOnSuccess();	// Called if the process succeeds
	virtual void VOnFail();		// Called if the process fails
	virtual void VOnAbort();	// Called if the process is aborted

	enum class ProcessState
	{
		// Neither dead nor alive
		PS_Uninitialized = -1,	// Created but not running
		PS_Removed,				// Removed from the process list but not destroyed

		// Living processes
		PS_Running,	// Initialized and running
		PS_Paused,	// Initialized but paused

		// Dead processes
		PS_Succeeded,	// Completed successfully
		PS_Failed,		// Failed to complete
		PS_Aborted		// Process aborted, it may not have even started
	}; // end scoped enum ProcessState

	ProcessState GetProcessState() const;
	ProcessState m_CurrentProcessState;
private:
	ProcessPtr m_pChildProcess;
}; // end class Process declaration


// TODO: static access for global manager
class ProcessManager
{
public:
	~ProcessManager();

#if DEBUGGING
	UINT64 UpdateProcesses(nanoseconds delta_ns);	// Update each process; see below for how to use the return value
#else
	void UpdateProcesses(nanoseconds delta_ns);	// Update each process
#endif

	void AddProcess(const ProcessPtr& pProcess); // Add the process to the process list so it can be run on the next update
	void AbortAllProcesses(bool immediate); // Abort all processes; if immediate is true, immmediately call VOnAbort() and destroy each process

	size_t GetProcessCount() const;

#if DEBUGGING
	// ProcessManager::UpdateProcesses returns the number of succeeded processed and that of failed processed
	// as an unsigned long long int; use the following constexprs to get the actual values
	constexpr ULONG GetFailedProcesses(UINT64 updateProcessesReturnValue) const;
	constexpr ULONG GetSucceededProcesses(UINT64 updateProcessesReturnValue) const;
#else 
#	define GetFailedProcesses(x)	do { void(0); } while(0);
#	define GetSucceededProcesses(x)	do { void(0); } while(0);
#endif

private:
	std::list<ProcessPtr> m_Processes;
}; // end class ProcessManager declaration

END_ENGINE_NAMESPACE