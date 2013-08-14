
#include "StdAfx.h"



/******************************Public*Routine******************************\
* Debug CAkCritSec helpers
*
* We provide debug versions of the Constructor, destructor, Lock and Unlock
* routines.  The debug code tracks who owns each critical section by
* maintaining a depth count.
*
* History:
*
\**************************************************************************/

#include "MsgThread.h"

#define ThreadStackSize (1*1024*1024)

CAkCritSec::CAkCritSec() 
{
#ifdef __Win32__
    InitializeCriticalSection(&m_CritSec);
#endif
    m_currentOwner = 0;
	m_lockCount = 0;
    m_fTrace = FALSE;
}

CAkCritSec::~CAkCritSec() {
#ifdef __Win32__
    DeleteCriticalSection(&m_CritSec);
#endif
}

/**
*   
*  @Date:	[2/16/2004]
*/
BOOL CAkCritSec::IsLock()
{
#ifdef __Win32__
    DWORD us = GetCurrentThreadId();
#else
	pthread_t us = pthread_self(); 
#endif

	return (m_currentOwner && m_currentOwner != us);
}

void CAkCritSec::Lock() {
    UINT tracelevel=3;

#ifdef __Win32__	
    DWORD us = GetCurrentThreadId();
    DWORD currentOwner = m_currentOwner;
#else
	pthread_t us = pthread_self();
	pthread_t currentOwner = m_currentOwner;
#endif

    if(currentOwner && (currentOwner != us)) {
        // already owned, but not by us
        if(m_fTrace) {
            //DbgLog((LOG_LOCKING, 2, TEXT("Thread %d about to wait for lock %x owned by %d"), 
            //    GetCurrentThreadId(), &m_CritSec, currentOwner));
            tracelevel=2;
            // if we saw the message about waiting for the critical
            // section we ensure we see the message when we get the
            // critical section
        }
    }

#ifdef __Win32__
    EnterCriticalSection(&m_CritSec);
#else
	m_CritSec.Lock();
#endif
	
    if(0 == m_lockCount++) 
	{
        // we now own it for the first time.  Set owner information
        m_currentOwner = us;
		
        if(m_fTrace) 
		{
            //DbgLog((LOG_LOCKING, tracelevel, TEXT("Thread %d now owns lock %x"), m_currentOwner, &m_CritSec));
        }
    }
}

void CAkCritSec::Unlock() 
{
    if(0 == --m_lockCount) {
        // about to be unowned
        if(m_fTrace) {
            //DbgLog((LOG_LOCKING, 3, TEXT("Thread %d releasing lock %x"), m_currentOwner, &m_CritSec));
        }
		
        m_currentOwner = 0;
    }

#ifdef __Win32__
		LeaveCriticalSection(&m_CritSec);
#else
		m_CritSec.Unlock();
#endif

}


// destructor for CAkMsgThread  - cleans up any messages left in the
// queue when the thread exited
CAkMsgThread::~CAkMsgThread() 
{
#ifdef __Win32__
    if(m_hThread != NULL) 
	{
        WaitForSingleObject(m_hThread, INFINITE);
        EXECUTE_ASSERT(CloseHandle(m_hThread));
		m_hThread =NULL; 
    }

	while (false == m_ThreadQueue.empty())
	{
		CMsg *pMsg = m_ThreadQueue.front();
		delete pMsg;
		m_ThreadQueue.pop();
	}
    if(m_hSem != NULL) {
        EXECUTE_ASSERT(CloseHandle(m_hSem));
    }

	if(m_hQueueLenSem != NULL) {
        EXECUTE_ASSERT(CloseHandle(m_hQueueLenSem));
    }
#else
	//pthread_join(m_threadId, &status);
    if(m_threadId != 0) 
    {
        int nret = 0;
        void *status = NULL;
        nret = pthread_join(m_threadId, &status);
        //GetExitCodeThread(m_hThread, lpdwExitCode);
        if(0 != nret)
        {
            fprintf(stderr, "pthread join thread %lu error\n", m_threadId);
        }
        //fprintf(stderr, "akmsgthread pthread_join thread 0x%lu\n", m_threadId);
        m_threadId = 0; 
    }

	while (true != m_ThreadQueue.empty())
	{
		CMsg *pMsg = m_ThreadQueue.front();
		delete pMsg;
		m_ThreadQueue.pop();
	}

	sem_destroy(&m_hSem);
	sem_destroy(&m_hQueueLenSem);
#endif
}

BOOL CAkMsgThread::CreateThread( ) 
{
#ifdef __Win32__
	m_nPreNum = 10;

    m_hSem = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
    if(m_hSem == NULL) 
	{
        return FALSE;
    }

	m_hQueueLenSem = CreateSemaphore(NULL, 100, 100, NULL);
    if(m_hQueueLenSem == NULL) 
	{
        return FALSE;
    }
    m_hThread = ::CreateThread(NULL, 0, DefaultThreadProc, (LPVOID)this, 0, &m_ThreadId);

    return m_hThread != NULL;
#else
	int ret;

	if (-1 == sem_init(&m_hSem, 0, 0)) 
		return FALSE;

	if (-1 == sem_init(&m_hQueueLenSem, 0, 100))
		return FALSE;

	pthread_attr_t attrarg;
	pthread_attr_init(&attrarg);
	int stacksize = ThreadStackSize;
	pthread_attr_setstacksize(&attrarg, stacksize);
	ret = pthread_create(&m_threadId, &attrarg, DefaultThreadProc, (void *)this);
	pthread_attr_destroy(&attrarg);

	return (ret == 0);
#endif
}


// This is the threads message pump.  Here we get and dispatch messages to
// clients thread proc until the client refuses to process a message.
// The client returns a non-zero value to stop the message pump, this
// value becomes the threads exit code.
#ifdef __Win32__
DWORD WINAPI CAkMsgThread::DefaultThreadProc( LPVOID lpParam) 
#else
void *CAkMsgThread::DefaultThreadProc(void *lpParam) 
#endif
{
    CAkMsgThread *lpThis = (CAkMsgThread *)lpParam;
    CMsg msg;
    LRESULT lResult;
	
    // !!!
//    ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	
    // allow a derived class to handle thread startup
    lpThis->OnThreadInit();
	
    do {
        lpThis->GetThreadMsg(&msg);
        lResult = lpThis->ThreadMessageProc(msg.uMsg, msg.dwFlags, msg.lpParam, msg.pEvent);
    } while(lResult == 0L);

#ifdef __Win32__
	return (DWORD)lResult;
#else
	pthread_exit((void *)lResult);
#endif
}


// Block until the next message is placed on the list m_ThreadQueue.
// copies the message to the message pointed to by *pmsg

void CAkMsgThread::GetThreadMsg(CMsg *msg) 
{
    CMsg * pmsg = NULL;
	
    // keep trying until a message appears
    while(TRUE)
	{ 
		{
			CAkAutoLock lck(&m_Lock);

			if (FALSE == m_ThreadQueue.empty())
			{
				pmsg = m_ThreadQueue.front();
				m_ThreadQueue.pop();
#ifdef __Win32__
				ReleaseSemaphore( m_hQueueLenSem ,1, &m_nPreNum);
#else
				sem_post(&m_hQueueLenSem); 
#endif
				break;
			}
			else
			{
				m_lWaiting++;
			}
		}
		// the semaphore will be signalled when it is non-empty
#ifdef __Win32__
		WaitForSingleObject(m_hSem, INFINITE);
#else
		sem_wait(&m_hSem);
#endif	
    }
    // copy fields to caller's CMsg
    *msg = *pmsg;
	
    // this CMsg was allocated by the 'new' in PutThreadMsg
    delete pmsg;
}


void CAkMsgThread::ClearQueue()
{
	CAkAutoLock lck(&m_Lock);
	while (false == m_ThreadQueue.empty())
	{
		CMsg *pMsg = m_ThreadQueue.front();
		delete pMsg;
		m_ThreadQueue.pop();
	}
}

