// LogThread.cpp: implementation of the CLogThread class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"


#include "LogThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogThread::CLogThread()
{
	m_nLogType = dfWriteWarningLog;
	m_pLogWriter = new CLogProcess;
}

CLogThread::~CLogThread()
{
	if(m_pLogWriter)
	{
		delete m_pLogWriter;
		m_pLogWriter = NULL;
	}
}

int CLogThread::SetDllName(CString DllName)
{
	return m_pLogWriter->SetDllName(DllName);
}

int CLogThread::SetSubDllName(CString SubDllName)
{
    return m_pLogWriter->SetSubDllName(SubDllName);
}

int CLogThread::FlushToFile()
{
	CAkEvent FlushEvent;
	//HANDLE kkk = m_hThread;
	FlushEvent.Reset();
	PutThreadMsg(m_nLogType, 0, NULL, &FlushEvent);
	FlushEvent.Wait();
	m_pLogWriter->FlushLog();
	
	return 1;
}

LRESULT CLogThread::ThreadMessageProc(UINT uMsg, DWORD dwFlags, LPVOID lpParam, CAkEvent *pEvent)
{
	
	LRESULT  lr = S_OK ;
	BOOL  br  = TRUE;
	
	switch(uMsg)
	{
	case dfWriteWarningLog:
		br = _WriteWarningLog((LogPara*)lpParam);
		if(pEvent)
		{
			pEvent->Set();
		}
		break;
	case dfWriteProcessLog:
		br = _WriteProcessLog((LogPara*)lpParam);
		if(pEvent)
		{
			pEvent->Set();
		}
		break;
	case dfWriteStatisticsLog:
		br = _WriteStatisticsLog((StatisticsLogPara*)lpParam);
		if(pEvent)
		{
			pEvent->Set();
		}
		break;
    case dfFushLogFile:
        br = _FlushLogFile((LogPara*)lpParam);
        if(pEvent)
        {
            pEvent->Set();
        }
        break;
	case dfTerminateProcess:
		br = _TerminateProcess((BOOL)dwFlags);
		break;
	default:
		br = FALSE;
#ifdef __Win32__
		ASSERT(FALSE);
#else
		assert(FALSE);
#endif
		break;
	}
	
	return lr ;
}

int CLogThread::_WriteWarningLog(LogPara* pPara)
{
	m_nLogType = dfWriteWarningLog;
	if(pPara)
	{
		m_pLogWriter->WriteLog(pPara->strLog,1,pPara->nMode);
		delete pPara;
		pPara = NULL;
	}
	return 1;
}

int CLogThread::_WriteProcessLog(LogPara* pPara)
{
	m_nLogType = dfWriteProcessLog;
	if(pPara)
	{
		m_pLogWriter->WriteLog(pPara->strLog,2,pPara->nMode);
		delete pPara;
		pPara = NULL;
	}
	return 1;
}

int CLogThread::_FlushLogFile(LogPara* pPara)
{
    if(m_pLogWriter)
    {
        m_pLogWriter->OnlyFlush();
    }
    return 1;
}

int CLogThread::_WriteStatisticsLog(StatisticsLogPara* pPara)
{
	m_nLogType = dfWriteStatisticsLog;
	if(pPara)
	{
		m_pLogWriter->WriteStatLog(pPara->pLog,pPara->nLog);
		if(pPara->pLog)
		{
			delete [] pPara->pLog;
		}
		delete pPara;
		pPara = NULL;
	}
	return 1;
}

int CLogThread::_TerminateProcess(BOOL bExtForce)
{
	BOOL lr  = TRUE ;
	
	TRACE(_T(" _TerminateProcess \n"));
	if(!bExtForce)
	{
	#ifdef __Win32__
		ExitThread(0);
	#else
		pthread_exit(NULL);
	#endif
	}
	else
	{
	#ifdef __Win32__
		TerminateThread(GetThreadHandle(),1);
	#else
		pthread_cancel(GetThreadHandle());
	#endif	
	}	
	return lr ;
}
