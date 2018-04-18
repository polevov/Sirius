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
#include "DetailList.h"

CDetailList::CDetailList()
{
    First=NULL;
    Last=NULL;
}

CDetailList::~CDetailList()
{
    Clear();
}

void CDetailList::Clear()
{
    CListOfDetail *Current;
    Current=First;
    while(Current)
    {
        Current=Current->Next;
        delete First->Detail;
        delete First;
        First=Current;
    }
    First=NULL;
    Last=NULL;
}

BOOL CDetailList::AddDetail(CDetail *DetailAdd)
{
    CListOfDetail *NewDetail;
    NewDetail=new CListOfDetail;
    if(!NewDetail) return FALSE;
    NewDetail->Detail=DetailAdd;
    if(Last)Last->Next=NewDetail;
    Last=NewDetail;
    Last->Next=NULL;
    if(!First)First=NewDetail;
    return TRUE;
}

BOOL CDetailList::Read(CListInFile *Current,CInFile *File,long Count)
{
    //	MessageBox(NULL,"","",0);
    CListInFile *CurrentDet=File->First;
    CListInFile *CurrentLongDet=File->First;
    BOOL Find=FALSE;
    BOOL FindLongName=FALSE;
    while(CurrentDet)
    {
        if(*(unsigned short*)&CurrentDet->Data[8]==TYPE_DETAIL_NAME &&
                *(unsigned short*)&Current->Data[12]==*(unsigned short*)&CurrentDet->Data[12])
        {
            Find=TRUE;
            break;
        }
        CurrentDet=CurrentDet->Next;
    }
    while(CurrentLongDet)
    {
        if(*(unsigned short*)&CurrentLongDet->Data[8]==TYPE_DETAIL_LONG_NAME &&
                *(unsigned short*)&Current->Data[12]==*(unsigned short*)&CurrentLongDet->Data[12])
        {
            FindLongName=TRUE;
            break;
        }
        CurrentLongDet=CurrentLongDet->Next;
    }
    if(!Find) return FALSE;
    CDetail *DetailAdd;
    DetailAdd=new CDetail;
    if(!DetailAdd) return FALSE;
    DetailAdd->Count=Count;
    memcpy(DetailAdd->Detail_ID,&CurrentDet->Data[16],8);
    //	 if(FindLongName)
    //		memcpy(DetailAdd->LongDetail_ID,&CurrentLongDet->Data[16],30);
    //	 else
    //		memcpy(DetailAdd->LongDetail_ID,&CurrentDet->Data[16],8);
    DetailAdd->ID=*(unsigned short*)&Current->Data[12];
    //	MessageBox(NULL,"1","",MB_ICONWARNING);
    return AddDetail(DetailAdd);
}

void CDetailList::Write(CnclFile *OutFile)
{
    CListOfDetail *Current=First;
    while(Current)
    {
        Current->Detail->Write(OutFile);
        Current=Current->Next;
    }
}

long CDetailList::Rename(long Number)
{
    CListOfDetail *CurrentDetail=First;
    CListOfFigure *CurrentFigure;
    while(CurrentDetail)
    {
        CurrentFigure=CurrentDetail->Detail->Figure.First;
        while(CurrentFigure)
        {
            CurrentFigure->Figure->Spline->ID=0;
            CurrentFigure=CurrentFigure->Next;
        }
        CurrentDetail=CurrentDetail->Next;
    }
    CurrentDetail=First;
    while(CurrentDetail)
    {
        CurrentFigure=CurrentDetail->Detail->Figure.First;
        while(CurrentFigure)
        {
            CurrentFigure->Figure->ID=Number;
            if(CurrentFigure->Figure->Spline->ID==0)
                CurrentFigure->Figure->Spline->ID=Number;
            CurrentFigure=CurrentFigure->Next;
            Number++;
        }
        CurrentDetail->Detail->ID=Number;
        Number++;
        CurrentDetail=CurrentDetail->Next;
    }
    Number--;
    return Number;
}

//если  WithCount - подстчет кол-ва детелей
//если !WithCount - подсчет кол-ва типов деталей
long CDetailList::GetCountOfDetail(BOOL WithCount)
{
    long Count=0;
    CListOfDetail *Current=First;
    while(Current)
    {
        if(WithCount)
            Count+=Current->Detail->Count;
        else
            Count++;
        Current=Current->Next;
    }
    return Count;
}
