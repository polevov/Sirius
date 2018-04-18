#ifndef _NCLFILE_
#define _NCLFILE_

#include <windows.h>
#define ENDL L"\r\n"

class CnclFile
{
public:
    CnclFile(void);
    CnclFile(const WCHAR *LogName,DWORD Access, DWORD Creation);
    ~CnclFile(void);
    CnclFile operator << (const WCHAR*);
    CnclFile operator << (double);
    BOOL operator!();

    void open(const WCHAR *LogName,DWORD Access, DWORD Creation, DWORD ShareMode=0);
    void close();
    HANDLE hFile;
    BOOL good();
    void getline(WCHAR* buf,DWORD size);
    void write(const char* buf,DWORD len);
    void read(char *buf,DWORD len);
    void seekg(DWORD Pos);
    DWORD tellg();
private:
    BOOL eof;
};
#endif
