////////////////////////////////////////////////////////
//                                                    //
//    Библиотека функций для построения алгоритмов    //
//    автоматического раскроя листового материала     //
//                      ©2002                         //
//     Разработчик: Полевов Александр Валерьевич      //
//             аспирантура УГТУ-УПИ                   //
//       Механико Машиностроительный Факультет        //
//                 Кафедра САПР                       //
//                                                    //
////////////////////////////////////////////////////////

#include "common.h"
#include "../define.h"
#include "nclDBS.h"
#include "DBS\\doc.h"

CnclDBS::CnclDBS():CnclCommonImport()
{
}

CnclDBS::~CnclDBS()
{

}

void CnclDBS::Read(const WCHAR *FileName)
{
    Clear();
    CDoc Doc;
    Doc.File.Read(FileName);
    Doc.ReadDetail(1,1);
    CListOfDetail *CurDetail=Doc.Detail.First;
    if(Doc.Detail.GetCountOfDetail(TRUE)==0)
    {
        //        Err.Add(L"LoadFromDBS(): ошибка чтения файла");
        return;
    }
    CTransform Transform;
    CnclPoint Begin;
    CnclPoint End;
    long ID=1;
    while(CurDetail)
    {
        if(!Add())
            break;
        Last->ID=ID;
        //memcpy(Last->Name,CurDetail->Detail->Detail_ID,8);
        CurDetail->Detail->GetID(Last->Name);
        Last->Name[8]=L'\0';
        /*
        WCHAR buf;
        for(int j=0;j<4;j++)
        {
            buf=Last->Name[j*2+1];
            Last->Name[j*2+1]=Last->Name[j*2];
            Last->Name[j*2]=buf;
        }
        */
        ::Trim(Last->Name);
        CListOfFigure *Figure=CurDetail->Detail->Figure.First;
        while(Figure)
        {
            CSegment *Segment=Figure->Figure->GetFirstSegment(Transform);
            CnclSpline Spline;
            while(Segment)
            {
                Begin.Set(Segment->x_begin,Segment->y_begin);
                End.Set(Segment->x_end,Segment->y_end);
                Spline.Add(Begin,End,Segment->z,FALSE);
                Segment=Figure->Figure->GetNextSegment(Transform);
            }
            Last->Add(&Spline);
            Figure=Figure->Next;
        }
        CurDetail=CurDetail->Next;
        ID++;
    }
}

void CnclDBS::Save(CnclNest *Nest,WCHAR *FileName)
{
    CDoc Doc;
    CnclNest::SnclDetail *Current;
    CnclPoint newEnd;
    CnclPoint newBegin;
    double z;
    long count=0;
    Current=Nest->First;
    BOOL ChangeDirection=FALSE;
    Doc.Detail.Clear();
    {
        if(Nest->Sheet.Count()>0)
        {
            CDetail *Detail=new CDetail;
            Doc.Detail.AddDetail(Detail);

            //size_t size;
            //wcstombs_s(&size,buf_c,sizeof(buf_c),str,sizeof(buf_c));
            WideCharToMultiByte(1251,0,Nest->Sheet.Name,-1,(char*)Detail->Detail_ID,sizeof(Detail->Detail_ID),0,0);

            for(int j=lstrlen(Nest->Sheet.Name);j<8;j++)
                Detail->Detail_ID[j]=' ';
            //		memcpy(Detail->Detail_ID,Nest->Sheet.Name,8);
            char buf;
            for(int j=0;j<4;j++)
            {
                buf=Detail->Detail_ID[j*2+1];
                Detail->Detail_ID[j*2+1]=Detail->Detail_ID[j*2];
                Detail->Detail_ID[j*2]=buf;
                Detail->Detail_ID[j*2+1]=Detail->Detail_ID[j*2+1]<' '?' ':Detail->Detail_ID[j*2+1];
                Detail->Detail_ID[j*2]=Detail->Detail_ID[j*2]<' '?' ':Detail->Detail_ID[j*2];
            }
            CnclDetail::SnclSpline *CurSpline=Nest->Sheet.First;
            while(CurSpline)
            {
                ChangeDirection=CurSpline->Area()>0;
                ChangeDirection=ChangeDirection ^ (CurSpline==Nest->Sheet.First);

                CnclSpline::SnclSegment *CurSegment=CurSpline->First;
                if(ChangeDirection)
                    CurSegment=CurSpline->Last;
                CFigure *Figure=new CFigure;
                Figure->Spline=new CSpline;
                while(CurSegment)
                {
                    if(ChangeDirection)
                    {
                        newBegin=CurSegment->GetNewCoord(CurSegment->End,FALSE,NULL);
                        z=-CurSegment->GetNewZ();
                        Figure->Spline->AddPoint((float)newBegin.x,(float)newBegin.y,(float)z);
                        if(CurSegment==CurSpline->First)
                        {
                            newEnd=CurSegment->GetNewCoord(CurSegment->Begin,FALSE,NULL);
                            Figure->Spline->AddPoint((float)newEnd.x,(float)newEnd.y,(float)z);
                        }
                        CurSegment=CurSegment->Prev;
                    }
                    else
                    {
                        newBegin=CurSegment->GetNewCoord(CurSegment->Begin,FALSE,NULL);
                        z=CurSegment->GetNewZ();
                        Figure->Spline->AddPoint((float)newBegin.x,(float)newBegin.y,(float)z);
                        if(CurSegment==CurSpline->Last)
                        {
                            newEnd=CurSegment->GetNewCoord(CurSegment->End,FALSE,NULL);
                            Figure->Spline->AddPoint((float)newEnd.x,(float)newEnd.y,(float)z);
                        }
                        CurSegment=CurSegment->Next;
                    }
                }
                CurSpline=CurSpline->Next;
                Detail->Figure.AddFigure(Figure);
            }
        }
    }
    while(Current)
    {
        CDetail *Detail=new CDetail;
        Doc.Detail.AddDetail(Detail);
        memset(Detail->Detail_ID,' ',8);
        WideCharToMultiByte(1251,0,Current->Name,-1,(char*)Detail->Detail_ID,sizeof(Detail->Detail_ID),0,0);
        for(int j=lstrlen(Nest->Sheet.Name);j<8;j++)
            Detail->Detail_ID[j]=' ';
        //		memcpy(Detail->Detail_ID,Current->Name,8);
        unsigned char buf;
        for(int j=0;j<4;j++)
        {
            buf=Detail->Detail_ID[j*2+1];
            Detail->Detail_ID[j*2+1]=Detail->Detail_ID[j*2];
            Detail->Detail_ID[j*2]=buf;
            Detail->Detail_ID[j*2+1]=Detail->Detail_ID[j*2+1]<' '?' ':Detail->Detail_ID[j*2+1];
            Detail->Detail_ID[j*2]=Detail->Detail_ID[j*2]<' '?' ':Detail->Detail_ID[j*2];
        }

        CnclDetail::SnclSpline *CurSpline=Current->First;
        while(CurSpline)
        {
            ChangeDirection=CurSpline->Area()>0;
            ChangeDirection=ChangeDirection ^ (CurSpline==Current->First);

            CnclSpline::SnclSegment *CurSegment=ChangeDirection?CurSpline->Last:CurSpline->First;
            CFigure *Figure=new CFigure;
            Figure->Spline=new CSpline;
            while(CurSegment)
            {
                if(ChangeDirection)
                {
                    newBegin=CurSegment->GetNewCoord(CurSegment->End,FALSE,NULL);
                    z=-CurSegment->GetNewZ();
                    Figure->Spline->AddPoint((float)newBegin.x,(float)newBegin.y,(float)z);
                    if(CurSegment==CurSpline->First)
                    {
                        newEnd=CurSegment->GetNewCoord(CurSegment->Begin,FALSE,NULL);
                        Figure->Spline->AddPoint((float)newEnd.x,(float)newEnd.y,(float)z);
                    }
                    CurSegment=CurSegment->Prev;
                }
                else
                {
                    newBegin=CurSegment->GetNewCoord(CurSegment->Begin,FALSE,NULL);
                    z=CurSegment->GetNewZ();
                    Figure->Spline->AddPoint((float)newBegin.x,(float)newBegin.y,(float)z);
                    if(CurSegment==CurSpline->Last)
                    {
                        newEnd=CurSegment->GetNewCoord(CurSegment->End,FALSE,NULL);
                        Figure->Spline->AddPoint((float)newEnd.x,(float)newEnd.y,(float)z);
                    }
                    CurSegment=CurSegment->Next;
                }
            }
            CurSpline=CurSpline->Next;
            Detail->Figure.AddFigure(Figure);
        }
        count++;
        Current=Current->Next;
    }
    CnclFile OutFileDBS(FileName,GENERIC_WRITE,CREATE_ALWAYS);
    Doc.Detail.Rename(1);
    Doc.Detail.Write(&OutFileDBS);
    unsigned short End=-1;
    OutFileDBS.write((const char*)&End,2);
    OutFileDBS.write((const char*)&End,2);
    OutFileDBS.close();
}
