// SBT_LogManager.h: interface for the SBT_LogManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SBT_LOGMANAGERU_H__C500FEE8_BD08_497B_BB42_4B75255BB976__INCLUDED_)
#define AFX_SBT_LOGMANAGERU_H__C500FEE8_BD08_497B_BB42_4B75255BB976__INCLUDED_

#ifdef __Win32__

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
		#pragma comment(lib, "sssLogManagerUd.lib") 
		#else
		#pragma comment(lib, "sssLogManagerU.lib")
		#endif
	#endif
	#include "ATLBASE.H"	
#else


#include "SSPlatformHeader.h"

#include <string>

using namespace std;

#endif // __Win32__

class xhLogManager;

class Xh_LogManager_PortMode SBT_LogManagerU  
{
public:
	SBT_LogManagerU();

#ifdef __Win32__	
	int ConfigName(HINSTANCE DllName);
#endif
	int ConfigName(CComBSTR DllName);

	int SetWorkMode(int nMode = 0);//0:error log mode , 1:statistics log mode
	int WriteLog(char* pLog, int nLogLen);
	
	virtual ~SBT_LogManagerU();

public:
	// use << to output ErrorLog
	// use <  to output WarningLog
	// use -  to output ProcessLog
	// use endl for a newline
	// use flush to close file
	SBT_LogManagerU& operator<<(void (__cdecl *pfun)(SBT_LogManagerU&,DWORD dwFlag));
	SBT_LogManagerU& operator<(void (__cdecl *pfun)(SBT_LogManagerU&,DWORD dwFlag));
	SBT_LogManagerU& operator-(void (__cdecl *pfun)(SBT_LogManagerU&,DWORD dwFlag));
	
	SBT_LogManagerU& operator<<(int nLog);
	SBT_LogManagerU& operator<(int nLog);
	SBT_LogManagerU& operator-(int nLog);
	
	SBT_LogManagerU& operator<<(float fLog);
	SBT_LogManagerU& operator<(float fLog);
	SBT_LogManagerU& operator-(float fLog);
	
	SBT_LogManagerU& operator<<(CComBSTR strLog);
	SBT_LogManagerU& operator<(CComBSTR strLog);
	SBT_LogManagerU& operator-(CComBSTR strLog);
	
	SBT_LogManagerU& operator<<(double dbLog);
	SBT_LogManagerU& operator<(double dbLog);
	SBT_LogManagerU& operator-(double dbLog);
	
	SBT_LogManagerU& operator<<(DWORD dwLog);
	SBT_LogManagerU& operator<(DWORD dwLog);
	SBT_LogManagerU& operator-(DWORD dwLog);
	
	SBT_LogManagerU& operator<<(__int64 llLog);
	SBT_LogManagerU& operator<(__int64 llLog);
	SBT_LogManagerU& operator-(__int64 llLog);
	
	SBT_LogManagerU& operator<<(BYTE byLog);
	SBT_LogManagerU& operator<(BYTE byLog);
	SBT_LogManagerU& operator-(BYTE byLog);

	SBT_LogManagerU& operator<<(char byLog);
	SBT_LogManagerU& operator<(char byLog);
	SBT_LogManagerU& operator-(char byLog);
	
public:
	
	xhLogManager* m_pLogManager;

};
void Xh_LogManager_PortMode endl(SBT_LogManagerU& c,DWORD f); 
void Xh_LogManager_PortMode flush(SBT_LogManagerU& c,DWORD f); 

#endif // !defined(AFX_SBT_LOGMANAGER_H__C500FEE8_BD08_497B_BB42_4B75255BB976__INCLUDED_)
