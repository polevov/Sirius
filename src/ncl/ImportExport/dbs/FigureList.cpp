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
#include "FigureList.h"

CFigureList::CFigureList()
{
    First=NULL;
    Last=NULL;

}

CFigureList::~CFigureList()
{
    Clear();
}

void CFigureList::Clear()
{
    CListOfFigure *Current;
    Current=First;
    while(Current)
    {
        Current=Current->Next;
        delete First->Figure;
        delete First;
        First=Current;
    }
    First=NULL;
    Last=NULL;
}

BOOL CFigureList::AddFigure(CFigure *FigureAdd)
{
    CListOfFigure *NewFigure;
    NewFigure=new CListOfFigure;
    if (!NewFigure) return FALSE;
    NewFigure->Figure=FigureAdd;
    if (Last) Last->Next=NewFigure;
    Last=NewFigure;
    Last->Next=NULL;
    if (!First)	First=NewFigure;
    return TRUE;
}

BOOL CFigureList::Read(char *Data,CSpline *Spline)
{
    CFigure *NewFigure;
    NewFigure=new CFigure;
    if(!NewFigure) return FALSE;
    if(!NewFigure->Read(Data,Spline))
        return FALSE;
    return AddFigure(NewFigure);
}


void CFigureList::Write(CnclFile *OutFile,long Group,CTransform &Transform)
{
    CListOfFigure *Current=First;
    while(Current)
    {
        Current->Figure->Write(OutFile,Group,Transform);
        Current=Current->Next;
    }
}

unsigned short CFigureList::GetCountOfFigure()
{
    unsigned short Count=0;
    CListOfFigure *Current=First;
    while(Current)
    {
        Count++;
        Current=Current->Next;
    }
    return Count;
}

void CFigureList::WriteNumber(CnclFile *OutFile)
{
    CListOfFigure *Current=First;
    unsigned short Zerro=0;
    while(Current)
    {
        OutFile->write((const char*)&Current->Figure->ID,2);
        OutFile->write((const char*)&Zerro,2);
        Current=Current->Next;
    }
}

