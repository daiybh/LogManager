// SBT_LogManager.cpp: implementation of the SBT_LogManager class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "SSS_LogManagerU.h"
#include "xhLogManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int g_nLogOn;
extern int g_nMaxLogNum;
extern int g_nMaxLogSize;
#ifdef __linux__
bool g_bLogReadedIni = 0;
#include "SSS_IniReader.h"
CAkCritSec g_csLogIniLock;
#endif

void endl(SBT_LogManagerU& c,DWORD f)
{
	c.m_pLogManager->endlog(f);
	return;
}

void flush(SBT_LogManagerU& c,DWORD f)
{
	c.m_pLogManager->flush(f);
	return;
}

SBT_LogManagerU::SBT_LogManagerU()
{
#ifdef __linux__
	if(g_bLogReadedIni == 0)
	{
		CAkAutoLock IniLock(&g_csLogIniLock);
		if(g_bLogReadedIni == 0)
		{
			printf("LogManager Get Ini Para\n");
			IniReader* pIniReader = IniReader::GetShareInstance();
			int nRet = 0;
			char cValue[1024];
			memset(cValue,0,1024);
			nRet = pIniReader->GetParaValue("LOG","LogFileOn",cValue);
			if(nRet > 0)
			{
				int nValue = atoi(cValue);
				g_nLogOn = nValue;
			}
			else
			{
				printf("Get Ini LogFileOn Failed!\n");
			}
			memset(cValue,0,1024);
			nRet = pIniReader->GetParaValue("LOG","MaxLogNum",cValue);
			if(nRet > 0)
			{
				int nValue = atoi(cValue);
				g_nMaxLogNum = nValue;
			}
			else
			{
				printf("Get Ini MaxLogNum Failed!\n");
			}
			memset(cValue,0,1024);
			nRet = pIniReader->GetParaValue("LOG","MaxLogSize",cValue);
			if(nRet > 0)
			{
				int nValue = atoi(cValue);
				g_nMaxLogSize = nValue;
			}
			else
			{
				printf("Get Ini MaxLogSize Failed!\n");
			}
			g_bLogReadedIni = 1;
		}
	}
#endif
	m_pLogManager = new xhLogManager;
}

SBT_LogManagerU::~SBT_LogManagerU()
{
	if(m_pLogManager)
	{
		delete m_pLogManager;
		m_pLogManager = NULL;
	}
}

int SBT_LogManagerU::ConfigName(CComBSTR DllName)
{
	return m_pLogManager->ConfigName((CString)DllName);
}

#ifdef __Win32__
int SBT_LogManagerU::ConfigName(HINSTANCE DllName)
{
	return m_pLogManager->ConfigName(DllName);
}
#endif
int SBT_LogManagerU::SetWorkMode(int nMode /* = 0 */)
{
	return m_pLogManager->SetWorkMode(nMode);
}

int SBT_LogManagerU::WriteLog(char* pLog, int nLogLen)
{
	return m_pLogManager->WriteLog(pLog,nLogLen);
}

SBT_LogManagerU& SBT_LogManagerU::operator<<(void (__cdecl *pfun)(SBT_LogManagerU&,DWORD dwFlag))
{
	pfun(*this,0);
	return *this;
	
}

SBT_LogManagerU& SBT_LogManagerU::operator<(void (__cdecl *pfun)(SBT_LogManagerU&,DWORD dwFlag))
{
	if(g_nLogOn >= 1)
		pfun(*this,1);
	return *this;
	
}

SBT_LogManagerU& SBT_LogManagerU::operator-(void (__cdecl *pfun)(SBT_LogManagerU&,DWORD dwFlag))
{
	if(g_nLogOn >= 2)
		pfun(*this,2);
	return *this;
	
}

SBT_LogManagerU& SBT_LogManagerU::operator<<(int nLog)
{
	*m_pLogManager<<nLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<(int nLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<nLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator-(int nLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-nLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<<(CComBSTR strLog)
{
	*m_pLogManager<<(CString)strLog;
	return *this;
} 

SBT_LogManagerU& SBT_LogManagerU::operator<(CComBSTR strLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<(CString)strLog;
	return *this;
} 

SBT_LogManagerU& SBT_LogManagerU::operator-(CComBSTR strLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-(CString)strLog;
	return *this;
} 

SBT_LogManagerU& SBT_LogManagerU::operator<<(double dbLog)
{
	*m_pLogManager<<dbLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<(double dbLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<dbLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator-(double dbLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-dbLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<<(DWORD dwLog)
{
	*m_pLogManager<<dwLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<(DWORD dwLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<dwLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator-(DWORD dwLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-dwLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<<(__int64 llLog)
{
	*m_pLogManager<<llLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<(__int64 llLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<llLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator-(__int64 llLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-llLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<<(float fLog)
{
	*m_pLogManager<<fLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<(float fLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<fLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator-(float fLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-fLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<<(BYTE byLog)
{
	*m_pLogManager<<byLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<(BYTE byLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<byLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator-(BYTE byLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-byLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<<(char cLog)
{
	*m_pLogManager<<cLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator<(char cLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<cLog;
	return *this;
}

SBT_LogManagerU& SBT_LogManagerU::operator-(char cLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-cLog;
	return *this;
}

