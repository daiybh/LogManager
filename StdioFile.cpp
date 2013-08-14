#include "StdioFile.h"
#include <string.h>

CStdioFile::CStdioFile()
	: m_pStream(NULL)
{

}

CStdioFile::~CStdioFile()
{
	if(m_pStream)
		fclose(m_pStream);
	
	m_pStream = NULL;
}

int CStdioFile::Open(const char *filename, char *mode)
{
	m_pStream = fopen(filename, mode);

	if(!m_pStream)
		return false;

	return true;
}

int CStdioFile::Close()
{
	if(m_pStream)
		fclose(m_pStream);

	m_pStream = NULL;

	return 0;
}

long CStdioFile::GetLength()
{
	long curpos, length;

	if(!m_pStream)
		return -1;

	curpos = ftell(m_pStream);
	fseek(m_pStream, 0, SEEK_END);

	length = ftell(m_pStream);
	fseek(m_pStream, curpos, SEEK_SET);
	
	return length;
}

size_t CStdioFile::WriteString(const char *msg)
{
	size_t nbytes;

	nbytes = fwrite(msg, strlen(msg), 1, m_pStream);
	
	return nbytes;
}

int CStdioFile::Seek(long offset, int whence)
{
	int ret;
	
	ret = fseek(m_pStream, offset, whence);

	return ret;
}

int CStdioFile::Flush()
{
	if(fflush(m_pStream) == 0)
		return 0;
	else
		return -1;
}
