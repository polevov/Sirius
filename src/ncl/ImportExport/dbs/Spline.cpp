////////////////////////////////////////////////////////
//                                                    //
//           Библиотека классов для работы            //
//              с файлами в формате DBS               //
//                      ©2002                         //
//     Разработчик: Полевов Александр Валерьевич      //
//             аспирантура УГТУ-УПИ                   //
//       Механико Машиностроительный Факультет        //
//                 Кафедра САПР                       //
//                                                    //
////////////////////////////////////////////////////////

#include "define.h"
#include "Spline.h"

CSpline::CSpline()
{
    First=NULL;
    Last=NULL;
    Offset=NULL;
}

CSpline::~CSpline()
{
    Clear();
}

void CSpline::Clear()
{
    CListOfPoint *Current=First;
    while (Current)
    {
        Current=Current->Next;
        delete First;
        First=Current;
    }
    delete Offset;
    Offset=NULL;
    First=NULL;
    Last=NULL;
}

BOOL CSpline::AddPoint(float x_coord,float y_coord,float z_coord)
{
    CListOfPoint *NewPoint;
    NewPoint=new CListOfPoint;
    if (!NewPoint) return FALSE;
    NewPoint->Point.x=x_coord;
    NewPoint->Point.y=y_coord;
    NewPoint->Point.z=z_coord;
    NewPoint->Next=NULL;
    if (Last) Last->Next=NewPoint;
    NewPoint->Prev=Last;
    Last=NewPoint;
    if (!First)	First=NewPoint;
    return TRUE;
}

BOOL CSpline::AddPoint(CListOfPoint &Point)
{
    CListOfPoint *NewPoint;
    NewPoint=new CListOfPoint;
    if (!NewPoint) return FALSE;
    NewPoint->Point.x=Point.Point.x;
    NewPoint->Point.y=Point.Point.y;
    NewPoint->Point.z=Point.Point.z;
    NewPoint->Next=NULL;
    if (Last) Last->Next=NewPoint;
    NewPoint->Prev=Last;
    Last=NewPoint;
    if (!First)	First=NewPoint;
    return TRUE;
}

BOOL CSpline::Read(char *Data)
{
    unsigned short CountOfCoord=(unsigned short)
            ((*(unsigned short*)&Data[0]-15)/3);
    ID=*(unsigned short*)&Data[12];
    for(unsigned short i=0;i<CountOfCoord;i++)
        if(!AddPoint(*(float*)&Data[64+i*12],*(float*)&Data[68+i*12],*(float*)&Data[72+i*12]))
            return FALSE;;
    return TRUE;
}

void CSpline::Write(CnclFile *OutFile)
{
    CListOfPoint *Current=First;
    while(Current)
    {
        OutFile->write((const char*)&Current->Point.x,4);
        OutFile->write((const char*)&Current->Point.y,4);
        OutFile->write((const char*)&Current->Point.z,4);
        Current=Current->Next;
    }
}

unsigned short CSpline::GetCountOfPoint()
{
    unsigned short Count=0;
    CListOfPoint *Current=First;
    while(Current)
    {
        Count++;
        Current=Current->Next;
    }
    return Count;
}


