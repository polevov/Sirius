// nclTaskFile.cpp: implementation of the CnclTaskFile class.
//
//////////////////////////////////////////////////////////////////////

#include "define.h"
#include "nclTaskFile.h"
#include "nclFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CnclTaskFile::CnclTaskFile()
{
    First=NULL;
    Last=NULL;
    FileName[0]='\0';
}

CnclTaskFile::~CnclTaskFile()
{
    Clear();
}

void CnclTaskFile::operator = (WCHAR *File)
{
    lstrcpy(FileName,File);
}

void CnclTaskFile::Clear()
{
    SnclGeomFile *Current=First;
    while(Current)
    {
        First=First->Next;
        delete Current;
        Current=First;
    }
    First=NULL;
    Last=NULL;
    FileName[0]='\0';
}

BOOL CnclTaskFile::LoadKOL(const WCHAR *File)
{
    //	Err<<L"Загрузка файла задания: "<<FileName<<ENDL;
    lstrcpy(FileName,File);
    CnclFile InFile(FileName,GENERIC_READ,OPEN_EXISTING);
    WCHAR nclFile[MAX_PATH];
    lstrcpy(nclFile,FileName);
    lstrcat(nclFile,L".ncl");
    CnclFile InFileNCL(nclFile,GENERIC_READ,OPEN_EXISTING);
    WCHAR FileGeom[1024];
    //	WCHAR FileDir[1024];
    int Flag;	//идентификатор 0-лист, 1-деталь
    int Count; //количество деталей или листов
    if(!InFile)
    {
        //		Err.MsgError();
        return FALSE;
    }
    while(InFile.good())
    {
        WCHAR buf[1024];
        {
            InFile.getline(buf,sizeof(buf));
            Trim(buf);
            if(lstrlen(buf)==0) continue;
            long len=lstrlen(buf);
            long i;
            for(i=len;i>0;i--)
                if(buf[i-1]<L'0')
                    break;
            if(i==0)
                return FALSE;
            buf[i-1]=L'\0';
            if(i<len)
                Flag=_wtol(&buf[i]);
            else
                return FALSE;
            Trim(buf);
            len=lstrlen(buf);
            for(i=len;i>0;i--)
                if(buf[i-1]<L'0')
                    break;
            if(i==0)
                return FALSE;
            buf[i-1]=L'\0';
            if(i<len)
                Count=_wtol(&buf[i]);
            else
                return FALSE;
            Trim(buf);
            lstrcpy(FileGeom,buf);
        }
        if(Count<=0 || Flag>1 || Flag<0)
        {
            //			Err<<"Ошибка в файле задания "<<FileName<<'\r\n';
            return FALSE;
        }
        if(!IsFile(buf))
        {
            lstrcat(FileGeom,L".dbs");
            if(!IsFile(FileGeom))
            {
                lstrcpy(&FileGeom[lstrlen(FileGeom)-3],L"dxf");
                if(!IsFile(FileGeom))
                {
                    WCHAR newFile[1024];
                    BuildFileName(newFile,buf);
                    if(!IsFile(buf))
                    {
                        lstrcat(FileGeom,L".dbs");
                        if(!IsFile(buf))
                        {
                            lstrcpy(&FileGeom[lstrlen(FileGeom)-3],L"dxf");
                            if(!IsFile(buf))
                            {
                                lstrcpy(FileGeom,buf);
                            }
                        }
                    }
                }
            }
        }
        /*
                    lstrcpy(FileGeom,FileDir);
                    lstrcat(FileGeom,buf);
                    lstrcat(FileGeom,".dbs");
                    if(_access(FileGeom,0)==-1)
                    {
                        lstrcpy(&FileGeom[lstrlen(FileGeom)-3],"dxf");
                        if(_access(FileGeom,0)==-1)
                        {
                            FileGeom[lstrlen(FileGeom)-4]='\0';
                        }
                    }
        */
        long angle_step=0;

        if(! !InFileNCL)
        {
            InFileNCL.seekg(0);
            while(InFileNCL.good())
            {
                InFileNCL.getline(buf,sizeof(buf));
                Trim(buf);
                if(lstrlen(buf)==0) continue;
                long len=lstrlen(buf);
                long i;
                for(i=len;i>0;i--)
                    if(buf[i-1]<L'0')
                        break;
                if(i==0)
                    return FALSE;
                buf[i-1]=L'\0';
                long current_angle_step=0;
                if(i<len)
                    current_angle_step=_wtol(&buf[i]);
                else
                    return FALSE;
                Trim(buf);
                if(!IsFile(buf))
                {
                    lstrcat(buf,L".dbs");
                    if(!IsFile(buf))
                    {
                        lstrcpy(&buf[lstrlen(buf)-3],L"dxf");
                        if(!IsFile(buf))
                        {
                            WCHAR newFile[1024];
                            BuildFileName(newFile,buf);
                            if(!IsFile(buf))
                            {
                                lstrcat(buf,L".dbs");
                                if(!IsFile(buf))
                                {
                                    lstrcpy(&FileGeom[lstrlen(buf)-3],L"dxf");
                                }
                            }
                        }
                    }
                }
                if(lstrcmp(buf,FileGeom)==0)
                    angle_step=current_angle_step;
            }
        }

        Add(FileGeom,Count,Flag,angle_step/*,_access(FileGeom,0)!=-1*/);
        if(!IsFile(FileGeom))
        {
            //Err<<L"Файл не найден! "<<FileGeom<<ENDL;
        }
        FileGeom[0]='\0';
    }
    InFile.close();
    if(!!InFileNCL)
        InFileNCL.close();
    return TRUE;
}

BOOL CnclTaskFile::Add(WCHAR *FileGeom, long Count, long Flag,long angle_step/*,long Find*/)
{
    SnclGeomFile *GeomFile=new SnclGeomFile(FileGeom,Count,Flag,angle_step/*,Find*/);
    if(!GeomFile)
        return FALSE;
    GeomFile->Prev=Last;
    if(!First)
        First=GeomFile;
    if(Last)
        Last->Next=GeomFile;
    Last=GeomFile;
    return TRUE;
}

CnclTaskFile::SnclGeomFile::SnclGeomFile(WCHAR* _FileName,long _Count, long _Flag,long _angle_step)
{
    ZeroMemory(FileName,sizeof(FileName));
    lstrcpy(FileName,_FileName);
    Count=_Count;
    Flag=_Flag;
    //	Find=_Find;
    Next=NULL;
    Prev=NULL;
    angle_step=_angle_step;
}

void CnclTaskFile::SaveKOL()
{
    if(lstrlen(FileName)==0)
        return;
    CnclFile Out(FileName,GENERIC_WRITE,CREATE_ALWAYS);
    if(!Out)
        return;
    WCHAR nclFile[MAX_PATH];
    lstrcpy(nclFile,FileName);
    lstrcat(nclFile,L".ncl");
    CnclFile OutNCL(nclFile,GENERIC_WRITE,CREATE_ALWAYS);
    if(!OutNCL)
        return;
    SnclGeomFile* Current=First;
    while(Current)
    {
        long len=lstrlen(Current->FileName);
        if(len>4)
        {
            if(Current->FileName[len-4]==L'.')
                len-=4;
        }
        //	Out.write(Current->FileName,len);
        WCHAR buf[MAX_PATH];
        lstrcpyn(buf,Current->FileName,len+1);
        Out<<buf<<L" "<<Current->Count<<L" "<<Current->Flag<<ENDL;
        //OutNCL.write(Current->FileName,len);
        if(Current->angle_step>0)
            OutNCL<<buf<<L" "<<Current->angle_step<<ENDL;
        Current=Current->Next;
    }
    Out.close();
    OutNCL.close();
}

void CnclTaskFile::BuildFileName(WCHAR *FileGeom,SnclGeomFile *File)
{
    WCHAR smFile[1024];
    lstrcpy(smFile,File->FileName);
    BuildFileName(FileName,smFile);
}

void CnclTaskFile::BuildFileName(WCHAR *FileGeom,WCHAR *File)
{
    WCHAR FileDir[1024];
    WCHAR newFile[1024];
    lstrcpy(FileDir,FileName);
    WCHAR *End=wcschr(FileDir,L'\\');
    if(End)
        End[1]=0;
    else
        FileDir[0]=0;

    lstrcpy(newFile,FileDir);
    lstrcat(newFile,File);
    lstrcpy(FileGeom,newFile);
}

