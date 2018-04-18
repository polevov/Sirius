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
#include "Figure.h"
#include "point.h"

CFigure::CFigure()
{
    Spline=NULL;
    FirstSegmentPoint=NULL;
    CosXX=1;
    CosYX=0;
    SinXX=0;
    SinYX=1;
    ID=0;
    Revers=REV_NORMAL;
    x=0;
    y=0;
}

CFigure::~CFigure()
{
}

BOOL CFigure::Read(char *Data,CSpline *SplineAdd)
{
    Spline=SplineAdd;
    ID=*(unsigned short int*)&Data[12];
    Revers=*(unsigned short*)&Data[36];
    CosXX=*(float*)&Data[40];
    SinXX=*(float*)&Data[44];
    CosYX=*(float*)&Data[48];
    SinYX=*(float*)&Data[52];
    x=*(float*)&Data[56];
    y=*(float*)&Data[60];
    return TRUE;
}

void CFigure::Write(CnclFile *OutFile,long Group,CTransform &Transform)
{
    unsigned short Type=TYPE_GEOM_FIGURE;
    unsigned short Zerro=0;
    unsigned short Len=15;
    unsigned short One=1;
    unsigned short IDWrite=(unsigned short)ID;
    unsigned short GroupWrite=(unsigned short)Group;
    if(ID==Spline->ID)
    {
        Type=TYPE_GEOM_SPLINE;
        Len=(unsigned short)(Spline->GetCountOfPoint()*3+15);
    }
    OutFile->write((const char*)&Len,2);			 //1
    OutFile->write((const char*)&IDWrite,2);		//2
    OutFile->write((const char*)&Len,2);			 //3
    OutFile->write((const char*)&Zerro,2);			//4
    OutFile->write((const char*)&Type,2);			//5
    OutFile->write((const char*)&Zerro,2);			//6
    OutFile->write((const char*)&IDWrite,2);		//7
    OutFile->write((const char*)&Zerro,2);			//8
    OutFile->write((const char*)&One,2);			//9
    OutFile->write((const char*)&Zerro,2);			//10
    OutFile->write((const char*)&Zerro,2);			//11
    OutFile->write((const char*)&Zerro,2);			//12
    OutFile->write((const char*)&Zerro,2);			//13
    OutFile->write((const char*)&Zerro,2);			//14
    OutFile->write((const char*)&GroupWrite,2);	//15
    OutFile->write((const char*)&Zerro,2);			//16
    if(Type==TYPE_GEOM_SPLINE)
        OutFile->write((const char*)&IDWrite,2);	//17
    else
    {
        IDWrite=(unsigned short)Spline->ID;
        OutFile->write((const char*)&IDWrite,2);	//17
    }
    OutFile->write((const char*)&Zerro,2);			//18
    OutFile->write((const char*)&Revers,2);		  //19
    OutFile->write((const char*)&Zerro,2);			//20
    CTransform WriteTransform;
    CTransform CurrentTransform;
    CurrentTransform.Matrix[0][0]=CosXX;
    CurrentTransform.Matrix[1][0]=CosYX;
    CurrentTransform.Matrix[2][0]=x;
    CurrentTransform.Matrix[0][1]=SinXX;
    CurrentTransform.Matrix[1][1]=SinYX;
    CurrentTransform.Matrix[2][1]=y;
    WriteTransform=CurrentTransform*Transform;
    OutFile->write((const char*)&WriteTransform.Matrix[0][0],4);		  //21,22
    OutFile->write((const char*)&WriteTransform.Matrix[0][1],4);		   //23,24
    OutFile->write((const char*)&WriteTransform.Matrix[1][0],4);		  //25,26
    OutFile->write((const char*)&WriteTransform.Matrix[1][1],4);		   //27,28
    OutFile->write((const char*)&WriteTransform.Matrix[2][0],4);			       //29,30
    OutFile->write((const char*)&WriteTransform.Matrix[2][1],4);			       //31,32
    if(Type==TYPE_GEOM_SPLINE)
        Spline->Write(OutFile);
}

CSegment* CFigure::GetFirstSegment(CTransform &Transform)
{
    switch(Revers)
    {
    case REV_NORMAL:
    {
        FirstSegmentPoint=Spline->First;
        if(!CalcSegment(Transform)) return NULL;
        break;
    }
    case REV_NORM_INV:
    {
        FirstSegmentPoint=Spline->First;
        if(!CalcSegment(Transform)) return NULL;
        break;
    }
    case REV_INVERSE:
    case REV_INV_NORM:
    {
        FirstSegmentPoint=Spline->Last;
        if(!CalcSegment(Transform)) return NULL;
        break;
    }
    default:
    {
        return NULL;
    }
    }
    return &Segment;
}

CSegment* CFigure::GetNextSegment(CTransform &Transform)
{
    if(!FirstSegmentPoint) return NULL;
    switch(Revers)
    {
    case REV_NORMAL:
    case REV_NORM_INV:
    {
        FirstSegmentPoint=FirstSegmentPoint->Next;
        if(!CalcSegment(Transform)) return NULL;
        break;
    }
    case REV_INVERSE:
    case REV_INV_NORM:
    {
        FirstSegmentPoint=FirstSegmentPoint->Prev;
        if(!CalcSegment(Transform)) return NULL;
        break;
    }
    default:
        return NULL;
    }
    return &Segment;
}

BOOL CFigure::CalcSegment(CTransform &Transform)
{
    if(!FirstSegmentPoint) return FALSE;
    switch(Revers)
    {
    case REV_INV_NORM:
    {
        if(!FirstSegmentPoint) return FALSE;
        if(!FirstSegmentPoint->Prev) return FALSE;
        CdbsPoint BufPoint;
        BufPoint.SetPoint(FirstSegmentPoint->Prev->Point.x*CosXX+FirstSegmentPoint->Prev->Point.y*CosYX+x,
                          FirstSegmentPoint->Prev->Point.x*SinXX+FirstSegmentPoint->Prev->Point.y*SinYX+y);
        BufPoint=BufPoint*Transform;
        Segment.x_begin=BufPoint.x;
        Segment.y_begin=BufPoint.y;
        BufPoint.SetPoint(FirstSegmentPoint->Point.x*CosXX+FirstSegmentPoint->Point.y*CosYX+x,
                          FirstSegmentPoint->Point.x*SinXX+FirstSegmentPoint->Point.y*SinYX+y);
        BufPoint=BufPoint*Transform;
        Segment.x_end=BufPoint.x;
        Segment.y_end=BufPoint.y;
        Segment.z=FirstSegmentPoint->Point.z;
        break;
    }
    case REV_NORMAL:
    {
        if(!FirstSegmentPoint) return FALSE;
        if(!FirstSegmentPoint->Next) return FALSE;
        CdbsPoint BufPoint;
        BufPoint.SetPoint(FirstSegmentPoint->Point.x*CosXX+FirstSegmentPoint->Point.y*CosYX+x,
                          FirstSegmentPoint->Point.x*SinXX+FirstSegmentPoint->Point.y*SinYX+y);
        BufPoint=BufPoint*Transform;
        Segment.x_begin=BufPoint.x;
        Segment.y_begin=BufPoint.y;
        BufPoint.SetPoint(FirstSegmentPoint->Next->Point.x*CosXX+FirstSegmentPoint->Next->Point.y*CosYX+x,
                          FirstSegmentPoint->Next->Point.x*SinXX+FirstSegmentPoint->Next->Point.y*SinYX+y);
        BufPoint=BufPoint*Transform;
        Segment.x_end=BufPoint.x;
        Segment.y_end=BufPoint.y;
        Segment.z=FirstSegmentPoint->Point.z;
        break;
    }
    case REV_NORM_INV:
    {
        if(!FirstSegmentPoint) return FALSE;
        if(!FirstSegmentPoint->Next) return FALSE;
        CdbsPoint BufPoint;
        BufPoint.SetPoint(FirstSegmentPoint->Next->Point.x*CosXX+FirstSegmentPoint->Next->Point.y*CosYX+x,
                          FirstSegmentPoint->Next->Point.x*SinXX+FirstSegmentPoint->Next->Point.y*SinYX+y);
        BufPoint=BufPoint*Transform;
        Segment.x_begin=BufPoint.x;
        Segment.y_begin=BufPoint.y;
        BufPoint.SetPoint(FirstSegmentPoint->Point.x*CosXX+FirstSegmentPoint->Point.y*CosYX+x,
                          FirstSegmentPoint->Point.x*SinXX+FirstSegmentPoint->Point.y*SinYX+y);
        BufPoint=BufPoint*Transform;
        Segment.x_end=BufPoint.x;
        Segment.y_end=BufPoint.y;
        Segment.z=FirstSegmentPoint->Point.z;
        break;
    }
    case REV_INVERSE:
    {
        if(!FirstSegmentPoint) return FALSE;
        if(!FirstSegmentPoint->Prev) return FALSE;
        CdbsPoint BufPoint;
        BufPoint.SetPoint(FirstSegmentPoint->Point.x*CosXX+FirstSegmentPoint->Point.y*CosYX+x,
                          FirstSegmentPoint->Point.x*SinXX+FirstSegmentPoint->Point.y*SinYX+y);
        BufPoint=BufPoint*Transform;
        Segment.x_begin=BufPoint.x;
        Segment.y_begin=BufPoint.y;
        BufPoint.SetPoint(FirstSegmentPoint->Prev->Point.x*CosXX+FirstSegmentPoint->Prev->Point.y*CosYX+x,
                          FirstSegmentPoint->Prev->Point.x*SinXX+FirstSegmentPoint->Prev->Point.y*SinYX+y);
        BufPoint=BufPoint*Transform;
        Segment.x_end=BufPoint.x;
        Segment.y_end=BufPoint.y;
        Segment.z=-FirstSegmentPoint->Prev->Point.z;
        break;
    }
    default:
    {
        return FALSE;
    }
    }
    return TRUE;
}

