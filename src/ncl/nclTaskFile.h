// nclTaskFile.h: interface for the CnclTaskFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NCLTASKFILE_H_
#define _NCLTASKFILE_H_
#include <windows.h>

#define LOAD_DETAIL		1	//загрузка детали
#define LOAD_SHEET		0	//загрузка листа

class CnclTaskFile  
{
public:
    void SaveKOL();
    struct SnclGeomFile
    {
        SnclGeomFile(WCHAR* _FileName,long _Count, long _Flag,long angle_step=0/*,long _Find*/);
        WCHAR FileName[MAX_PATH];
        long Count;	//количество деталей
        long Flag;	//0-Лист 1-Деталь
        long angle_step; //количество вращений 0-количество берется из настроек
        //		BOOL Find;
        SnclGeomFile *Next;
        SnclGeomFile *Prev;
    };
    void BuildFileName(WCHAR *FileGeom,SnclGeomFile *File);
    void BuildFileName(WCHAR *FileGeom,WCHAR *File);
    SnclGeomFile *First;
    SnclGeomFile *Last;
    BOOL LoadKOL(const WCHAR *File);
    CnclTaskFile();
    ~CnclTaskFile();
    WCHAR FileName[MAX_PATH];
    void operator = (WCHAR *File);
    void Clear();
    BOOL Add(WCHAR *FileGeom,long Count,long Flag,long angle_step=0/*,long Find*/);
};

#endif
