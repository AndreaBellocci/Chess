// Milan, copied from older files on 10th December 2021
//

#include "Processes.h"

BEGIN_ENGINE_NAMESPACE

Process::Process()
	: m_CurrentProcessState(ProcessState::PS_Uninitialized),
	m_pChildProcess(nullptr)
{} // end class Process constructor

Process::~Process()
{
	if (this->m_pChildProcess)
		this->m_pChildProcess->VOnAbort();
} // end class Process destructor

void Process::AttachChildProcess(const ProcessPtr& pChild)
{
	if (this->m_pChildProcess)
		this->m_pChildProcess->AttachChildProcess(pChild);
	else
		this->m_pChildProcess = pChild;
} // end method AttachChildProcess

ProcessPtr Process::PeekChild() const
{
	return this->m_pChildProcess;
} // end method PeekChild

ProcessPtr Process::DetachChild()
{
	if (this->m_pChildProcess)
	{
		// Prevents the child process from being destroyed during detaching;
		// The child gets destroyed only if the caller do not save the pointer returned
		ProcessPtr pChild = this->m_pChildProcess;
		this->m_pChildProcess = nullptr; // Reset the pointer
		return pChild;
	} // end if

	return nullptr;
} // end method DetachChild

Process::ProcessState Process::GetProcessState() const
{
	return this->m_CurrentProcessState;
} // end method GetProcessState

bool Process::NeedInitialization() const
{
	return this->m_CurrentProcessState == ProcessState::PS_Uninitialized;
} // end method IsAlive

bool Process::IsAlive() const
{
	return (this->m_CurrentProcessState == ProcessState::PS_Running || this->m_CurrentProcessState == ProcessState::PS_Paused);
} // end method IsAlive

bool Process::IsDead() const
{
	return (this->m_CurrentProcessState == ProcessState::PS_Succeeded ||
		this->m_CurrentProcessState == ProcessState::PS_Failed || this->m_CurrentProcessState == ProcessState::PS_Aborted);
} // end method IsDead

bool Process::IsRemoved() const
{
	return this->m_CurrentProcessState == ProcessState::PS_Removed;
} // end method IsRemoved

bool Process::IsPaused() const
{
	return this->m_CurrentProcessState == ProcessState::PS_Paused;
} // end method IsPaused

void Process::Succeed()
{
	if (this->m_CurrentProcessState == ProcessState::PS_Running || this->m_CurrentProcessState == ProcessState::PS_Paused)
		this->m_CurrentProcessState = ProcessState::PS_Succeeded;
} // end method Succeed

void Process::Fail()
{
	if (this->m_CurrentProcessState == ProcessState::PS_Running || this->m_CurrentProcessState == ProcessState::PS_Paused)
		this->m_CurrentProcessState = ProcessState::PS_Failed;
} // end method Fail

void Process::Abort()
{
	if (this->m_CurrentProcessState == ProcessState::PS_Running || this->m_CurrentProcessState == ProcessState::PS_Paused)
		this->m_CurrentProcessState = ProcessState::PS_Aborted;
} // end method Abort

void Process::Pause()
{
	if (this->m_CurrentProcessState == ProcessState::PS_Running)
		this->m_CurrentProcessState = ProcessState::PS_Paused;
} // end method Pause

void Process::UnPause()
{
	if (this->m_CurrentProcessState == ProcessState::PS_Paused)
		this->m_CurrentProcessState = ProcessState::PS_Running;
} // end method UnPause

void Process::VOnInitialize()
{
	this->m_CurrentProcessState = ProcessState::PS_Running;
} // end method VOnInitialize


void Process::VOnSuccess()
{} // end method VOnSuccess

void Process::VOnFail()
{} // end method VOnFail

void Process::VOnAbort()
{} // end method VOnAbort


ProcessManager::~ProcessManager()
{
	this->m_Processes.clear();
} // end class ProcessManager destructor


#if DEBUGGING
UINT64
#else 
void
#endif
ProcessManager::UpdateProcesses(nanoseconds delta_ns)
{
#if DEBUGGING
	ULONG successCount = 0;
	ULONG failCount = 0;
#endif
	auto it = this->m_Processes.begin();
	while (it != this->m_Processes.end())
	{
		// Grab the next process
		ProcessPtr pCurrProcess = (*it);

		// Save the iterator and increment counter, in case this process needs to be removed from the list
		auto thisIt = it;
		++it;

		// Process is uninitialized, so initialize it
		if (pCurrProcess->NeedInitialization())
			pCurrProcess->VOnInitialize();

		// Give the process an update tick if it's running
		if (pCurrProcess->GetProcessState() == Process::ProcessState::PS_Running)
			pCurrProcess->VOnUpdate(delta_ns);

		// Check to see if the process is dead
		if (pCurrProcess->IsDead())
		{
			// Run the appropriate exit function
			switch (pCurrProcess->GetProcessState())
			{
			case Process::ProcessState::PS_Succeeded:
				pCurrProcess->VOnSuccess();
				// Get child process, if any, and attach it to the process list
				{
					ProcessPtr pChild = pCurrProcess->DetachChild();
					if (pChild)
						this->AddProcess(pChild);
#if DEBUGGING
					else
					{
						// When a whole process chain is completed, update number of succeeded processes
						++successCount;
					} // end else
#endif
				}
				break;

			case Process::ProcessState::PS_Failed:
				pCurrProcess->VOnFail();
#if DEBUGGING
				++failCount;
#endif
				break;

			case Process::ProcessState::PS_Aborted:
				pCurrProcess->VOnAbort();
#if DEBUGGING
				++failCount;
#endif
				break;
			} // end switch

			// Remove the process from the list and destroy it
			this->m_Processes.erase(thisIt);
		} // end if
	} // end while

#if DEBUGGING
	return ((UINT64(successCount) << 8 * sizeof(ULONG)) | failCount);
#endif
} // UpdateProcesses

void ProcessManager::AddProcess(const ProcessPtr& pProcess)
{
	this->m_Processes.push_front(pProcess);
} // AddProcess

void ProcessManager::AbortAllProcesses(bool immediate)
{
	auto it = this->m_Processes.begin();
	if (immediate)
	{
		while (it != this->m_Processes.end())
		{
			auto tempIt = it;
			++it;

			ProcessPtr pProcess = *tempIt;
			if (pProcess->IsAlive())
			{
				pProcess->Abort();
				pProcess->VOnAbort();
				this->m_Processes.erase(tempIt);
			} // end if
		} // end if
	} // end if
	else
	{
		while (it != this->m_Processes.end())
		{
			auto tempIt = it;
			++it;

			ProcessPtr pProcess = *tempIt;
			if (pProcess->IsAlive())
			{
				pProcess->Abort();
			} // end if
		} // end if
	} // end else		
} // AbortAllProcesses


size_t ProcessManager::GetProcessCount() const
{
	return this->m_Processes.size();
} // GetProcessCount


#if DEBUGGING
constexpr ULONG ProcessManager::GetSucceededProcesses(UINT64 updateProcessesReturnValue) const
{
	return ULONG(updateProcessesReturnValue >> 8 * sizeof(ULONG));
} // end constexpr GetSucceededProcesses

constexpr ULONG ProcessManager::GetFailedProcesses(UINT64 updateProcessesReturnValue) const
{
	return ULONG(updateProcessesReturnValue);
} // end constexpr GetFailedProcesses
#endif

END_ENGINE_NAMESPACE