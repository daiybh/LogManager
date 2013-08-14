#ifndef _SDAMS_MSGTHREAD_H_
#define _SDAMS_MSGTHREAD_H_
//create by lp
//------------------------------------------------------------------------------
// File: MsgThrd.h
//
// Desc: DirectShow base classes - provides support for a worker thread 
//       class to which one can asynchronously post messages.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifdef __Win32__
	#pragma warning(disable : 4786)
#endif

#ifdef __linux__

#include "Synchronize.h"

#include <pthread.h>
#include <semaphore.h>

#include "SSPlatformHeader.h"


#endif



#include <queue>
#include <string>

using namespace std;

#define EXECUTE_ASSERT(_x_) ((void)(_x_))

// wrapper for event objects
class CAkEvent
{
	
    // make copy constructor and assignment operator inaccessible
	
    CAkEvent(const CAkEvent &refEvent);
    CAkEvent &operator=(const CAkEvent &refEvent);
	
protected:

#ifdef __Win32__
    HANDLE m_hEvent;
#else
	CEvent *m_hEvent;
#endif	
	
public:
    CAkEvent(BOOL fManualReset = FALSE)
	{
	#ifdef __Win32__
		m_hEvent = CreateEvent(NULL, fManualReset, FALSE, NULL);
	#else
		m_hEvent = new CEvent(fManualReset);
	#endif
	}
   ~CAkEvent()
	{
		if(m_hEvent) 
		{
		#ifdef __Win32__
		    EXECUTE_ASSERT(CloseHandle(m_hEvent));
		#else
			delete m_hEvent;
			m_hEvent = NULL;			
		#endif
		}
	}
	 
    // Cast to HANDLE - we don't support this as an lvalue

#ifdef __Win32__
    operator HANDLE () const { return m_hEvent; }
#endif

    void Set() 
	{ 
	#ifdef __Win32__
		EXECUTE_ASSERT(SetEvent(m_hEvent)); 
	#else
		m_hEvent->set();
	#endif
	}
	
    BOOL Wait(DWORD dwTimeout = INFINITE) 
	{
	#ifdef __Win32__
		return (WaitForSingleObject(m_hEvent, dwTimeout) == WAIT_OBJECT_0);
	#else
		return m_hEvent->wait(dwTimeout);
	#endif
	}
    void Reset() 
	{ 
	#ifdef __Win32__
		ResetEvent(m_hEvent); 
	#else
		m_hEvent->reset();
	#endif
	}
	
    BOOL Check() 
	{ 
	#ifdef __Win32__
		return Wait(0); 
	#else
		return m_hEvent->wait(0);
	#endif

	}
};

// wrapper for whatever critical section we have
class CAkCritSec {
	
    // make copy constructor and assignment operator inaccessible
	
    CAkCritSec(const CAkCritSec &refCritSec);
    CAkCritSec &operator=(const CAkCritSec &refCritSec);

#ifdef __Win32__
    CRITICAL_SECTION m_CritSec;
#else
	CCriticalSection m_CritSec;
#endif
	
public:
	
#ifdef __Win32__
    DWORD   m_currentOwner;
#else
	pthread_t m_currentOwner;
#endif

    DWORD   m_lockCount;
    BOOL    m_fTrace;        // Trace this one
public:
    CAkCritSec();
    ~CAkCritSec();
    void Lock();
    void Unlock();
	
	BOOL IsLock();
};

// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class CAkAutoLock {
	
    // make copy constructor and assignment operator inaccessible
	
    CAkAutoLock(const CAkAutoLock &refAutoLock);
    CAkAutoLock &operator=(const CAkAutoLock &refAutoLock);
	
protected:
    CAkCritSec * m_pLock;
	
public:
    CAkAutoLock(CAkCritSec * plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };
	
    ~CAkAutoLock() {
        m_pLock->Unlock();
    };
};

// Message class - really just a structure.
//
class CMsg {
public:
    UINT uMsg;
    DWORD dwFlags;
    LPVOID lpParam;
    CAkEvent *pEvent;


    CMsg(UINT u, DWORD dw, LPVOID lp, CAkEvent *pEvnt)
        : uMsg(u), dwFlags(dw), lpParam(lp), pEvent(pEvnt) {}

    CMsg()
        : uMsg(0), dwFlags(0L), lpParam(NULL), pEvent(NULL) {}
};

// This is the actual thread class.  It exports all the usual thread control
// functions.  The created thread is different from a normal WIN32 thread in
// that it is prompted to perform particaular tasks by responding to messages
// posted to its message queue.
//
class CAkMsgThread {
protected:
#ifdef __Win32__
    static DWORD WINAPI DefaultThreadProc(LPVOID lpParam);


    DWORD               m_ThreadId;
    HANDLE              m_hThread;
#else
    static void *DefaultThreadProc(void *lpParam);
    pthread_t	m_threadId;
#endif

protected:

    // if you want to override GetThreadMsg to block on other things
    // as well as this queue, you need access to this
//    CGenericList<CMsg>        m_ThreadQueue;
	std::queue <CMsg *>			  m_ThreadQueue;
    CAkCritSec                m_Lock;
	
#ifdef __Win32__	
    HANDLE                    m_hSem;
	HANDLE                    m_hQueueLenSem;
#else
	sem_t					  m_hSem;
	sem_t					  m_hQueueLenSem;
#endif

    LONG                      m_lWaiting;

public:
    CAkMsgThread()
		:
#ifdef __Win32__
        m_ThreadId(0),
        m_hThread(NULL),
		m_hQueueLenSem(NULL),
        m_hSem(NULL),
#endif
		m_lWaiting(0)
    {
    }

    virtual ~CAkMsgThread();
    // override this if you want to block on other things as well
    // as the message loop
    void virtual GetThreadMsg(CMsg *msg);

    // override this if you want to do something on thread startup
    virtual void OnThreadInit() {  };

    BOOL CreateThread();
	
#ifdef __Win32__
    BOOL WaitForThreadExit(LPDWORD lpdwExitCode) {
        if (m_hThread != NULL) {
            WaitForSingleObject(m_hThread, INFINITE);
            return GetExitCodeThread(m_hThread, lpdwExitCode);
        }
        return FALSE;
    }


    DWORD ResumeThread() {
        return ::ResumeThread(m_hThread);
    }

    DWORD SuspendThread() {
        return ::SuspendThread(m_hThread);
    }

    int GetThreadPriority() {
        return ::GetThreadPriority(m_hThread);
    }

    BOOL SetThreadPriority(int nPriority) {
        return ::SetThreadPriority(m_hThread, nPriority);
    }

    HANDLE GetThreadHandle() {
        return m_hThread;
    }

    DWORD GetThreadId() {
        return m_ThreadId;
    }
#else
	pthread_t GetThreadId() 
	{
		return m_threadId;
	}
	
    pthread_t GetThreadHandle() {
        return m_threadId;
    }
#endif

#ifdef __Win32__
    void PutThreadMsg(UINT uMsg, DWORD dwMsgFlags,
                      LPVOID lpMsgParam, CAkEvent *pEvent = NULL) 
	{
        CAkAutoLock * plck = NULL;
        CMsg* pMsg = new CMsg(uMsg, dwMsgFlags, lpMsgParam, pEvent);

		WaitForSingleObject(m_hQueueLenSem, INFINITE);	

		plck = new CAkAutoLock(&m_Lock) ;
		
        m_ThreadQueue.push(pMsg);
        
        if (m_lWaiting != 0)
		{	// ×èÈûµÈ´ýÖÐ¡£¡£¡£
            ReleaseSemaphore(m_hSem, m_lWaiting, 0);
            m_lWaiting = 0;
        }

		delete plck;
    }
#else
    void PutThreadMsg(UINT uMsg, DWORD dwMsgFlags,
                      LPVOID lpMsgParam, CAkEvent *pEvent = NULL) 
	{
		CMsg* pMsg = new CMsg(uMsg, dwMsgFlags, lpMsgParam, pEvent);

		sem_wait(&m_hQueueLenSem);

		{
			CAkAutoLock lck(&m_Lock);

			m_ThreadQueue.push(pMsg);

			if (m_lWaiting != 0)
			{	
				sem_post(&m_hSem);
				m_lWaiting = 0;
			}
		}
	}
#endif

#ifdef __Win32__	
	void WaitThreadExit()
	{
		if(m_hThread != NULL) 
		{
			WaitForSingleObject(m_hThread, INFINITE);
			EXECUTE_ASSERT(CloseHandle(m_hThread));
			m_hThread = NULL ;
		}
	}
#else
    void WaitThreadExit()
    {
        int nret = 0;;
        if (m_threadId != 0) 
        {
            void *status;
            nret = pthread_join(m_threadId, &status);
            //GetExitCodeThread(m_hThread, lpdwExitCode);
            if(0 != nret)
            {
                fprintf(stderr, "pthread join thread %lu error\n", m_threadId);
            }
            m_threadId = 0;
        }
    }
#endif
    // This is the function prototype of the function that the client
    // supplies.  It is always called on the created thread, never on
    // the creator thread.
    //
    virtual LRESULT ThreadMessageProc(
        UINT uMsg, DWORD dwFlags, LPVOID lpParam, CAkEvent *pEvent) = 0;

	void ClearQueue();
public:
	long m_nPreNum;
};

#ifdef __Win32__

struct stFlagsEx
{
	stFlagsEx(HANDLE hSock, DWORD dwSourceType)
	{
		hSocket = hSock;
		dwMsgSourceType = dwSourceType;
	}

	HANDLE hSocket;
	DWORD dwMsgSourceType;
};

#endif



#endif // _SDAMS_MSGTHREAD_H_
