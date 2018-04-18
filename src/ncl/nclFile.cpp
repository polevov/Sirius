#include <common.h>
#include "ncl/define.h"
#include ".\nclfile.h"

CnclFile::CnclFile(const WCHAR *FileName,DWORD Access, DWORD Creation)
{
    open(FileName,Access,Creation);
}

CnclFile::CnclFile(void)
{
    hFile=INVALID_HANDLE_VALUE;
    eof=TRUE;
}

CnclFile::~CnclFile(void)
{
}

void CnclFile::open(const WCHAR *LogName,DWORD Access, DWORD Creation, DWORD ShareMode)
{
    hFile=CreateFile(LogName,Access,ShareMode,NULL,Creation,FILE_ATTRIBUTE_NORMAL,NULL);
    eof=(hFile==INVALID_HANDLE_VALUE);
    if(hFile==INVALID_HANDLE_VALUE)
    {
        //		Err.MsgError();
        hFile=NULL;
    }
}

void CnclFile::close()
{
    if(hFile)
        CloseHandle(hFile);
    hFile=NULL;
}

CnclFile CnclFile::operator << (const WCHAR* str)
{
    DWORD tmp;
    //size_t size;
    char buf_c[1024];
    //wcstombs_s(&size,buf_c,sizeof(buf_c),str,sizeof(buf_c));
    buf_c[WideCharToMultiByte(1251,0,str,-1,buf_c,sizeof(buf_c),0,0)]='\0';
    WriteFile(hFile,buf_c,strlen(buf_c),&tmp,NULL);
    //	WriteFile(hFile,str,lstrlen(str)*sizeof(WCHAR),&tmp,NULL);
    return *this;
}

CnclFile CnclFile::operator << (double a)
{
    DWORD tmp;
    WCHAR buf[32];
    DoubleToStr(a,buf);
    char buf_c[32];
    size_t size;
    wcstombs(buf_c,buf,sizeof(buf_c));
    WriteFile(hFile,buf_c,strlen(buf_c),&tmp,NULL);
    return *this;
}

BOOL CnclFile::operator!()
{
    return (hFile==INVALID_HANDLE_VALUE);
}

BOOL CnclFile::good()
{
    return !eof;
}

void CnclFile::getline(WCHAR* buf,DWORD size)
{
    DWORD tmp;
    WCHAR *caret=wcschr(buf,L'\n');
    char cr;
    long read=size;
    while(read)
    {
        eof=!ReadFile(hFile,&cr,1,&tmp,NULL);
        if(eof)
            break;
        eof=(tmp==0);
        if(eof)
            break;
        MultiByteToWideChar(1251,MB_PRECOMPOSED,&cr,1,&buf[size-read],1);
        //buf[size-read]=cr;
        if(cr==L'\n')
            break;
        read--;
    }
    buf[size-read]=L'\0';
    if(size-read>0)
    {
        if(buf[size-read-1]==L'\r')
            buf[size-read-1]=L'\0';
    }
}

void CnclFile::write(const char *buf,DWORD len)
{
    DWORD tmp;
    WriteFile(hFile,buf,len,&tmp,NULL);
}

void CnclFile::read(char *buf,DWORD len)
{
    DWORD tmp;
    ReadFile(hFile,buf,len,&tmp,NULL);
}

void CnclFile::seekg(DWORD Pos)
{
    SetFilePointer(hFile,Pos,NULL,FILE_BEGIN);
    eof=FALSE;
}

DWORD CnclFile::tellg()
{
    return SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
}
