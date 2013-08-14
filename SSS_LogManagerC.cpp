#include "StdAfx.h"
#include "SSS_LogManagerC.h"
#include <stdlib.h>
#include "SSS_LogManagerU.h"
#include "LogProcess.h"
#include "MsgThread.h"

#define DEBUG_LOG_DK 1
#ifdef __Win32__
#define snprintf _snprintf
#else
#include "SSS_IniReader.h"
#endif

//class CSSS_LogManagerCInit
//{
//public:
//    CSSS_LogManagerCInit();
//    ~CSSS_LogManagerCInit();
//};
//
//static CSSS_LogManagerCInit sCSSS_LogManagerCInit;
//
//Xh_LogManager_PortMode CSSS_LogManagerC* pSSS_LogManagerC = NULL;
//CSSS_LogManagerCInit::CSSS_LogManagerCInit()
//{
//    //printf("%s\n", __func__);
//    if (pSSS_LogManagerC) {
//        delete(pSSS_LogManagerC);
//        pSSS_LogManagerC = NULL;
//    }
//    pSSS_LogManagerC = new CSSS_LogManagerC;
//}
//
//CSSS_LogManagerCInit::~CSSS_LogManagerCInit()
//{
//    //printf("%s\n", __func__);
//    if (pSSS_LogManagerC) {
//        delete (pSSS_LogManagerC);
//        pSSS_LogManagerC = NULL;
//    }
//}

//////////////////////////////////////////////////////////////////////////
CSSS_LogManagerC::CSSS_LogManagerC()
{
    m_bInitOk = false;
    m_lkLog = new CAkCritSec;
    CAkAutoLock lk(m_lkLog);
    //IniReader* pIniReader = IniReader::GetShareInstance();
    //char* theIniPath = "log.ini";
    //if(pIniReader->Init(theIniPath) <= 0) {
    //    fprintf(stderr, "Read Ini File Failed!\n");
    //}
    m_pMsg = new char [LogMsgLen+10];
    memset(m_pMsg, 0, LogMsgLen+10);
    m_pLogMsg = new char [LogMsgLen+10];
    memset(m_pLogMsg, 0, LogMsgLen+10);

    m_pLog = new SBT_LogManagerU;
    m_pLog->ConfigName("_log_default_");
    
    m_nLogDebug = 0;
    InitAKLogEnv();
    m_bInitOk = true;
}

CSSS_LogManagerC::~CSSS_LogManagerC()
{
    m_bInitOk = false;
    {CAkAutoLock lk(m_lkLog);
    UninitAKLogEnv();

    if (m_pLog) {
        delete m_pLog;
        m_pLog = NULL;
    }
    if(m_pMsg){
        delete [] m_pMsg;
        m_pMsg = NULL;
    }
    if(m_pLogMsg){
        delete [] m_pLogMsg;
        m_pLogMsg = NULL;
    }}
    if (m_lkLog) {
        delete (m_lkLog);
        m_lkLog = NULL;
    }
}


int CSSS_LogManagerC::InitAKLogEnv()
{
    return 0;
}

int CSSS_LogManagerC::UninitAKLogEnv()
{
    return 0;
}

int CSSS_LogManagerC::WriteLog(const int nLevel, const char * pErrMsg , ...)
{
    if (!m_bInitOk) {
        return 0;
    }
    CAkAutoLock lk(m_lkLog);
    memset(m_pMsg, 0, LogMsgLen);

    va_list arg_ptr;
    va_start(arg_ptr, pErrMsg);
    vsnprintf(m_pMsg, LogMsgLen, pErrMsg, arg_ptr);
    va_end(arg_ptr);

    switch (nLevel) {
    case SSSDebug:
        *m_pLog -m_pMsg -endl;
        printf("%s\n", m_pMsg);
        break;
    case SSSError:
        *m_pLog <<m_pMsg <<endl;
        break;
    case SSSWarning:
        *m_pLog <m_pMsg <endl;
        break;
    case SSSProcess:
        *m_pLog -m_pMsg -endl;
        break;
    case SSSScreen:
        printf("%s\n", m_pMsg);
        break;
    default:
        *m_pLog -m_pMsg -endl;
        break;
    }
    return 0;
}

int CSSS_LogManagerC::WriteLog(const int nLevel, const char *pFile, int nLine, const char * pErrMsg , ...)
{
    if (!m_bInitOk) {
        return 0;
    }
    CAkAutoLock lk(m_lkLog);
    memset(m_pMsg, 0, LogMsgLen);
    memset(m_pLogMsg, 0, LogMsgLen);

    va_list arg_ptr;
    va_start(arg_ptr, pErrMsg);
    vsnprintf(m_pMsg, LogMsgLen, pErrMsg, arg_ptr);

    switch (m_nLogDebug) {
    case 2:
        snprintf(m_pLogMsg, LogMsgLen, "%s, %s:%d", m_pMsg, pFile, nLine);
        fprintf(stderr, "%s", m_pLogMsg);
        break;
    case 1:
        snprintf(m_pLogMsg, LogMsgLen, "%s, %s:%d", m_pMsg, pFile, nLine);
        break;
    case 0:
        snprintf(m_pLogMsg, LogMsgLen, "%s",  m_pMsg);
    default:
        break;
    }
    va_end(arg_ptr);

    switch (nLevel) {
    case SSSDebug:
        *m_pLog -m_pLogMsg -endl;
        printf("%s\n", m_pLogMsg);
        break;
    case SSSError:
        *m_pLog <<m_pLogMsg <<endl;
        break;
    case SSSWarning:
        *m_pLog <m_pLogMsg <endl;
        break;
    case SSSProcess:
        *m_pLog -m_pLogMsg -endl;
        break;
    case SSSScreen:
        printf("%s\n", m_pLogMsg);
        break;
    default:
        *m_pLog -m_pLogMsg -endl;
        break;
    }
    return 0 ;
}

int CSSS_LogManagerC::SetDebugLog(int nLevel)
{
    CAkAutoLock lk(m_lkLog);
    m_nLogDebug = nLevel;
    return 0;
}

int CSSS_LogManagerC::ConfigName(const char* name)
{
    m_pLog->ConfigName(name);
    return 0;
}


