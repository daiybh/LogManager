#ifndef STDIO_FILE_H
#define STDIO_FILE_H

#include <stdio.h>

class CStdioFile
{
public:
	CStdioFile();
	~CStdioFile();

	int Open(const char *name, char *mode);
	int Close();
	long GetLength();
	size_t WriteString(const char *msg);
	int Seek(long offset, int whence);
	int Flush();

	FILE *m_pStream;
};

#endif
