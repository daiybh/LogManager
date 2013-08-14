// LogProcess.cpp: implementation of the CLogProcess class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "LogProcess.h"


#ifdef __Win32__

#include "psapi.h"
#include  <io.h>

#else

#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern CString GetLogManagerDir();

extern int g_nMaxLogNum;
extern int g_nMaxLogSize;
CLogProcess::CLogProcess()
{
    m_nLogType =0 ;
    m_nLogFileTotalNumber = g_nMaxLogNum;
    m_nMaxLogFileSize = g_nMaxLogSize * 1024 * 1024;
    
#ifdef __Win32__
    memset(&m_timeNowFileTime, 0 , sizeof(SYSTEMTIME));
#else
    memset(&m_timeNowFileTime, 0 , sizeof(struct tm));
#endif
}

CLogProcess::~CLogProcess()
{
    FlushLog();
    if(m_stdProcessLogFile.m_pStream)
    {
        m_stdProcessLogFile.Close();
    }
}

int CLogProcess::SetDllName(CString DllName)
{
    m_strDllName = DllName;
    return 1;
}

int CLogProcess::SetSubDllName(CString SubDllName)
{
#ifdef __Win32__
    if(SubDllName.GetLength() == 0)
#else
    if(SubDllName.size() == 0)
#endif
    {
        m_strSubDllName = SubDllName;
    }
    else
    {
#ifdef __Win32__
        m_strSubDllName = _T(" ") + SubDllName;
#else
        m_strSubDllName = _T("_") + SubDllName;
#endif
    }
    
    return 1;
}
    
int CLogProcess::WriteLog(CString strLog, int nType, int nMode)
{ 
    int nRet = 1;

#ifdef __Win32__
    try
#endif
    {
        DWORD dwFileSize = 0;
//        DWORD dwFileSizeHigh = 0;
        if(m_stdProcessLogFile.m_pStream)
            dwFileSize = m_stdProcessLogFile.GetLength();
        if(m_stdProcessLogFile.m_pStream == NULL || 
            (nMode == 0 && dwFileSize >= m_nMaxLogFileSize))

        {
            nRet = FindTargetFile(nType);
        }

        if(nRet)
        {
    
#ifdef __Win32__
            m_stdProcessLogFile.WriteString(strLog);
#else
            m_stdProcessLogFile.WriteString(strLog.c_str());
#endif
        }
    }
#ifdef __Win32__
    catch (CMemoryException* e)
    {
        nRet = -1;
        printf("=========LogMemoryException=========");
    }
    catch (CFileException* e)
    {
        nRet = -1;
        printf("=========LogFileException=========");
    }
#endif

    return nRet;
}

int CLogProcess::OnlyFlush()
{
#ifdef __Win32__
    try
#endif
    {
        m_stdProcessLogFile.Flush();
    }
#ifdef __Win32__
    catch (CMemoryException* e)
    {
        printf("=========LogMemoryException=========");
    }
    catch (CFileException* e)
    {
        printf("=========LogFileException=========");
    }
#endif

    return 1;
}

int CLogProcess::FlushLog()
{
#ifdef __Win32__
    try
#endif
    {
        m_stdProcessLogFile.Flush();
        if(m_stdProcessLogFile.m_pStream)
        {
            m_stdProcessLogFile.Close();
        }
    }
#ifdef __Win32__
    catch (CMemoryException* e)
    {
        printf("=========LogMemoryException=========");
    }
    catch (CFileException* e)
    {
        printf("=========LogFileException=========");
    }
#endif

    return 1;
}

int CLogProcess::FindTargetFile(int nType)
{
    int nRet =  1;
    if(m_stdProcessLogFile.m_pStream)
    {
        m_stdProcessLogFile.Flush();
        m_stdProcessLogFile.Close();
    }

#ifdef __Win32__    
    CString strDllPath = GetLogManagerDir()+_T("\\");
    CString strLogPath = strDllPath + _T("ssslogs\\");
    CString strLogFile = strLogPath;
#else
    char tmpPath[1024];
    getcwd(tmpPath, sizeof(tmpPath));
    
    string strDllPath(tmpPath);
    string strLogPath = strDllPath + "/ssslogs/";
    string strLogFile = strLogPath;
#endif
    switch(nType) 
    {
    case 0:
        {
#ifdef __Win32__
        strLogPath += _T("ErrorLog\\");
        TCHAR cPath[MAX_PATH];
        DWORD dwID = GetCurrentProcessId();
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwID);
        GetModuleFileNameEx(hProcess, NULL, cPath, MAX_PATH);
        CString strFileName =  cPath;
        strFileName = strFileName.Left(strFileName.ReverseFind('.'));
        strFileName = strFileName.Right((strFileName.GetLength()) - (strFileName.ReverseFind('\\')) - 1);
        strLogFile = strLogPath + strFileName;
        strLogFile += _T(" ");
        CloseHandle(hProcess);
#else
        strLogPath += "ErrorLog/";
        
        char cPath[1024];
        char buf[1024];
        pid_t pid = getpid();
        sprintf(cPath, "/proc/%d/exe", pid);
        
        memset(buf, 0, 1024);
        int re = readlink(cPath, buf, 1024);
        if (re == -1) {
            m_stdProcessLogFile.Close();
            return false;
        }

        char* ptr = strrchr(buf, '/');
        if (ptr) {
            ++ ptr;
        } else {
            ptr = buf;
        }
        char strFileName[1024];
        sprintf(strFileName, "%s", ptr);
    
        strLogFile = strLogPath + strFileName;
#endif
        }
        break;
    case 1:
#ifdef __Win32__
        strLogPath += _T("WarningLog\\");
        strLogFile = strLogPath + m_strDllName;
        strLogFile += _T(" ");
#else
        strLogPath += "WarningLog/";
        strLogFile = strLogPath + m_strDllName;
#endif
        break;
    case 2:
#ifdef __Win32__
        strLogPath += _T("ProcessLog\\");
        strLogFile = strLogPath + m_strDllName;
        strLogFile += _T(" ");
#else
        strLogPath += "ProcessLog/";
        strLogFile = strLogPath + m_strDllName;
#endif
        break;
    default:
        break;
    }

#ifdef __Win32__
    CFileFind tempFind;  
    CString strExtendName = _T("*.log");
    
    CString strFindDel = strLogFile + strExtendName;
    CString strFind = strLogFile + _T("*") + m_strSubDllName + _T(".log");
    BOOL IsFinded = tempFind.FindFile(strFind.GetBuffer(strFind.GetLength()));  
    CString strLastFileName = _T("");
    CString strNewFilename = _T("");
    int nFileNum = 0;

    while (IsFinded)  
    {  
        IsFinded = tempFind.FindNextFile(); 
        if (!tempFind.IsDots())  
        {  
            CString strName = tempFind.GetFileName();
            if (nFileNum == 0)
                strNewFilename = strName;
            if (strName > strNewFilename)
                strNewFilename = strName;
           // nFileNum++;
        }      
    }  
    tempFind.Close(); 

    IsFinded = tempFind.FindFile(strFindDel.GetBuffer(strFindDel.GetLength()));  
    while (IsFinded)  
    {  
        IsFinded = tempFind.FindNextFile(); 
        if (!tempFind.IsDots())  
        {  
            CString strName = tempFind.GetFileName();
            if (nFileNum == 0)
                strLastFileName = strName;
            if (strName < strLastFileName )
                strLastFileName =  strName;
            nFileNum++;
        }      
    }  
    tempFind.Close(); 
#else
    int nFileNum = 0;
    string strLastFileName;
    string strNewFilename;
    DIR *dir = opendir(strLogPath.c_str());
    if (dir) {
        struct dirent *entry = NULL;
        while((entry = readdir(dir)) != NULL)
        {
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char* ptr = strrchr(entry->d_name, '.');
            if(!ptr || (ptr && strcmp(ptr, ".log") != 0))
                continue;   
            if(m_strSubDllName.size() != 0)
            {
                string sub = m_strSubDllName + ".log";
                char* subptr = ptr - m_strSubDllName.size();
                if(!subptr || subptr < entry->d_name || (subptr && strcmp(subptr, sub.c_str()) != 0))
                    continue; 
            }
            if(strncmp(entry->d_name, m_strDllName.c_str(), m_strDllName.size()) != 0)
                continue;

            if(nFileNum == 0)
            {
                //strLastFileName = entry->d_name;
                strNewFilename = entry->d_name;
            }

           // if(strcmp(entry->d_name, strLastFileName.c_str()) < 0)
          //      strLastFileName = entry->d_name;

            if(strcmp(entry->d_name, strNewFilename.c_str()) > 0)
                strNewFilename = entry->d_name;

         //   nFileNum++;
        }
        closedir(dir);  
    } else {
        FILE* fp = fopen(".temp/logmanager.log", "a+");
        if (fp) {
            fprintf(fp, "log process open dir: %s\n", strerror(errno));
            fclose(fp);
        } else {
            perror("log process open dir:");
        }
    }

    dir = opendir(strLogPath.c_str());
    if (dir) {
        struct dirent *entry = NULL;
        while((entry = readdir(dir)) != NULL)
        {
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char* ptr = strrchr(entry->d_name, '.');
            if(!ptr || (ptr && strcmp(ptr, ".log") != 0))
                continue;        
            if(strncmp(entry->d_name, m_strDllName.c_str(), m_strDllName.size()) != 0)
                continue;

            if(nFileNum == 0)
            {
                strLastFileName = entry->d_name;
               // strNewFilename = entry->d_name;
            }

            if(strcmp(entry->d_name, strLastFileName.c_str()) < 0)
                strLastFileName = entry->d_name;

          //  if(strcmp(entry->d_name, strNewFilename.c_str()) > 0)
           //     strNewFilename = entry->d_name;

            nFileNum++;
        }
        closedir(dir);  
    } else {
        FILE* fp = fopen(".temp/logmanager.log", "a+");
        if (fp) {
            fprintf(fp, "log process open dir: %s\n", strerror(errno));
            fclose(fp);
        } else {
            perror("log process open dir:");
        }
    }
#endif

    if (nFileNum < m_nLogFileTotalNumber)
    {
        if (strNewFilename != _T(""))
        {
            CString strOpenFileName = strLogPath + strNewFilename;
            if(m_stdProcessLogFile.m_pStream)
            {
                m_stdProcessLogFile.Close();
            }
#ifdef __Win32__
            nRet = m_stdProcessLogFile.Open(strOpenFileName, CFile::modeRead | CFile::modeWrite | CFile::shareDenyNone);
#else
            nRet = m_stdProcessLogFile.Open(strOpenFileName.c_str(), "a+");
#endif
            if(nRet <= 0)
            {
                m_stdProcessLogFile.Close();
                return FALSE;
            }
            //            m_hLogFile = ::CreateFile(strOpenFileName,GENERIC_WRITE,FILE_SHARE_READ,
//                NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
//            if(m_hLogFile == INVALID_HANDLE_VALUE)
//            {
//                ::CloseHandle(m_hLogFile);
//                m_hLogFile = NULL;
//                return FALSE;
//            }
            DWORD dwFileSize = 0;
            //DWORD dwFileSizeHigh = 0;
//            if(m_hLogFile)
//                dwFileSize = ::GetFileSize(m_hLogFile,&dwFileSizeHigh);
            dwFileSize = m_stdProcessLogFile.GetLength();
//            if ((m_hLogFile != NULL && dwFileSize >= m_nMaxLogFileSize) || nRet< 1)
            if ((m_stdProcessLogFile.m_pStream != NULL && dwFileSize >= m_nMaxLogFileSize) || nRet< 1)
            {    
//                if (m_hLogFile)
//                {
//                    ::CloseHandle(m_hLogFile);
//                    m_hLogFile = NULL;
//                }
                if(m_stdProcessLogFile.m_pStream)
                {
                    m_stdProcessLogFile.Close();
                }

#ifdef __Win32__
                CTime   ct ; 
                CString strDateTime;
                ct = CTime::GetCurrentTime();
                strDateTime.Format(_T("%04d.%02d.%02d-%02d.%02d.%02d"),
                    ct.GetYear(),ct.GetMonth(),ct.GetDay(),
                    ct.GetHour(),ct.GetMinute(),ct.GetSecond());
                
                CString strCreateFileName =  strLogFile + strDateTime + m_strSubDllName + _T(".log");
                nRet = m_stdProcessLogFile.Open(strCreateFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
#else
                struct tm tm;   
                char strDateTime[1024];
                char strCreateFileName[1024];
                time_t now = time(NULL);   
                localtime_r(&now, &tm);
                sprintf(strDateTime, "%04d.%02d.%02d-%02d.%02d.%02d", 
                        1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

                sprintf(strCreateFileName, "%s_%s.log", strLogFile.c_str(), strDateTime);
                nRet = m_stdProcessLogFile.Open(strCreateFileName, "a+");
#endif
                if(nRet <= 0)
                {
                    m_stdProcessLogFile.Close();
                    return FALSE;
                }
//                m_hLogFile = ::CreateFile(strCreateFileName,GENERIC_WRITE,FILE_SHARE_READ,
//                    NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
//                if(m_hLogFile == INVALID_HANDLE_VALUE)
//                {
//                    ::CloseHandle(m_hLogFile);
//                    m_hLogFile = NULL;
//                    return FALSE;
//                }
            }
            else
            {
                //LONG lLog = 0;
                //::SetFilePointer(m_hLogFile,dwFileSize,&lLog,FILE_BEGIN);
            #ifdef __Win32__
                m_stdProcessLogFile.Seek(m_stdProcessLogFile.GetLength(), CFile::begin);
            #else
                m_stdProcessLogFile.Seek(m_stdProcessLogFile.GetLength(), SEEK_SET);
            #endif
            }    
        }
        else//first create file    
        {    
#ifdef __Win32__
            CTime   ct ; 
            CString strDateTime;
            ct = CTime::GetCurrentTime();
            strDateTime.Format(_T("%04d.%02d.%02d-%02d.%02d.%02d"),
                ct.GetYear(),ct.GetMonth(),ct.GetDay(),
                ct.GetHour(),ct.GetMinute(),ct.GetSecond());
            CString strCreateFileName = strLogFile + strDateTime + m_strSubDllName + _T(".log");
            nRet = m_stdProcessLogFile.Open(strCreateFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
#else
            struct tm tm;   
            char strDateTime[1024];
            char strCreateFileName[1024];


            time_t now = time(NULL);   
            localtime_r(&now, &tm);
            sprintf(strDateTime, "%04d.%02d.%02d-%02d.%02d.%02d", 
                    1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            sprintf(strCreateFileName, "%s_%s%s.log", strLogFile.c_str(), strDateTime, m_strSubDllName.c_str());

            nRet = m_stdProcessLogFile.Open(strCreateFileName, "a+");
#endif
            if(nRet <= 0)
            {
                m_stdProcessLogFile.Close();
                return nRet;
            }
//            m_hLogFile = ::CreateFile(strCreateFileName,GENERIC_WRITE,FILE_SHARE_READ,
//                NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
//            if(m_hLogFile == INVALID_HANDLE_VALUE)
//            {
//                ::CloseHandle(m_hLogFile);
//                m_hLogFile = NULL;
//                return FALSE;
//            }
        }
    }
    else
    {
        DWORD dwFileSize = 0;
        if (strNewFilename != _T(""))
        {
            //open the last file to check the file length;
            CString strOpenFileName = strLogPath + strNewFilename;
            if(m_stdProcessLogFile.m_pStream)
            {
                m_stdProcessLogFile.Close();
            }

#ifdef __Win32__
            nRet = m_stdProcessLogFile.Open(strOpenFileName, CFile::modeRead | CFile::modeWrite | CFile::shareDenyNone);
#else
            nRet = m_stdProcessLogFile.Open(strOpenFileName.c_str(), "a+");
#endif
            if(nRet <= 0)
            {
                m_stdProcessLogFile.Close();
                return nRet;
            }
            //        m_hLogFile = ::CreateFile(strOpenFileName,GENERIC_WRITE,FILE_SHARE_READ,
            //            NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
            //        if(m_hLogFile == INVALID_HANDLE_VALUE)
            //        {
            //            ::CloseHandle(m_hLogFile);
            //            m_hLogFile = NULL;
            //            return FALSE;
            //        }
           
            //DWORD dwFileSizeHigh = 0;
            //        if(m_hLogFile)
            if(m_stdProcessLogFile.m_pStream)
                dwFileSize = m_stdProcessLogFile.GetLength();
        }
        else
        {
            //not flund the tartget file ,there we need create a new file and delete the lastest one;
            dwFileSize = m_nMaxLogFileSize;
        }
        //dwFileSize = ::GetFileSize(m_hLogFile,&dwFileSizeHigh);
        if (dwFileSize >= m_nMaxLogFileSize)
        {
            if(m_stdProcessLogFile.m_pStream)
            {
                m_stdProcessLogFile.Close();
            }
//            if (m_hLogFile)
//            {
//                ::CloseHandle(m_hLogFile);
//                m_hLogFile = NULL;
//            }
            strLastFileName = strLogPath + strLastFileName;
#ifdef __Win32__
            ::DeleteFile(strLastFileName);
#else
            unlink(strLastFileName.c_str());
#endif

#ifdef __Win32__
            CTime   ct ; 
            CString strDateTime;
            ct = CTime::GetCurrentTime();
            strDateTime.Format(_T("%04d.%02d.%02d-%02d.%02d.%02d"),
                ct.GetYear(),ct.GetMonth(),ct.GetDay(),
                ct.GetHour(),ct.GetMinute(),ct.GetSecond());
            CString strCreateFileName = strLogFile + strDateTime + m_strSubDllName + _T(".log");
            
            nRet = m_stdProcessLogFile.Open(strCreateFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
#else
            struct tm tm;   
            char strDateTime[1024];
            char strCreateFileName[1024];


            time_t now = time(NULL);   
            localtime_r(&now, &tm);
            sprintf(strDateTime, "%04d.%02d.%02d-%02d.%02d.%02d", 
                    1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            sprintf(strCreateFileName, "%s_%s%s.log", strLogFile.c_str(), strDateTime, m_strSubDllName.c_str());

            nRet = m_stdProcessLogFile.Open(strCreateFileName, "a+");
#endif
//            m_hLogFile = ::CreateFile(strCreateFileName,GENERIC_WRITE,FILE_SHARE_READ,
//                NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
//            if(m_hLogFile == INVALID_HANDLE_VALUE)
//            {
//                ::CloseHandle(m_hLogFile);
//                m_hLogFile = NULL;
//                return FALSE;
//            }

            if(nRet <= 0)
            {
                m_stdProcessLogFile.Close();
                return FALSE;
            }
        }
#ifdef __Win32__
        m_stdProcessLogFile.Seek(m_stdProcessLogFile.GetLength(),CFile::begin);
#else
        m_stdProcessLogFile.Seek(m_stdProcessLogFile.GetLength(), SEEK_SET);
#endif
        //LONG lLog = 0;
        //::SetFilePointer(m_hLogFile,dwFileSize,&lLog,FILE_BEGIN);
    }
    return nRet;

}

int CLogProcess::WriteStatLog(char *pLog, int nLog)
{
    int nRet = 0;
    
#ifdef __Win32__
    try
#endif
    {
        nRet = FindStatisticsFile();
        if(nRet > 0)
        {
#ifdef __Win32__
        m_stdProcessLogFile.Write(pLog, nLog);
#else
        m_stdProcessLogFile.WriteString(pLog);
#endif
        }
    }
#ifdef __Win32__
    catch (CMemoryException* e)
    {
        nRet = -1;
        printf("=========LogMemoryException=========");
    }
    catch (CFileException* e)
    {
        nRet = -1;
        printf("=========LogFileException=========");
    }
#endif

    return nRet;
}

int CLogProcess::FindStatisticsFile()
{
    int nRet = 1;

#ifdef __Win32__
    SYSTEMTIME time;
    ::GetLocalTime(&time);
    if(m_stdProcessLogFile.m_pStream)
    {
        if(time.wYear == m_timeNowFileTime.wYear && time.wMonth == m_timeNowFileTime.wMonth && time.wDay == m_timeNowFileTime.wDay)
        {
            //the time is right and there is a opened file;
            return 1;
        }
        else
        {
            //the time is wrong , close opened file ,and find a right file or create a new one;
            m_stdProcessLogFile.Flush();
            m_stdProcessLogFile.Close();
        }
    }

    CString strLogTemp;
    strLogTemp.Format(_T("_%04d%02d%02d.log"),time.wYear,time.wMonth,time.wDay);
#else
    struct tm tm;
    time_t now = time(NULL);   
    localtime_r(&now, &tm);

    if(m_stdProcessLogFile.m_pStream)
    {
        if(tm.tm_year == m_timeNowFileTime.tm_year && tm.tm_mon == m_timeNowFileTime.tm_mon && 
            tm.tm_mday == m_timeNowFileTime.tm_mday)
        {
            //the time is right and there is a opened file;
            return 1;
        }
        else
        {
            //the time is wrong , close opened file ,and find a right file or create a new one;
            m_stdProcessLogFile.Flush();
            m_stdProcessLogFile.Close();
        }
    }
    
    char strLogTemp[1024];
    sprintf(strLogTemp, "_%04d%02d%02d.log", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday);
#endif

    CString strLogFile = m_strDllName + strLogTemp;

#ifdef __Win32__
    nRet = m_stdProcessLogFile.Open(strLogFile, CFile::modeRead | CFile::modeWrite | CFile::shareDenyNone);
#else
    nRet = m_stdProcessLogFile.Open(strLogFile.c_str(), "a+");
#endif
    if(nRet <= 0)
    {
        m_stdProcessLogFile.Close();
#ifdef __Win32__
        nRet = m_stdProcessLogFile.Open(strLogFile, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
#else
        nRet = m_stdProcessLogFile.Open(strLogFile.c_str(), "a+");
#endif
        if(nRet <= 0)
        {
            m_stdProcessLogFile.Close();
            return -1;
        }
    }
#ifdef __Win32__
    m_stdProcessLogFile.Seek(m_stdProcessLogFile.GetLength(),CFile::begin);
    m_timeNowFileTime = time;
#else
    m_stdProcessLogFile.Seek(m_stdProcessLogFile.GetLength(), SEEK_SET);
    m_timeNowFileTime = tm;
#endif

    return 1;
}
