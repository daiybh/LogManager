#include "Synchronize.h"
#include <assert.h>

CLock::CLock()
{
	if (0 != pthread_mutex_init(&m_mutex , NULL))
	{
		printf("CLock::CLock pthread_mutex_init error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
	
	if (0 != pthread_mutex_init(&m_self_mutex, NULL))
	{
		printf("CLock::CLock pthread_mutex_init error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}

	m_owner_thread_id = 0;
	m_n_reference_count = 0;
}

CLock::~CLock()
{
	if(0 != pthread_mutex_destroy(&m_mutex))
	{
		printf("CLock::~CLock pthread_mutex_destroy error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
	
	if(0 != pthread_mutex_destroy(&m_self_mutex))
	{
		printf("CLock::~CLock pthread_mutex_destroy error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
}

void CLock::lock()
{
    int r = 0;
	pthread_t current_thread_id = pthread_self();
	
	if(0 != (r = pthread_mutex_lock(&m_self_mutex)))
	{
		printf("CLock::lock pthread_mutex_lock error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
	
	if (pthread_equal(m_owner_thread_id, current_thread_id) != 0 && m_n_reference_count > 0)
	{
		m_n_reference_count++;
		
		if(0 != pthread_mutex_unlock(&m_self_mutex))
		{
			printf("CLock::unlock pthread_mutex_unlock error<%d>.\n", errno);
			assert(0);
			//exit(-1);
		}
		
		return;
	}
	
	if(0 == pthread_mutex_trylock(&m_mutex))
	{
		if(0 == m_n_reference_count) 
		{
			m_n_reference_count++;
			/* we now own it for the first time.  Set owner information */
			m_owner_thread_id = current_thread_id;
		}
		
		if(0 != pthread_mutex_unlock(&m_self_mutex))
		{
			printf("CLock::unlock pthread_mutex_unlock error<%d>.\n", errno);
			assert(0);
			//exit(-1);
		}
		
		return;
	}
	
	if(0 != pthread_mutex_unlock(&m_self_mutex))
	{
		printf("CLock::unlock pthread_mutex_unlock error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
	
	if(0 != pthread_mutex_lock(&m_mutex))
	{
		printf("CLock::lock pthread_mutex_lock error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
	
	if(0 != pthread_mutex_lock(&m_self_mutex))
	{
		printf("CLock::lock pthread_mutex_lock error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
	
	if(0 == m_n_reference_count) 
	{
		m_n_reference_count++;
		/* we now own it for the first time.  Set owner information */
		m_owner_thread_id = current_thread_id;
	}
	
	if(0 != pthread_mutex_unlock(&m_self_mutex))
	{
		printf("CLock::unlock pthread_mutex_unlock error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
}

void CLock::unlock()
{
	pthread_t current_thread_id = pthread_self();
	
	if(0 != pthread_mutex_lock(&m_self_mutex))
	{
		printf("CLock::lock pthread_mutex_lock error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
	
	if (m_n_reference_count > 0) m_n_reference_count--;
	
	if (m_n_reference_count > 0 && pthread_equal(current_thread_id, m_owner_thread_id) != 0)
	{
		if(0 != pthread_mutex_unlock(&m_self_mutex))
		{
			printf("CLock::unlock pthread_mutex_unlock error<%d>.\n", errno);
			assert(0);
			//exit(-1);
		}
		
		return;
	}
	
	if(0 == m_n_reference_count)
	{
		m_owner_thread_id = 0;
	}
	
	if(0 != pthread_mutex_unlock(&m_self_mutex))
	{
		printf("CLock::unlock pthread_mutex_unlock error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
	
	if(0 != pthread_mutex_unlock(&m_mutex))
	{
		printf("CLock::unlock pthread_mutex_unlock error<%d>.\n", errno);
		assert(0);
		//exit(-1);
	}
}

pthread_t CLock::get_current_owner()
{
	return m_owner_thread_id;
}

CCriticalSection::CCriticalSection() 
{
	m_bDelete = 0;
	m_dwReferenceNum = 0;
}

CCriticalSection::~CCriticalSection() 
{
}

void CCriticalSection::Lock() 
{
	m_CritSec.lock();
	m_currentOwner = m_CritSec.get_current_owner();
}

void CCriticalSection::Unlock() 
{
	m_CritSec.unlock();
	m_currentOwner = m_CritSec.get_current_owner();
}

bool CCriticalSection::Islock()
{
    return m_CritSec.islock();
}


CSemaphore::CSemaphore(u_int32_t value)
{
	m_hold = value;

	if (0 != sem_init(&m_semaphore, 0, value))
	{
		printf("CSemaphore::CSemaphore sem_init error<%d>.\n", errno);
	}
}

CSemaphore::~CSemaphore()
{
	if(0 != sem_destroy(&m_semaphore))
	{
		printf("CSemaphore::~CSemaphore sem_destroy error<%d>.\n", errno);
	}
}

int CSemaphore::set()
{
	int ret;

	ret = sem_post(&m_semaphore);  // ++

	return ret;
}

int CSemaphore::reset()
{
	if(0 != sem_destroy(&m_semaphore))
	{
		return -1;
	}

	if (0 != sem_init(&m_semaphore, 0, m_hold))
	{
		return -1;
	}
	
	return 0;
}

int CSemaphore::check()
{
	return wait(0);
}

int CSemaphore::wait(u_int32_t msec)
{
	int ret = 0;
	u_int32_t wait_time = 0;
	
	if(INFINITE == msec)
	{
		while(0 != (ret = sem_wait(&m_semaphore)) && EINTR == errno);
	}
	else
	{
		struct timespec delay_time;
		
		delay_time.tv_sec = 0;
		delay_time.tv_nsec = 1000000; // 1ms = 1000us // 1us = 1000ns
		
		while (true)
		{
			ret = sem_trywait(&m_semaphore);
			
			if (0 == ret)
				break;

			if (EAGAIN != errno)
			{
				ret = -1;
				break;
			}

			if (msec <= wait_time)
			{
				ret = -1;
				break;
			}
			
			nanosleep(&delay_time, NULL);
			wait_time++;
		}
	}

	return ret;
}

CEvent::CEvent(bool manual_reset)
{
	m_manual_reset = manual_reset;
}

CEvent::~CEvent()
{
}

int CEvent::set()
{
	int ret;
	
	ret = m_event.set();
	
	return ret;
}

int CEvent::reset()
{
	int ret;
	
	ret = m_event.reset();

	return ret;
}


int CEvent::wait(u_int32_t msec)
{
	int ret;
	
	ret = m_event.wait(msec);
	
	if (m_manual_reset)
	{
		m_event.set();
	}

	return ret;
}
