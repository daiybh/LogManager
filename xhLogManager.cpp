// xhLogManager.cpp: implementation of the xhLogManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#ifdef __Win32__

#include "Psapi.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include "xhLogManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogProcess g_ErrorLogWriter;
CCriticalSection g_csErrorLogLock;

xhLogManager::xhLogManager()
{
	m_pWarningLogThread = NULL;
	m_pProcessoLogThread = NULL;
	m_pStatisticsLogThread = NULL;
	m_bNeedOutProTime = TRUE;
	m_bNeedOutWarnTime = TRUE;
	m_bNeedOutErrorTime = TRUE;
	m_DllName = _T("NoName");
    m_DllSubName = _T("");
	m_nWorkMode = 0;

#ifndef __Win32__
    if(access("./ssslogs", F_OK) != 0)
        mkdir("./ssslogs", 0755);
#endif
}

xhLogManager::~xhLogManager()
{
	if(m_pWarningLogThread)
	{
		m_pWarningLogThread->FlushToFile();
		m_pWarningLogThread->PutThreadMsg(dfTerminateProcess,0,0);
		m_pWarningLogThread->WaitThreadExit();
		delete m_pWarningLogThread;
		m_pWarningLogThread = NULL;
	}
	if(m_pProcessoLogThread)
	{
		m_pProcessoLogThread->FlushToFile();
		m_pProcessoLogThread->PutThreadMsg(dfTerminateProcess,0,0);
		m_pProcessoLogThread->WaitThreadExit();
		delete m_pProcessoLogThread;
		m_pProcessoLogThread = NULL;
	}
	if(m_pStatisticsLogThread)
	{
		m_pStatisticsLogThread->FlushToFile();
		m_pStatisticsLogThread->PutThreadMsg(dfTerminateProcess,0,0);
		m_pStatisticsLogThread->WaitThreadExit();
		delete m_pStatisticsLogThread;
		m_pStatisticsLogThread = NULL;
	}
}

int xhLogManager::ConfigName(CString DllName)
{
#ifdef __Win32__
    int nSub = DllName.Find(_T("|"));
    if(nSub >= 0)
    {
        m_DllName = DllName.Left(nSub);
        m_DllSubName = DllName.Right(DllName.GetLength() - nSub - 1);
    }
    else
    {
        m_DllName = DllName;
    }
#else
    int nSub = DllName.find(_T(":"));
    if(nSub >= 0)
    {
        m_DllName = DllName.substr(0, nSub);
        m_DllSubName = DllName.substr(nSub + 1);
    }
    else
    {
        m_DllName = DllName;
    }
#endif
	return 1;
}

int xhLogManager::SetWorkMode(int nMode /* = 0 */)
{
	m_nWorkMode = nMode;
	return 1;
}

int xhLogManager::WriteLog(char* pLog, int nLogLen)
{
	StatisticsLogPretreatment();
	StatisticsLogPara* pPara = new StatisticsLogPara;
	pPara->pLog = new char [nLogLen + 1];
	memcpy(pPara->pLog, pLog, nLogLen);
	pPara->pLog[nLogLen] = 0;
	pPara->nLog = nLogLen;
	m_pStatisticsLogThread->PutThreadMsg(dfWriteStatisticsLog,0,pPara,NULL);
	return 1;
}

#ifdef __Win32__
int xhLogManager::ConfigName(HINSTANCE DllName)
{
	TCHAR cPath[MAX_PATH];
	DWORD dwID = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwID);
	GetModuleFileNameEx(hProcess, (HINSTANCE)DllName, cPath, MAX_PATH);
	m_DllName = cPath;
	m_DllName = m_DllName.Left(m_DllName.ReverseFind('.'));
	m_DllName = m_DllName.Right((m_DllName.GetLength()) - (m_DllName.ReverseFind('\\')) - 1);
	CloseHandle(hProcess);
	return 1;
}
#endif

void xhLogManager::endlog(DWORD dwFlag)
{
	
	if(dwFlag == 0)
	{
		Auto_lock Elk(&g_csErrorLogLock);
		ErrorLogPretreatment();
		if(m_bNeedOutErrorTime == FALSE)
		{
			Auto_Unlock Euk(&g_csErrorLogLock);
			m_bNeedOutErrorTime = TRUE;
			g_ErrorLogWriter.FlushLog();
		}		
	}
	else if (dwFlag == 1)
	{
		CAkAutoLock Wlk(&m_csLock);
		WarningLogPretreatment();
		if(m_pWarningLogThread)
		{
			if(m_bNeedOutWarnTime == FALSE)
			{
				m_csLock.Unlock();
				m_bNeedOutWarnTime = TRUE;
			}
           // m_pWarningLogThread->FlushToFile();
            m_pWarningLogThread->PutThreadMsg(dfFushLogFile,0,NULL,NULL);
		}
	}
	else if(dwFlag == 2)
	{
		CAkAutoLock Plk(&m_csLock);
		ProcessLogPretreatment();
		if(m_pProcessoLogThread)
		{
			if(m_bNeedOutProTime == FALSE)
			{
				m_csLock.Unlock();
				m_bNeedOutProTime = TRUE;
			}
            //m_pProcessoLogThread->FlushToFile();
            m_pProcessoLogThread->PutThreadMsg(dfFushLogFile,0,NULL,NULL);
		}
	}
	return ;
}

void xhLogManager::flush(DWORD dwFlag)
{
	if(dwFlag == 0)
	{
		Auto_lock Elk(&g_csErrorLogLock);
		ErrorLogPretreatment();
		g_ErrorLogWriter.FlushLog();
	}
	else if (dwFlag == 1)
	{
		CAkAutoLock Wlk(&m_csLock);
		WarningLogPretreatment();
		if(m_pWarningLogThread)
		{
			m_pWarningLogThread->FlushToFile();
		}
	}
	else if(dwFlag == 2)
	{
		CAkAutoLock Plk(&m_csLock);
		ProcessLogPretreatment();
		if(m_pProcessoLogThread)
		{
			m_pProcessoLogThread->FlushToFile();
		}
	}
	return;
}

xhLogManager& xhLogManager::operator<<(int nLog)
{
	Auto_lock Elk(&g_csErrorLogLock);
	ErrorLogPretreatment();
	if(m_bNeedOutErrorTime)
	{
		g_csErrorLogLock.Lock();
		m_bNeedOutErrorTime = FALSE;
		g_ErrorLogWriter.WriteLog(GetTimeStr(),0,0);
	}

#ifdef __Win32__
	CString strTemp;
	strTemp.Format(_T("%d "),nLog);
#else
	char strTemp[64];
	sprintf(strTemp, "%d ", nLog);	
#endif
	g_ErrorLogWriter.WriteLog(strTemp, 0);
	return *this;
}

xhLogManager& xhLogManager::operator<(int nLog)
{
	CAkAutoLock lk(&m_csLock);
	WarningLogPretreatment();
	if(m_pWarningLogThread)
	{
		if(m_bNeedOutWarnTime)
		{
			m_csLock.Lock();
			m_bNeedOutWarnTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%d "),nLog);
#else
		char strTemp[32];
		sprintf(strTemp, "%d ", nLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator-(int nLog)
{
	CAkAutoLock lk(&m_csLock);
	ProcessLogPretreatment();
	if(m_pProcessoLogThread)
	{
		if(m_bNeedOutProTime)
		{
			m_csLock.Lock();
			m_bNeedOutProTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%d "),nLog);
#else
		char strTemp[32];
		sprintf(strTemp, "%d ", nLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator<<(CString strLog)
{
	Auto_lock Elk(&g_csErrorLogLock);
	ErrorLogPretreatment();
	if(m_bNeedOutErrorTime)
	{
		g_csErrorLogLock.Lock();
		m_bNeedOutErrorTime = FALSE;
		g_ErrorLogWriter.WriteLog(GetTimeStr(),0,0);
	}
	CString strTemp = _T("  ");
	strTemp = strTemp + strLog;

	g_ErrorLogWriter.WriteLog(strTemp, 0);

	return *this;
} 

xhLogManager& xhLogManager::operator<(CString strLog)
{
	CAkAutoLock lk(&m_csLock);
	WarningLogPretreatment();
	if(m_pWarningLogThread)
	{
		if(m_bNeedOutWarnTime)
		{
			m_csLock.Lock();
			m_bNeedOutWarnTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTime,NULL);
		}
		CString strTemp = _T("  ");
		strTemp = strTemp + strLog;
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTemp,NULL);
	}
	return *this;
} 

xhLogManager& xhLogManager::operator-(CString strLog)
{
	CAkAutoLock lk(&m_csLock);
	ProcessLogPretreatment();
	if(m_pProcessoLogThread)
	{
		if(m_bNeedOutProTime)
		{
			m_csLock.Lock();
			m_bNeedOutProTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTime,NULL);
		}
		CString strTemp = _T("  ");
		strTemp = strTemp + strLog;
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTemp,NULL);
	}
	return *this;
} 

xhLogManager& xhLogManager::operator<<(double dbLog)
{
	Auto_lock Elk(&g_csErrorLogLock);
	ErrorLogPretreatment();
	if(m_bNeedOutErrorTime)
	{
		g_csErrorLogLock.Lock();
		m_bNeedOutErrorTime = FALSE;
		g_ErrorLogWriter.WriteLog(GetTimeStr(),0,0);
	}
#ifdef __Win32__
	CString strTemp;
	strTemp.Format(_T("%lf "),dbLog);
#else
    char strTemp[32];
    sprintf(strTemp, "%lf ", dbLog);
#endif
	g_ErrorLogWriter.WriteLog(strTemp,0);

	return *this;
}

xhLogManager& xhLogManager::operator<(double dbLog)
{
	CAkAutoLock lk(&m_csLock);
	WarningLogPretreatment();
	if(m_pWarningLogThread)
	{
		if(m_bNeedOutWarnTime)
		{
			m_csLock.Lock();
			m_bNeedOutWarnTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%lf "),dbLog);
#else
   	 	char strTemp[32];
    	sprintf(strTemp, "%lf ", dbLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator-(double dbLog)
{
	CAkAutoLock lk(&m_csLock);
	ProcessLogPretreatment();
	if(m_pProcessoLogThread)
	{
		if(m_bNeedOutProTime)
		{
			m_csLock.Lock();
			m_bNeedOutProTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%lf "),dbLog);
#else
        char strTemp[32];
        sprintf(strTemp, "%lf ", dbLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator<<(DWORD dbLog)
{
	Auto_lock Elk(&g_csErrorLogLock);
	ErrorLogPretreatment();
	if(m_bNeedOutErrorTime)
	{
		g_csErrorLogLock.Lock();
		m_bNeedOutErrorTime = FALSE;
		g_ErrorLogWriter.WriteLog(GetTimeStr(),0,0);
	}
#ifdef __Win32__
	CString strTemp;
	strTemp.Format(_T("%u "),dbLog);
#else
	char strTemp[32];
    sprintf(strTemp, "%u ", dbLog);
#endif
	g_ErrorLogWriter.WriteLog(strTemp,0);
	return *this;
}

xhLogManager& xhLogManager::operator<(DWORD dbLog)
{
	CAkAutoLock lk(&m_csLock);
	WarningLogPretreatment();
	if(m_pWarningLogThread)
	{
		if(m_bNeedOutWarnTime)
		{
			m_csLock.Lock();
			m_bNeedOutWarnTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%u "),dbLog);
#else
    	char strTemp[32];
        sprintf(strTemp, "%u ", dbLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator-(DWORD dbLog)
{
	CAkAutoLock lk(&m_csLock);
	ProcessLogPretreatment();
	if(m_pProcessoLogThread)
	{
		if(m_bNeedOutProTime)
		{
			m_csLock.Lock();
			m_bNeedOutProTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%u "),dbLog);
#else
		char strTemp[32];
        sprintf(strTemp, "%u ", dbLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator<<(__int64 llLog)
{
	Auto_lock Elk(&g_csErrorLogLock);
	ErrorLogPretreatment();
	if(m_bNeedOutErrorTime)
	{
		g_csErrorLogLock.Lock();
		m_bNeedOutErrorTime = FALSE;

		g_ErrorLogWriter.WriteLog(GetTimeStr(),0,0);
	}
	
#ifdef __Win32__
	CString strTemp;
	strTemp.Format(_T("%I64d "),llLog);
#else
	char strTemp[32];
#if __WORDSIZE == 64
    sprintf(strTemp, "%ld ", llLog);
#else
    sprintf(strTemp, "%lld ", llLog);
#endif
#endif
	g_ErrorLogWriter.WriteLog(strTemp, 0);
	return *this;
}

xhLogManager& xhLogManager::operator<(__int64 llLog)
{
	CAkAutoLock lk(&m_csLock);
	WarningLogPretreatment();
	if(m_pWarningLogThread)
	{
		if(m_bNeedOutWarnTime)
		{
			m_csLock.Lock();
			m_bNeedOutWarnTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%I64d "),llLog);
#else
        char strTemp[32];
#if __WORDSIZE == 64
        sprintf(strTemp, "%ld ", llLog);
#else
        sprintf(strTemp, "%lld ", llLog);
#endif
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator-(__int64 llLog)
{
	CAkAutoLock lk(&m_csLock);
	ProcessLogPretreatment();
	if(m_pProcessoLogThread)
	{
		if(m_bNeedOutProTime)
		{
			m_csLock.Lock();
			m_bNeedOutProTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%I64d "),llLog);
#else
        char strTemp[32];
#if __WORDSIZE == 64
        sprintf(strTemp, "%ld ", llLog);
#else
        sprintf(strTemp, "%lld ", llLog);
#endif
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator<<(float fLog)
{
	Auto_lock Elk(&g_csErrorLogLock);
	ErrorLogPretreatment();
	if(m_bNeedOutErrorTime)
	{
		g_csErrorLogLock.Lock();
		m_bNeedOutErrorTime = FALSE;

		g_ErrorLogWriter.WriteLog(GetTimeStr(),0,0);
	}
#ifdef __Win32__
	CString strTemp;
	strTemp.Format(_T("%f "),fLog);
#else
	char strTemp[32];
    sprintf(strTemp, "%f ", fLog);
#endif
	g_ErrorLogWriter.WriteLog(strTemp,0);
	return *this;
}

xhLogManager& xhLogManager::operator<(float fLog)
{
	CAkAutoLock lk(&m_csLock);
	WarningLogPretreatment();
	if(m_pWarningLogThread)
	{
		if(m_bNeedOutWarnTime)
		{
			m_csLock.Lock();
			m_bNeedOutWarnTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%f "),fLog);
#else
		char strTemp[32];
        sprintf(strTemp, "%f ", fLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator-(float fLog)
{
	CAkAutoLock lk(&m_csLock);
	ProcessLogPretreatment();
	if(m_pProcessoLogThread)
	{
		if(m_bNeedOutProTime)
		{
			m_csLock.Lock();
			m_bNeedOutProTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%f "),fLog);
#else
	    char strTemp[32];
        sprintf(strTemp, "%f ", fLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator<<(BYTE byLog)
{
	Auto_lock Elk(&g_csErrorLogLock);
	ErrorLogPretreatment();
	if(m_bNeedOutErrorTime)
	{
		g_csErrorLogLock.Lock();
		m_bNeedOutErrorTime = FALSE;
		g_ErrorLogWriter.WriteLog(GetTimeStr(),0,0);
	}
#ifdef __Win32__
	CString strTemp;
	strTemp.Format(_T("%c "),byLog);
#else
	char strTemp[32];
    sprintf(strTemp, "%c ", byLog);
#endif
	g_ErrorLogWriter.WriteLog(strTemp,0);
	return *this;
}

xhLogManager& xhLogManager::operator<(BYTE byLog)
{
	CAkAutoLock lk(&m_csLock);
	WarningLogPretreatment();
	if(m_pWarningLogThread)
	{
		if(m_bNeedOutWarnTime)
		{
			m_csLock.Lock();
			m_bNeedOutWarnTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%c "),byLog);
#else
		char strTemp[32];
        sprintf(strTemp, "%c ", byLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator-(BYTE byLog)
{
	CAkAutoLock lk(&m_csLock);
	ProcessLogPretreatment();
	if(m_pProcessoLogThread)
	{
		if(m_bNeedOutProTime)
		{
			m_csLock.Lock();
			m_bNeedOutProTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%c "),byLog);
#else
	    char strTemp[32];
        sprintf(strTemp, "%c ", byLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator<<(char cLog)
{
	Auto_lock Elk(&g_csErrorLogLock);
	ErrorLogPretreatment();
	if(m_bNeedOutErrorTime)
	{
		g_csErrorLogLock.Lock();
		m_bNeedOutErrorTime = FALSE;
		g_ErrorLogWriter.WriteLog(GetTimeStr(),0,0);
	}
#ifdef __Win32__
	CString strTemp;
	strTemp.Format(_T("%c "),cLog);
#else
    char strTemp[32];
    sprintf(strTemp, "%c ", cLog);
#endif
	g_ErrorLogWriter.WriteLog(strTemp,0);
	return *this;
}

xhLogManager& xhLogManager::operator<(char cLog)
{
	CAkAutoLock lk(&m_csLock);
	WarningLogPretreatment();
	if(m_pWarningLogThread)
	{
		if(m_bNeedOutWarnTime)
		{
			m_csLock.Lock();
			m_bNeedOutWarnTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%c "),cLog);
#else
        char strTemp[32];
        sprintf(strTemp, "%c ", cLog);
#endif

		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pWarningLogThread->PutThreadMsg(dfWriteWarningLog,0,pTemp,NULL);
	}
	return *this;
}

xhLogManager& xhLogManager::operator-(char cLog)
{
	CAkAutoLock lk(&m_csLock);
	ProcessLogPretreatment();
	if(m_pProcessoLogThread)
	{
		if(m_bNeedOutProTime)
		{
			m_csLock.Lock();
			m_bNeedOutProTime = FALSE;
			LogPara *pTime = new LogPara;
			pTime->strLog = GetTimeStr();
			pTime->nMode = 0;
			m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTime,NULL);
		}
#ifdef __Win32__
		CString strTemp;
		strTemp.Format(_T("%c "),cLog);
#else
        char strTemp[32];
        sprintf(strTemp, "%c ", cLog);
#endif
		LogPara *pTemp = new LogPara;
		pTemp->strLog = strTemp;
		m_pProcessoLogThread->PutThreadMsg(dfWriteProcessLog,0,pTemp,NULL);
	}
	return *this;
}

int xhLogManager::ErrorLogPretreatment()
{
#ifndef __Win32__
    if(access("./ssslogs/ErrorLog", F_OK) != 0)
        mkdir("./ssslogs/ErrorLog", 0755);
#endif

	return 1;
}

int xhLogManager::WarningLogPretreatment()
{
	if(m_pWarningLogThread == NULL)
	{
#ifndef __Win32__
        if(access("./ssslogs/WarningLog", F_OK) != 0)
            mkdir("./ssslogs/WarningLog", 0755);
#endif

		int nRet;
		m_pWarningLogThread = new CLogThread;
		m_pWarningLogThread->OnThreadInit();
		nRet = m_pWarningLogThread->CreateThread();
		if(nRet <= 0)
		{
#ifdef __Win32__
			DWORD dwError = GetLastError();
			CString strErrorCode;
			strErrorCode.Format(_T("<SDK-ERROR>: LogManagerWarningThreadCreateFailed , ErrorCode:%d"),dwError);
			OutputDebugString(strErrorCode);
#else
			int dwError = errno;
            char errorCode[1024];

            sprintf(errorCode, "<ERROR>: LogManagerWarningThreadCreateFailed , ErrorCode:%d", dwError);
			printf("%s", strerror(dwError));
			string strErrorCode = errorCode;
#endif
			ReportInfo(strErrorCode);
			if(m_pWarningLogThread)
			{
				delete m_pWarningLogThread;
				m_pWarningLogThread = NULL;
			}
			return 0;
		}
#ifdef __Win32__
		m_pWarningLogThread->SetDllName(m_DllName);
        m_pWarningLogThread->SetSubDllName(m_DllSubName);
#else
		m_pWarningLogThread->SetDllName(m_DllName.c_str());
        m_pWarningLogThread->SetSubDllName(m_DllSubName.c_str());
#endif
	}
	return 1;
}

int xhLogManager::ProcessLogPretreatment()
{
	if(m_pProcessoLogThread == NULL)
	{
#ifndef __Win32__
        if(access("./ssslogs/ProcessLog", F_OK) != 0)
            mkdir("./ssslogs/ProcessLog", 0755);
#endif
		int nRet;
		m_pProcessoLogThread = new CLogThread;
		m_pProcessoLogThread->OnThreadInit();
		nRet = m_pProcessoLogThread->CreateThread();
		if(nRet <= 0)
		{
#ifdef __Win32__
			DWORD dwError = GetLastError();
			CString strErrorCode;
			strErrorCode.Format(_T("<SDK-ERROR>: LogManagerProcessThreadCreateFailed , ErrorCode:%d"),dwError);
			OutputDebugString(strErrorCode);
#else
			int dwError = errno;
            char errorCode[1024];

			sprintf(errorCode, "<SDK-ERROR>: LogManagerProcessThreadCreateFailed , ErrorCode:%d", dwError);
			printf("%s", strerror(dwError));
            string strErrorCode = errorCode;
#endif
			ReportInfo(strErrorCode);
			if(m_pProcessoLogThread)
			{
				delete m_pProcessoLogThread;
				m_pProcessoLogThread = NULL;
			}
			return 0;
		}
#ifdef __Win32__
		m_pProcessoLogThread->SetDllName(m_DllName);
        m_pProcessoLogThread->SetSubDllName(m_DllSubName);
#else
		m_pProcessoLogThread->SetDllName(m_DllName.c_str());
        m_pProcessoLogThread->SetSubDllName(m_DllSubName.c_str());
#endif
	}
	return 1;
}

int xhLogManager::StatisticsLogPretreatment()
{
	if(m_pStatisticsLogThread == NULL)
	{
		int nRet;
		m_pStatisticsLogThread = new CLogThread;
		m_pStatisticsLogThread->OnThreadInit();
		nRet = m_pStatisticsLogThread->CreateThread();
		if(nRet <= 0)
		{
#ifdef __Win32__
			DWORD dwError = GetLastError();
			CString strErrorCode;
			strErrorCode.Format(_T("<SDK-ERROR>: LogManagerStatisticsThreadCreateFailed , ErrorCode:%d"),dwError);
			OutputDebugString(strErrorCode);
#else
			int dwError = errno;
            char errorCode[1024];

			sprintf(errorCode, "<SDK-ERROR>: LogManagerStatisticsThreadCreateFailed , ErrorCode:%d", dwError);
			printf("%s", strerror(dwError));
            string strErrorCode = errorCode;
#endif
			ReportInfo(strErrorCode);
			if(m_pStatisticsLogThread)
			{
				delete m_pStatisticsLogThread;
				m_pStatisticsLogThread = NULL;
			}
			return 0;
		}
#ifdef __Win32__
		m_pStatisticsLogThread->SetDllName(m_DllName);
        m_pStatisticsLogThread->SetSubDllName(m_DllSubName);
#else
		m_pStatisticsLogThread->SetDllName(m_DllName.c_str());
        m_pStatisticsLogThread->SetSubDllName(m_DllSubName.c_str());
#endif
	}
	return 1;
}

CString xhLogManager::GetTimeStr()
{
#ifdef __Win32__
	DWORD dwthread = GetCurrentThreadId();
	DWORD dwProcId = GetCurrentProcessId();
	SYSTEMTIME localTime;
	GetLocalTime(&localTime);
	CString strTime;
	strTime.Format(_T("\n<SDKLOG>[%d.%d.%d--%2d:%2d:%2d:%3d][pid:%d][tid:%d]  "),localTime.wYear, localTime.wMonth, localTime.wDay, 
		localTime.wHour,localTime.wMinute, localTime.wSecond,localTime.wMilliseconds,
		dwProcId,dwthread);
#else
	pthread_t dwthread = pthread_self();
	pid_t dwProcId = getpid();
	struct tm tm;   

    timeval temptime;
    gettimeofday(&temptime, NULL);
	time_t now = time(NULL);   
	localtime_r(&now, &tm);
	char strTime[1024];	
    sprintf(strTime, "\n<LOG>[%04d.%02d.%02d-%02d.%02d.%02d:%03ld][pid:%d][tid:%lu]", 
		    1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, temptime.tv_usec/1000, 
            dwProcId, dwthread);
#endif
	return strTime;
}

int xhLogManager::ReportInfo(LPCTSTR szMsg)
{
#ifdef __Win32__
	HANDLE h; 
	
    h = RegisterEventSource(NULL,  // uses local computer 
		_T("SDK_LogManager"));    // source name 
    if (h == NULL) 
        return 0;
	
    if (!ReportEvent(h,           // event log handle 
		EVENTLOG_INFORMATION_TYPE,  // event type 
		0,                    // category zero 
		0,				 // event identifier 
		NULL,                 // no user security identifier 
		1,                    // one substitution string 
		0,                    // no data 
		&szMsg,                // pointer to string array 
		NULL))                // pointer to data 
        return 0;
	
    DeregisterEventSource(h); 
#endif
	return 1;
}
