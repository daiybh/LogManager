// LogProcess.h: interface for the CLogProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGPROCESS_H__9AD2A5BB_A8A5_4571_8096_4B174728DFBA__INCLUDED_)
#define AFX_LOGPROCESS_H__9AD2A5BB_A8A5_4571_8096_4B174728DFBA__INCLUDED_

#ifdef __Win32__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#else

#include "StdioFile.h"
#include "SSPlatformHeader.h"

#endif

class CLogProcess  
{
public:
	CLogProcess();
	virtual ~CLogProcess();
	
	int SetDllName(CString DllName);
    int SetSubDllName(CString SubDllName);
	int WriteLog(CString strLog,int nType,int nMode = 1);//Write Data to File
	int WriteStatLog(char* pLog, int nLog);//Write Statistics Log;
	int FlushLog();//flush Data to File, and Close File
    int OnlyFlush();//only flush data to File;

private:

	int FindTargetFile(int nType);

	int FindStatisticsFile();
	
	CString m_strDllName;
    CString m_strSubDllName;
	CStdioFile  m_stdProcessLogFile;

	int m_nLogType;
	int m_nLogFileTotalNumber;
#ifdef __Win32__
	int m_nMaxLogFileSize;
	SYSTEMTIME m_timeNowFileTime;
#else
	uint32_t m_nMaxLogFileSize;
	struct tm m_timeNowFileTime;
#endif
};

#endif // !defined(AFX_LOGPROCESS_H__9AD2A5BB_A8A5_4571_8096_4B174728DFBA__INCLUDED_)
