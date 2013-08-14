#ifndef SYNCHRONIZE_H
#define SYNCHRONIZE_H

#include <sys/types.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>

#define INFINITE 0xFFFFFFFF 

class CLock
{
public:
	CLock();
	~CLock();

public:
	void lock();
	void unlock();
    bool islock()
    {
        bool bret= false;

        pthread_mutex_lock(&m_self_mutex);
        bret = m_n_reference_count > 0 ? true : false;
        pthread_mutex_unlock(&m_self_mutex);

        return bret;
    }

public:
	pthread_t get_current_owner();

private:
	pthread_mutex_t m_mutex;
	pthread_mutex_t m_self_mutex;
	pthread_t m_owner_thread_id;
	unsigned long m_n_reference_count;
};

class CCriticalSection
{
public:
	CCriticalSection();
	~CCriticalSection();

public:
	void Lock();
	void Unlock();
    bool Islock();

public:
	pthread_t m_currentOwner;
	bool m_bDelete;
	u_int32_t m_dwReferenceNum;

	void PlusReference() { m_dwReferenceNum++; }
	void SetDeleteFlag() { m_bDelete = true; }
	bool IsNeedDelete() { return (m_bDelete && m_dwReferenceNum==0); }
private:
    CLock m_CritSec;
};

class CSemaphore
{
public:
	CSemaphore(u_int32_t value = 0);
	~CSemaphore();

public:
	int  	set();
	int  	reset();
	int	    check();
	int  	wait(u_int32_t msec = INFINITE);

private:
	sem_t				m_semaphore;
	u_int32_t		m_hold;
};


class CEvent
{
public:
	CEvent(bool manual_reset = false);
	~CEvent();
	
public:
	int  	set();
	int  	reset();
	int 	check() { return m_event.check(); };
	int  	wait(u_int32_t msec);

private:
	CSemaphore     	    m_event;
	bool            	m_manual_reset;
};

#endif
