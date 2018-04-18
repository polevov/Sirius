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

#include "nclSpline.h"
#include "ncl/define.h"

CnclSpline::CnclSpline()
{
    First=NULL;
    Last=NULL;
    SegmentCount=0;
    Matrix=NULL;
    DrawMatrix=NULL;
}

CnclSpline::CnclSpline(const CnclSpline &Spline)
{
    First=NULL;
    Last=NULL;
    SegmentCount=0;
    Matrix=NULL;
    DrawMatrix=NULL;
    *this=Spline;
}

CnclSpline::~CnclSpline()
{
    Clear();
}

//Удаление всех сегментов из кривой и
//инициализация переменных класса начальными значениями
void CnclSpline::Clear()
{
    SnclSegment *Current=First;
    while(Current)
    {
        First=First->Next;
        delete Current;
        Current=First;
    }
    First=NULL;
    Last=NULL;
    SegmentCount=0;
    Matrix=NULL;
    DrawMatrix=NULL;
}

//Возвращает количество сегментов кривой
long CnclSpline::Count()
{
    return SegmentCount;
}

//Добавляет сегмент к кривой
BOOL CnclSpline::Add(CnclSpline::SnclSegment *Segment,BOOL AsFirst)
{
    if(AsFirst)
    {
        if(First)
        {
            if(!Segment->IsContinue(First))
            {
                //				Err.Add(ERR_SEGMENT,L"CnclSpline::Add(). Конец добавляемого сегмента "
                //					L"не совпадает с началом первого сегмента контура.",ERR_INFO);
                //				return FALSE;
            }
        }
    }
    else
    {
        if(Last)
        {
            if(!Last->IsContinue(Segment))
            {
                //				Err.Add(ERR_SEGMENT,L"CnclSpline::Add(). Начало добавляемого сегмента "
                //					L"не совпадает с концом последнего сегмента контура.",ERR_INFO);
                //				return FALSE;
            }
        }
    }
    if(Segment->IsNull())
    {
        //		Err.Add(ERR_SEGMENT,L"CnclSpline::Add(). Начало добавляемого сегмента "
        //			L"совпадает с его концом.",ERR_INFO);
        return FALSE;
    }
    Segment->Matrix=&Matrix;
    Segment->DrawMatrix=&DrawMatrix;
    if(AsFirst)
    {
        Segment->Next=First;
        if(!Last)
            Last=Segment;
        if(First)
            First->Prev=Segment;
        First=Segment;
    }
    else
    {
        Segment->Prev=Last;
        if(!First)
        {
            First=Segment;
        }
        if(Last)
            Last->Next=Segment;
        Last=Segment;
    }
    //First->cutting=true;
    SegmentCount++;
    return TRUE;
}

//Создает сегмент и добавляет его к кривой
BOOL CnclSpline::Add(const CnclPoint &Begin, const CnclPoint &End, double z,BOOL AsFirst)
{
    SnclSegment *newSegment=new SnclSegment(Begin,End,z,FALSE);
    if(!newSegment)
    {
        //		Err.Add(ERR_MEMORY,L"CnclSpline::Add()",ERR_EXIT);
        return FALSE;
    }
    if(!Add(newSegment,AsFirst))
    {
        delete newSegment;
        return FALSE;
    }
    return TRUE;
}

//конструктор класса SnclSegment
CnclSpline::SnclSegment::SnclSegment():
    CnclSegment()
{
    Next=NULL;
    Prev=NULL;
}

//деструктор класса SnclSegment
CnclSpline::SnclSegment::~SnclSegment()
{
}

//конструктор класса SnclSegment
CnclSpline::SnclSegment::SnclSegment(const CnclPoint &_Begin, const CnclPoint &_End, double _z,BOOL _cutting):
    CnclSegment(_Begin,_End,_z,_cutting)
{
    Next=NULL;
    Prev=NULL;
}
/*
//вывод кривой на экран
void CnclSpline::Draw(GLUtesselator * tobj,BOOL DrawArrows)
{
    if(tobj)
    {
        gluTessBeginContour(tobj);
    }
    else
    {
        glBegin(GL_LINE_STRIP);
    }
    SnclSegment *Current=First;
    while(Current)
    {
        Current->Draw(tobj);
        Current=Current->Next;
    }
    if(tobj)
    {
        gluTessEndContour(tobj);
    }
    else
    {
        glEnd();
    }
    if(!tobj)
    {
        SnclSegment *Current=First;
        while(Current)
        {
            if(Current->cutting)
            {
                GLdouble params[4];
                glGetDoublev(GL_CURRENT_COLOR,params);
                glColor4d(1,0,0,params[3]);
                glBegin(GL_LINE_STRIP);
                Current->Draw(tobj);
                glEnd();
                glColor4dv(params);
            }
            Current=Current->Next;
        }
    }
    if(DrawArrows)
    {
        Current=First;
        while(Current)
        {
            Current->DrawArrow();
            Current=Current->Next;
        }
    }
}
*/

//вычисление габаритов кривой
CnclRect CnclSpline::Rect(BOOL Draw,CnclTransformView *TransformView)
{
    CnclRect RectSpline;
    SnclSegment *Current=First;
    while(Current)
    {
        RectSpline=RectSpline & Current->Rect(Draw,TransformView);
        Current=Current->Next;
    }
    return RectSpline;
}

//вычисление длины кривой
double CnclSpline::Len()
{
    double LenOfSpline=0;
    SnclSegment *Current=First;
    while(Current)
    {
        LenOfSpline+=Current->Len();
        Current=Current->Next;
    }
    return LenOfSpline;
}

//вычисление плащади кривой
//Если направление контура по часовой стрелке >0
//иначе <0
double CnclSpline::Area()
{
    CnclRect Rect=CnclSpline::Rect(FALSE,NULL);
    SnclSegment *Current=First;
    /*	while(Current)
    {
        Rect=Rect & Current->Rect(FALSE);
        Current=Current->Next;
    }
*/
    double AreaOfSpline=0;
    //	Current=First;
    while(Current)
    {
        AreaOfSpline+=Current->Area(Rect.Left);
        Current=Current->Next;
    }
    return AreaOfSpline;
}

//определяет является ли контур самопересекающимся
int CnclSpline::IsCrossing()
{
    SnclSegment *Current=First;
    SnclSegment *CurrentForSpline;
    int Result;
    while(Current)
    {
        CurrentForSpline=First;
        while(CurrentForSpline)
        {
            if(CurrentForSpline==Current)
                continue;
            Result=Current->IsCross(CurrentForSpline,NULL,FALSE);
            if(Result==CROSS_YES || Result==CROSS_END || Result==CROSS_BEGIN)
                return CROSS_YES;
            CurrentForSpline=CurrentForSpline->Next;
        }
        Current=Current->Next;
    }
    return CROSS_NO;
}

//определяет пересечение с другим контуром
int CnclSpline::IsCross(CnclSpline *Spline)
{
    SnclSegment *Current=First;
    SnclSegment *CurrentForSpline;
    while(Current)
    {
        CurrentForSpline=Spline->First;
        while(CurrentForSpline)
        {
            if(Current->IsCross(CurrentForSpline,NULL,FALSE)!=CROSS_NO)
            {
                //				Current->IsCross(CurrentForSpline,NULL,FALSE);
                return CROSS_YES;
            }
            CurrentForSpline=CurrentForSpline->Next;
        }
        Current=Current->Next;
    }
    if(IsInSpline(Spline->First->Begin)==IN_YES)
        return CROSS_IN;
    if(Spline->IsInSpline(First->Begin)==IN_YES)
        return CROSS_ON;
    return CROSS_NO;
}

//определяет является ли кривая замнкнутой
BOOL CnclSpline::IsClose()
{
    if(!First || !Last)
        return FALSE;
    if(First->RealBegin==Last->RealEnd)
        return TRUE;
    CnclRect curRect=Rect(FALSE,NULL);
    if(ABS(First->RealBegin.x-Last->RealEnd.x)<curRect.Width()/1000 &&
            ABS(First->RealBegin.y-Last->RealEnd.y)<curRect.Height()/1000)
    {
        First->RealBegin=Last->RealEnd;
        return TRUE;
    }
    return FALSE;
}

//проверка на то находится ли точка внутри контура
int CnclSpline::IsInSpline(const CnclPoint &Point)
{
    if(!IsClose()) return IN_SPLINE_NO_CLOSED;
    SnclSegment *Current=First;
    double angl=0;
    CnclVector Begin,End;
    BOOL InSideArc=FALSE;	//попадание в сектор с z<0;
    BOOL OutSideArc=FALSE;	//попадание в сектор с z>0;
    while(Current)
    {
        double z=Current->GetNewZ();
        Begin.Set(Point,Current->Begin);
        End.Set(Point,Current->End);
        angl+=Begin.AbsoluteAngle(End);

        if(z!=0)
        {
            CnclPoint Center=Current->CenterArc(FALSE,FALSE,NULL); //центр дуги
            double Radius=Current->RadiusArc(Center,FALSE,NULL);
            if(CnclVector(Center,Point).Len()<=Radius)
            {
                CnclPoint CenterArc;	//центральная точка дуги
                double angl_begin,angl_all;
                CnclVector Vec_begin,Vec_end;
                angl_begin=CnclVector(Center,Current->Begin).Angle();
                if(z<0)
                {
                    CnclVector vector(Center,Current->Begin);
                    angl_all=CnclVector(Center,Current->End).Angle(vector);
                }
                else
                {
                    CnclVector vector(CnclVector(Center,Current->End));
                    angl_all=CnclVector(Center,Current->Begin).Angle(vector);
                }
                if(z<0)
                {
                    CenterArc.Set(cos(angl_begin-angl_all/2)*Radius+Center.x,
                                  sin(angl_begin-angl_all/2)*Radius+Center.y);
                }
                else
                {
                    CenterArc.Set(cos(angl_begin+angl_all/2)*Radius+Center.x,
                                  sin(angl_begin+angl_all/2)*Radius+Center.y);
                }
                if(SIGN(Current->ToVector(FALSE,NULL)&CnclVector(Current->Begin,CenterArc))==
                        SIGN(Current->ToVector(FALSE,NULL)&CnclVector(Current->Begin,Point)))
                {
                    if(z>0)
                        InSideArc=TRUE;
                    else
                        OutSideArc=TRUE;
                }
            }
        }
        Current=Current->Next;

    }

    if(Area()>0)
    {
        if(OutSideArc)
            return IN_YES;
        if(angl>=Epsilon-2*PI)
            return IN_NO;
        if(InSideArc)
            return IN_NO;
    }
    else
    {
        if(InSideArc)
            return IN_YES;
        if(angl<=2*PI-Epsilon)
            return IN_NO;
        if(OutSideArc)
            return IN_NO;
    }
    return IN_YES;
}

//проверка на то находится ли точка внутри контура
int CnclSpline::IsInSpline(double x,double y)
{
    return IsInSpline(CnclPoint(x,y));
}

void CnclSpline::Recalc()
{
    SnclSegment *Current=First;
    SegmentCount=0;
    while(Current)
    {
        SegmentCount++;
        Current->Recalc();
        Current=Current->Next;
    }
    RealRect=Rect(FALSE,NULL);
}

CnclSpline& CnclSpline::operator = (const CnclSpline &Spline)
{
    Clear();
    SnclSegment *Current=Spline.First;
    while(Current)
    {
        Add(Current->RealBegin,Current->RealEnd,Current->z,FALSE);
        Last->DrawMatrix=&DrawMatrix;
        Current=Current->Next;
    }
    Matrix=Spline.Matrix;
    DrawMatrix=NULL;
    Recalc();
    return *this;
}

//удаляет сегмент из сплайна
//ВНИМАНИЕ!!!!
//Полсле удаления данные на которые ссылается *Segment НЕВЕРНЫ!!!!
//эту переменную нужно переинициализировать!!!!
void CnclSpline::Delete(CnclSpline::SnclSegment *Segment)
{
    if(Segment->Next)
    {
        Segment->Next->Prev=Segment->Prev;
    }
    else
    {
        Last=Segment->Prev;
        if(Last)
            Last->Next=NULL;
    }
    if(Segment->Prev)
    {
        Segment->Prev->Next=Segment->Next;
    }
    else
    {
        First=Segment->Next;
        if(First)
            First->Prev=NULL;
    }
    if(Segment==First && Segment==Last)
    {
        Segment=NULL;
        Clear();
    }
    else
    {
        SegmentCount--;
        delete Segment;
    }
}

void CnclSpline::Check()
{
    if(First==Last)
        return;
    SnclSegment *Current=First;
    SnclSegment *Next;
    while(Current)
    {
        Next=Current->Next;
        if(!Next)
            Next=First;
        if(Current->End.Distance(Next->Begin)<Epsilon)
            Current->End=Next->Begin;
        else
        {
            //			SnclSegment *Segment=new SnclSegment;
            //			Segment->Set(CnclSegment(Current->End,Next->Begin,0));
            //			Insert(Current,Segment);
        }
        Current=Current->Next;
    }
}

void CnclSpline::Insert(SnclSegment *Segment,SnclSegment *newSegment)
{
    if(!newSegment)
        return;
    newSegment->Matrix=&Matrix;
    newSegment->DrawMatrix=&DrawMatrix;
    if(!Segment)
    {
        newSegment->Next=First;
        if(!Last)
            Last=newSegment;
        if(First)
            First->Prev=newSegment;
        First=newSegment;
    }
    else
    {
        newSegment->Prev=Segment;
        newSegment->Next=Segment->Next;
        if(newSegment->Next)
            newSegment->Next->Prev=newSegment;
        else
            Last=newSegment;
        Segment->Next=newSegment;
    }
    SegmentCount++;
}
void CnclSpline::Reverse()
{
    //return;
    SnclSegment *newFirst=Last;
    SnclSegment *newLast=First;
    SnclSegment *current=newFirst;
    SnclSegment *buf;
    while(current)
    {
        buf=current->Next;
        current->Next=current->Prev;
        current->z=-current->z;
        current->Prev=buf;
        CnclPoint buf=current->Begin;
        current->Begin=current->End;
        current->End=buf;
        buf=current->RealBegin;
        current->RealBegin=current->RealEnd;
        current->RealEnd=buf;
        current=current->Next;
    }
    Last=newLast;
    First=newFirst;
}
