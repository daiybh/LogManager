// xhLogManager.h: interface for the xhLogManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XHLOGMANAGER_H__929687E1_0DEB_42F3_BB47_1C6113F3EF8C__INCLUDED_)
#define AFX_XHLOGMANAGER_H__929687E1_0DEB_42F3_BB47_1C6113F3EF8C__INCLUDED_

#ifdef __Win32__

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000

	#include "AFXMT.H"

#endif

#include "LogProcess.h"
#include "LogThread.h"



struct Auto_Unlock 
{
	CCriticalSection* pLock;
	Auto_Unlock(CCriticalSection* pLockIn)
	{
		pLock = pLockIn;
	}
	~Auto_Unlock()
	{
		if(pLock)
			pLock->Unlock();
	}
}; 

struct Auto_lock 
{
	CCriticalSection* pLock;
	Auto_lock(CCriticalSection* pLockIn)
	{
		pLock = pLockIn;
		if(pLock)
			pLock->Lock();
	}
	~Auto_lock()
	{
		if(pLock)
			pLock->Unlock();
	}
}; 

class xhLogManager  
{
public:
	xhLogManager();

	int ConfigName(CString DllName);
#ifdef __Win32__
	int ConfigName(HINSTANCE DllName);
#endif
	int SetWorkMode(int nMode = 0);//0:error log mode , 1:statistics log mode

	int WriteLog(char* pLog, int nLogLen);

	void endlog(DWORD dwFlag);

	void flush(DWORD dwFlag);

	virtual ~xhLogManager();

private:

	int ErrorLogPretreatment();

	int WarningLogPretreatment();
	
	int ProcessLogPretreatment();

	int StatisticsLogPretreatment();

	CString GetTimeStr();
	
	int ReportInfo(LPCTSTR szMsg);
	
	
private:
	
//	CLogProcess m_pErrorLogWriter;
	CLogThread* m_pWarningLogThread;
	CLogThread* m_pProcessoLogThread;
	CLogThread* m_pStatisticsLogThread;
	

	BOOL m_bNeedOutErrorTime;
	BOOL m_bNeedOutWarnTime;
	BOOL m_bNeedOutProTime;

	CAkCritSec m_csLock;
	CString m_DllName;
    CString m_DllSubName;
	int m_nWorkMode;

public:
	
	xhLogManager& operator<<(int value);
	xhLogManager& operator<(int value);
	xhLogManager& operator-(int value);
	
	
	xhLogManager& operator<<(CString strLog);
	xhLogManager& operator<(CString strLog);
	xhLogManager& operator-(CString strLog);
	
	xhLogManager& operator<<(double strLog);
	xhLogManager& operator<(double strLog);
	xhLogManager& operator-(double strLog);
	
	xhLogManager& operator<<(DWORD dwLog);
	xhLogManager& operator<(DWORD dwLog);
	xhLogManager& operator-(DWORD dwLog);
	
	xhLogManager& operator<<(__int64 llLog);
	xhLogManager& operator<(__int64 llLog);
	xhLogManager& operator-(__int64 llLog);
	
	xhLogManager& operator<<(float fLog);
	xhLogManager& operator<(float fLog);
	xhLogManager& operator-(float fLog);
	
	xhLogManager& operator<<(BYTE byLog);
	xhLogManager& operator<(BYTE byLog);
	xhLogManager& operator-(BYTE byLog);
	
	xhLogManager& operator<<(char cLog);
	xhLogManager& operator<(char cLog);
	xhLogManager& operator-(char cLog);
	
};


#endif // !defined(AFX_XHLOGMANAGER_H__929687E1_0DEB_42F3_BB47_1C6113F3EF8C__INCLUDED_)
