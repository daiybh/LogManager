// LogManager.cpp : Defines the initialization routines for the DLL.
//
#include "StdAfx.h"

int g_nLogOn = 0;
int g_nMaxLogNum = 50;
int g_nMaxLogSize = 2;

#ifdef __Win32__


#include <afxdllx.h>
#include "Psapi.h"
#include  <io.h>
#include <Tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//int g_nLogOn = 0;
//int g_nMaxLogNum = 50;
//int g_nMaxLogSize = 2;

static AFX_EXTENSION_MODULE LogManagerDLL = { NULL, NULL };

HINSTANCE g_LogManagerHinstance = NULL;

BOOL GetCurDllPath(LPTSTR szPath)
{
	GetModuleFileName(g_LogManagerHinstance,szPath,MAX_PATH);

/*	DWORD_PTR			dw;
	__asm
	{
label1:
		push	EAX
		mov		EAX, label1
		mov		dw, EAX
		pop		EAX
	}
	HANDLE				hToolHelp32Snap = INVALID_HANDLE_VALUE;
	DWORD				dwPID			= GetCurrentProcessId();
	MODULEENTRY32		me32;

	me32.dwSize		= sizeof(MODULEENTRY32); 
	hToolHelp32Snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if(Module32First(hToolHelp32Snap, &me32)) 
	{
		do 
		{ 
			if((DWORD_PTR)me32.modBaseAddr <= dw && (DWORD_PTR)me32.modBaseAddr + me32.modBaseSize > dw)
			{
				_tcscpy(szPath, me32.szExePath);
				break;
			}
		}while(Module32Next(hToolHelp32Snap, &me32));
	}
	if(hToolHelp32Snap)
	{
		::CloseHandle(hToolHelp32Snap);
		hToolHelp32Snap	= NULL;
	}
*/
	return TRUE;
}

CString GetLogManagerDir()
{
	TCHAR cPath[MAX_PATH];
	wmemset(cPath,0,MAX_PATH);
	GetCurDllPath(cPath);

	CString str = cPath;
	str = str.Left(str.ReverseFind('\\'));
	str = str.Left(str.ReverseFind('\\'));

	if(str == _T(""))
	{
		str = _T("C:\\Sobey\\streaming\\server");
	}
	return str;

}

int ReadIniSetting()
{
	CString iniFilePath = GetLogManagerDir();
	iniFilePath += _T("\\ssssdk.ini");
	
	try
	{
		int LogOn = GetPrivateProfileInt(_T("LOG"), _T("LogFileOn"), 0, iniFilePath);
		g_nLogOn = LogOn;

		int LogNum = GetPrivateProfileInt(_T("LOG"), _T("MaxLogNum"), 50, iniFilePath);
		g_nMaxLogNum = LogNum;

		int LogSize = GetPrivateProfileInt(_T("LOG"), _T("MaxLogSize"), 2, iniFilePath);
		g_nMaxLogSize = LogSize;
	}
	catch (...) 
	{
		return FALSE;
	}
	return TRUE;
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("LOGMANAGER.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(LogManagerDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(LogManagerDLL);
		g_LogManagerHinstance = hInstance;


		ReadIniSetting();
		CString strPath = GetLogManagerDir();
		if(strPath == _T(""))
		{
			OutputDebugString(_T("Get Log Dir Failed!"));
		}
		else
		{	
			strPath  = strPath + _T("\\ssslogs");
			if (_taccess(strPath, 0) != 0)
			{
				CreateDirectory(strPath, NULL);
			}
			CString strLogPath;
			strLogPath  = strPath + _T("\\ErrorLog");
			if (_taccess(strLogPath, 0) != 0)
			{
				CreateDirectory(strLogPath, NULL);
			}
			strLogPath  = strPath + _T("\\WarningLog");
			if (_taccess(strLogPath, 0) != 0)
			{
				CreateDirectory(strLogPath, NULL);
			}
			strLogPath  = strPath + _T("\\ProcessLog");
			if (_taccess(strLogPath, 0) != 0)
			{
				CreateDirectory(strLogPath, NULL);
			}
		}
		
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("LOGMANAGER.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(LogManagerDLL);
	}
	return 1;   // ok
}

#endif

