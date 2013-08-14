#pragma once   

#ifndef __SSS_LOGMANAGERC_H__
#define __SSS_LOGMANAGERC_H__

#ifdef __linux__
#include <pthread.h>
#include <time.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <stdarg.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <assert.h>


#ifdef __Win32__


#ifdef _SBT_LOG_MANAGER_
#define Xh_LogManager_PortMode __declspec(dllexport)
#else
#define Xh_LogManager_PortMode __declspec(dllimport)
#endif
#else
#define Xh_LogManager_PortMode
	
#endif

#define LogFileNameLen  (1024)
#define LogMsgLen       (8*1024)

enum eLevel {
    SSSDebug = 0x100,
    SSSError,
    SSSWarning,
    SSSProcess,
    SSSScreen,
};

class CAkCritSec;
class SBT_LogManagerU;

class Xh_LogManager_PortMode CSSS_LogManagerC
{
public:
    CSSS_LogManagerC();
    ~CSSS_LogManagerC();
    int WriteLog(const int nLevel, const char *pFile, int nLine, const char * pErrMsg , ...);
    int WriteLog(const int nLevel, const char * pErrMsg , ...);
    int SetDebugLog(int nLevel);
    int ConfigName(const char* name);

private:
    SBT_LogManagerU *m_pLog;

private:
    char* m_pMsg;
    char* m_pLogMsg;

private:
    int InitAKLogEnv();
    int UninitAKLogEnv();

private:
    bool m_bInitOk;
    int m_nLogDebug;
    CAkCritSec* m_lkLog;

};

//extern Xh_LogManager_PortMode CSSS_LogManagerC* pSSS_LogManagerC;
//#ifdef __linux__
//#define SSS_LogDebug(level,args...)         if (pSSS_LogManagerC) pSSS_LogManagerC->WriteLog(CSSS_LogManagerC::NEWLOG,SSSDebug,__FILE__,__LINE__,##args)
//#define SSS_Log(level,args...)              if (pSSS_LogManagerC) pSSS_LogManagerC->WriteLog(CSSS_LogManagerC::NEWLOG,level,__FILE__,__LINE__,##args)
//#define SSS_Log2Screen(args...)             if (pSSS_LogManagerC) pSSS_LogManagerC->WriteLog(CSSS_LogManagerC::NEWLOG,SSSScreen,__FILE__,__LINE__,##args)
//#else
//#define SSS_LogDebug(level,...)             if (pSSS_LogManagerC) pSSS_LogManagerC->WriteLog(CSSS_LogManagerC::NEWLOG,SSSDebug,__FILE__,__LINE__,__VA_ARGS__)
//#define SSS_Log(level,...)                  if (pSSS_LogManagerC) pSSS_LogManagerC->WriteLog(CSSS_LogManagerC::NEWLOG,level,__FILE__,__LINE__,__VA_ARGS__)
//#define SSS_Log2Screen(...)                 if (pSSS_LogManagerC) pSSS_LogManagerC->WriteLog(CSSS_LogManagerC::NEWLOG,SSSScreen,__FILE__,__LINE__,__VA_ARGS__)
//#endif
//#define SetDebugLevel(level)                if (pSSS_LogManagerC) pSSS_LogManagerC->SetDebugLog(level);
//#define SetLogName(name)                    if (pSSS_LogManagerC) pSSS_LogManagerC->ConfigName(name);


#endif





















