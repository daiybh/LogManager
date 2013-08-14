// LogThread.h: interface for the CLogThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGTHREAD_H__84A44087_96FD_4A9B_9D1E_2443EED9A98A__INCLUDED_)
#define AFX_LOGTHREAD_H__84A44087_96FD_4A9B_9D1E_2443EED9A98A__INCLUDED_

#ifdef __Win32__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#endif

#include "MsgThread.h"
#include "LogProcess.h"

#include <string>
#include <sys/types.h>

using namespace std;

#define		dfWriteWarningLog     		  (0x1) //启动一个写日志请求
#define		dfWriteProcessLog     		  (0x2) //启动一个写日志请求
#define     dfTerminateProcess            (0x3) //终止本处理线程的运行
#define     dfWriteStatisticsLog		  (0x4) //启动一个写日志请求
#define     dfFushLogFile	        	  (0x5) //用线程FLUSH日志文件
 

struct LogPara 
{
	CString strLog;
	int nMode;
	LogPara()
	{
		strLog = _T("");
		nMode = 1;
	}
};

struct StatisticsLogPara
{
	char* pLog;
	int nLog;
	StatisticsLogPara()
	{
		pLog = NULL;
		nLog = 0;
	}
};

class CLogThread : public CAkMsgThread  
{
public:
	CLogThread();
	virtual ~CLogThread();

	int SetDllName(CString DllName);
    int SetSubDllName(CString SubDllName);
	LRESULT ThreadMessageProc(UINT uMsg, DWORD dwFlags, LPVOID lpParam, CAkEvent *pEvent);
	int FlushToFile();


private:

	int _WriteWarningLog(LogPara* pPara);
	int _WriteProcessLog(LogPara* pPara);
	int _WriteStatisticsLog(StatisticsLogPara* pPara);
    int _FlushLogFile(LogPara* pPara);
	int _TerminateProcess(BOOL bExtForce);


private:
	
	int m_nLogType;
	CLogProcess* m_pLogWriter;

};

#endif // !defined(AFX_LOGTHREAD_H__84A44087_96FD_4A9B_9D1E_2443EED9A98A__INCLUDED_)
