// SBT_LogManager.h: interface for the SBT_LogManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SBT_LOGMANAGER_H__C500FEE8_BD08_497B_BB42_4B75255BB976__INCLUDED_)
#define AFX_SBT_LOGMANAGER_H__C500FEE8_BD08_497B_BB42_4B75255BB976__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _SBT_LOG_MANAGER_
#define Xh_LogManager_PortMode __declspec(dllexport)
#else
#define Xh_LogManager_PortMode __declspec(dllimport)
#endif

/////////////////////////// lib related /////////////////////////////////
#ifndef _SBT_LOG_MANAGER_
#ifdef _DEBUG   
#pragma comment(lib, "LogManagerd.lib") 
#else
#pragma comment(lib, "LogManager.lib")
#endif
#endif


#include "ATLBASE.H"

class xhLogManager;

class Xh_LogManager_PortMode SBT_LogManager  
{
public:
	SBT_LogManager();
	
	int ConfigName(HINSTANCE DllName);

	int ConfigName(CComBSTR DllName);
	
	virtual ~SBT_LogManager();

public:
	// use << to output ErrorLog
	// use <  to output WarningLog
	// use -  to output ProcessLog
	// use endl for a newline
	// use flush to close file
	SBT_LogManager& operator<<(void (__cdecl *pfun)(SBT_LogManager&,DWORD dwFlag));
	SBT_LogManager& operator<(void (__cdecl *pfun)(SBT_LogManager&,DWORD dwFlag));
	SBT_LogManager& operator-(void (__cdecl *pfun)(SBT_LogManager&,DWORD dwFlag));
	
	SBT_LogManager& operator<<(int nLog);
	SBT_LogManager& operator<(int nLog);
	SBT_LogManager& operator-(int nLog);
	
	SBT_LogManager& operator<<(float fLog);
	SBT_LogManager& operator<(float fLog);
	SBT_LogManager& operator-(float fLog);
	
	SBT_LogManager& operator<<(CString strLog);
	SBT_LogManager& operator<(CString strLog);
	SBT_LogManager& operator-(CString strLog);
	
	SBT_LogManager& operator<<(double dbLog);
	SBT_LogManager& operator<(double dbLog);
	SBT_LogManager& operator-(double dbLog);
	
	SBT_LogManager& operator<<(DWORD dwLog);
	SBT_LogManager& operator<(DWORD dwLog);
	SBT_LogManager& operator-(DWORD dwLog);
	
	SBT_LogManager& operator<<(__int64 llLog);
	SBT_LogManager& operator<(__int64 llLog);
	SBT_LogManager& operator-(__int64 llLog);
	
	SBT_LogManager& operator<<(BYTE byLog);
	SBT_LogManager& operator<(BYTE byLog);
	SBT_LogManager& operator-(BYTE byLog);

	SBT_LogManager& operator<<(char byLog);
	SBT_LogManager& operator<(char byLog);
	SBT_LogManager& operator-(char byLog);
	
public:
	
	xhLogManager* m_pLogManager;

};
void Xh_LogManager_PortMode endl(SBT_LogManager& c,DWORD f); 
void Xh_LogManager_PortMode flush(SBT_LogManager& c,DWORD f); 

#endif // !defined(AFX_SBT_LOGMANAGER_H__C500FEE8_BD08_497B_BB42_4B75255BB976__INCLUDED_)
