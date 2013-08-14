#ifndef __SS_PALTFORM_HEADER_H__
#define __SS_PALTFORM_HEADER_H__

#ifdef __linux__

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string>

using namespace std;

typedef uint32_t    DWORD;
typedef int64_t     __int64;
typedef uint8_t     BYTE;
typedef uint8_t     byte;
typedef bool        BOOL;
typedef uint32_t    UINT;
typedef long        LONG;
typedef long        LRESULT;
typedef int64_t     LONGLONG;
typedef int64_t     LARGE_INTEGER;

#define LPVOID      void*
#define LPCTSTR     string

#ifndef TRUE
#define TRUE        true
#endif

#ifndef FALSE
#define FALSE       false
#endif

#define _T 
#define CString     string
#define CComBSTR    string
#define S_OK        0
#define INVALID_HANDLE_VALUE NULL

#ifndef ASSERT
#define ASSERT(x)   assert(x)
#endif

#define TRACE(x)    printf(x)
#define Xh_LogManager_PortMode 
#define SBT_EasyFile_ExportMode  
#define SBT_NoBufferFile_ExportMode

#define __cdecl 

#endif

#endif
