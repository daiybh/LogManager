// SBT_LogManager.cpp: implementation of the SBT_LogManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SBT_LogManager.h"
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

void endl(SBT_LogManager& c,DWORD f)
{
	c.m_pLogManager->endlog(f);
	return;
}

void flush(SBT_LogManager& c,DWORD f)
{
	c.m_pLogManager->flush(f);
	return;
}

SBT_LogManager::SBT_LogManager()
{
	m_pLogManager = new xhLogManager;
}

SBT_LogManager::~SBT_LogManager()
{
	if(m_pLogManager)
	{
		delete m_pLogManager;
		m_pLogManager = NULL;
	}
}

int SBT_LogManager::ConfigName(CComBSTR DllName)
{
	return m_pLogManager->ConfigName((CString)DllName);
}

int SBT_LogManager::ConfigName(HINSTANCE DllName)
{
	return m_pLogManager->ConfigName(DllName);
}

SBT_LogManager& SBT_LogManager::operator<<(void (__cdecl *pfun)(SBT_LogManager&,DWORD dwFlag))
{
	pfun(*this,0);
	return *this;
	
}

SBT_LogManager& SBT_LogManager::operator<(void (__cdecl *pfun)(SBT_LogManager&,DWORD dwFlag))
{
	if(g_nLogOn >= 1)
		pfun(*this,1);
	return *this;
	
}

SBT_LogManager& SBT_LogManager::operator-(void (__cdecl *pfun)(SBT_LogManager&,DWORD dwFlag))
{
	if(g_nLogOn >= 2)
		pfun(*this,2);
	return *this;
	
}

SBT_LogManager& SBT_LogManager::operator<<(int nLog)
{
	*m_pLogManager<<nLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<(int nLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<nLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator-(int nLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-nLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<<(CString strLog)
{
	*m_pLogManager<<strLog;
	return *this;
} 

SBT_LogManager& SBT_LogManager::operator<(CString strLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<strLog;
	return *this;
} 

SBT_LogManager& SBT_LogManager::operator-(CString strLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-strLog;
	return *this;
} 

SBT_LogManager& SBT_LogManager::operator<<(double dbLog)
{
	*m_pLogManager<<dbLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<(double dbLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<dbLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator-(double dbLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-dbLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<<(DWORD dwLog)
{
	*m_pLogManager<<dwLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<(DWORD dwLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<dwLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator-(DWORD dwLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-dwLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<<(__int64 llLog)
{
	*m_pLogManager<<llLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<(__int64 llLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<llLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator-(__int64 llLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-llLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<<(float fLog)
{
	*m_pLogManager<<fLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<(float fLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<fLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator-(float fLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-fLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<<(BYTE byLog)
{
	*m_pLogManager<<byLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<(BYTE byLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<byLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator-(BYTE byLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-byLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<<(char cLog)
{
	*m_pLogManager<<cLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator<(char cLog)
{
	if(g_nLogOn >= 1)
		*m_pLogManager<cLog;
	return *this;
}

SBT_LogManager& SBT_LogManager::operator-(char cLog)
{
	if(g_nLogOn >= 2)
		*m_pLogManager-cLog;
	return *this;
}

