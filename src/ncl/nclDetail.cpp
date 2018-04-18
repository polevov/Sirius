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

#include "define.h"
#include "nclDetail.h"

//Конструктор класса
CnclDetail::CnclDetail()
{
    First=NULL;
    Last=NULL;
    SplineCount=0;
    DrawMatrix=NULL;
    Name[0]='\0';
    Ecv=NULL;
    angle_step=0;
}

//конструктор класса
CnclDetail::CnclDetail(const CnclDetail &Detail)
{
    First=NULL;
    Last=NULL;
    SplineCount=0;
    DrawMatrix=NULL;
    Name[0]='\0';
    Ecv=NULL;
    *this=Detail;
}

//Деструтор класса
CnclDetail::~CnclDetail()
{
    Clear();
}

//Конструктор класса
CnclDetail::SnclSpline::SnclSpline():CnclSpline()
{
    Next=NULL;
    Prev=NULL;
}

//Деструтор класса
CnclDetail::SnclSpline::~SnclSpline()
{
    Clear();
}

//Удаление всех кривых и
//инициализация переменных класса начальными значениями
void CnclDetail::Clear()
{
    SnclSpline *Current=First;
    while(Current)
    {
        First=First->Next;
        delete Current;
        Current=First;
    }
    First=NULL;
    Last=NULL;
    SplineCount=0;
    if(Ecv)
    {
        delete Ecv;
        Ecv=NULL;
    }
    //	if(Ecv!=this)
    //	{
    //		delete Ecv;
    //		Ecv=this;
    //	}
}

//Добавляет кривую к детали
BOOL CnclDetail::Add(CnclDetail::SnclSpline *Spline)
{
    Spline->Prev=Last;
    if(!First)
        First=Spline;
    if(Last)
        Last->Next=Spline;
    Last=Spline;
    SplineCount++;
    Spline->Matrix=&Matrix;
    Spline->DrawMatrix=&DrawMatrix;
    return TRUE;
}

//вычисление периметра всех контуров, входящих в деталь
double CnclDetail::Len()
{
    double DetailLen=0;
    SnclSpline *Current=First;
    while(Current)
    {
        DetailLen+=Current->Len();
        Current=Current->Next;
    }
    return DetailLen;
}

//возвращает количество контуров входящих в деталь
long CnclDetail::Count()
{
    return SplineCount;
}

//вычисление площади детали
double CnclDetail::Area()
{
    double DetailArea=0;
    double CurrentDetailArea;
    SnclSpline *Current=First;
    while(Current)
    {
        CurrentDetailArea=Current->Area();
        if(Current==First)
            DetailArea=ABS(CurrentDetailArea);
        else
            DetailArea-=ABS(CurrentDetailArea);
        Current=Current->Next;
    }
    return DetailArea;
}

/*
//вывод детали на экран
void CnclDetail::Draw(BOOL Fill,BOOL DrawArrows)
{
    CnclSpline Spline;
    SnclSpline *Current=First;
    GLUtesselator* tobj=NULL;
    if(Fill)
    {
        GLdouble params[4];
        glGetDoublev(GL_CURRENT_COLOR,params);
        params[3]=0.4;
        glColor4dv(params);
        tobj=gluNewTess();
        gluTessCallback(tobj,GLU_TESS_VERTEX,(void(__stdcall*)(void))glVertex3dv);
        gluTessCallback(tobj,GLU_TESS_BEGIN,(void(__stdcall*)(void))beginCallback);
        gluTessCallback(tobj,GLU_TESS_END,(void(__stdcall*)(void))endCallback);
        gluTessCallback(tobj,GLU_TESS_ERROR,(void(__stdcall*)(void))errorCallback);
        gluTessCallback(tobj,GLU_TESS_COMBINE,(void(__stdcall*)(void))combineCallback);
        gluTessBeginPolygon(tobj,NULL);
        while(Current)
        {
            if(Current->IsClose())
            {
                Current->Draw(tobj,DrawArrows);
            }
            Current=Current->Next;
        }
        gluTessEndPolygon(tobj);
        gluDeleteTess(tobj);
        Application->WndView.ClearPoint();
        params[3]=1;
        glColor4dv(params);
    }
    Current=First;
    while(Current)
    {
        Current->Draw(NULL,DrawArrows);
        Current=Current->Next;
    }

}
*/
//вычисление габаритов детали
CnclRect CnclDetail::Rect(BOOL Draw,CnclTransformView *TransformView)
{
    CnclRect RectDetail;
    SnclSpline *Current=First;
    while(Current)
    {
        RectDetail=RectDetail & Current->Rect(Draw,TransformView);
        Current=Current->Next;
    }
    return RectDetail;
}

//Добавляет кривую к детали
BOOL CnclDetail::Add(CnclSpline *Spline)
{
    SnclSpline *newSpline=new SnclSpline(*Spline);
    if(!newSpline)
    {
        //Err.Add(ERR_MEMORY,L"CnclDetail::Add()",ERR_EXIT);
        return FALSE;
    }
    return Add(newSpline);
}

//Конструктор класса
CnclDetail::SnclSpline::SnclSpline(const CnclSpline &Spline):CnclSpline(Spline)
{
    Next=NULL;
    Prev=NULL;
}

//Конструктор класса
CnclDetail::SnclSpline::SnclSpline(const CnclDetail::SnclSpline &Spline)
{
    Clear();
    SnclSegment *Current=Spline.First;
    while(Current)
    {
        Add(Current->RealBegin,Current->RealEnd,Current->z,FALSE);
        Current=Current->Next;
    }
    Matrix=Spline.Matrix;
    DrawMatrix=NULL;
    Next=NULL;
    Prev=NULL;
}

//определяет является ли деталь самопересекающейся
int CnclDetail::IsCrossing()
{
    SnclSpline *Current=First;
    SnclSpline *CurrentForDetail;
    int Result;
    while(Current)
    {
        CurrentForDetail=First;
        while(CurrentForDetail)
        {
            if(CurrentForDetail==Current)
                continue;
            Result=Current->IsCross(CurrentForDetail);
            if(Result==CROSS_YES)
                return CROSS_YES;
            Result=Current->IsCrossing();
            if(Result==CROSS_YES)
                return CROSS_YES;
            CurrentForDetail=CurrentForDetail->Next;
        }
        Current=Current->Next;
    }
    return CROSS_NO;
}

//определяет пересечение с другой деталью
//если детали персекаются, то возвращаем CROSS_YES
//если деталь Detail лежит внутри текущей, то - CROSS_IN
//если текущая деталь лежит внутри Detail - CROSS_ON
//во всех остальных случаях CROSS_NO
int CnclDetail::IsCross(CnclDetail *Detail)
{
    if(!(RealRect&&Detail->RealRect))
        return CROSS_NO;
    SnclSpline *Current=First;
    SnclSpline *CurrentForDetail;
    long Result;
    BOOL InFirst=FALSE;
    while(Current)
    {
        CurrentForDetail=Detail->First;
        while(CurrentForDetail)
        {
            Result=Current->IsCross(CurrentForDetail);
            if(Result==CROSS_YES || Result==CROSS_ON)
                return CROSS_YES;
            if(Result==CROSS_IN)
            {
                if(Current==First)
                    InFirst=TRUE;
                else
                    return CROSS_IN;
            }
            CurrentForDetail=CurrentForDetail->Next;
        }
        Current=Current->Next;
    }
    if(InFirst)
    {
        Current=First;
        while(Current)
        {
            if(Detail->First->IsCross(Current)==CROSS_IN)
                return CROSS_YES;
            Current=Current->Next;
        }
        return CROSS_ON;
    }
    else
        return CROSS_NO;
}

//пересчитать координаты и габариты детали
//в соответствии с матрицей Matrix
void CnclDetail::Recalc()
{
    SnclSpline *Current=First;
    SplineCount=0;
    while(Current)
    {
        SplineCount++;
        Current->Recalc();
        Current=Current->Next;
    }
    RealRect=Rect(FALSE,FALSE);
}

//определяет лежит ли точка внутри детали
BOOL CnclDetail::IsInDetail(CnclPoint Point)
{
    SnclSpline *Current=First;
    BOOL In=FALSE;
    while(Current)
    {
        if(Current->IsInSpline(Point)==IN_YES)
        {
            if(Current==First)
                In=TRUE;
            else
                return FALSE;
        }
        Current=Current->Next;
    }
    if(In)
        return TRUE;
    return FALSE;
}

CnclDetail& CnclDetail::operator = (const CnclDetail &Detail)
{
    if(this==&Detail)
        return *this;
    Clear();
    SnclSpline *Current=Detail.First;
    Matrix=Detail.Matrix;
    DrawMatrix=Detail.DrawMatrix;
    ID=Detail.ID;
    lstrcpy(Name,Detail.Name);
    angle_step=Detail.angle_step;
    while(Current)
    {
        Add(new SnclSpline(*Current));
        Current=Current->Next;
    }
    Recalc();
    if(Ecv)
        delete Ecv;
    Ecv=NULL;
    if(Detail.Ecv!=this && Detail.Ecv)
    {
        Ecv=new CnclDetail;
        if(Ecv)
        {
            *Ecv=*(Detail.Ecv);
            Ecv->DrawMatrix=DrawMatrix; //&Application.DrawTransform;
        }
    }

    return *this;
}

//если distance>0 - эквидестанта строиться наружу
//иначе вовнутрь
void CnclDetail::new_EcvBuild(double distance)
{
    if(Ecv)
        delete Ecv;
    Ecv=new CnclDetail;
    if(!Ecv)
    {
        return;
    }
    //если расстояние ==0, то эквидестантой является сама деталь
    if(distance==0)
    {
        *Ecv=*this;
        Ecv->Recalc();
        return;
    }
    //куда строиться эквидестанта
    //спарва или слева в зависимости от направления контура
    SnclSpline *CurrentSpline=First;
    while(CurrentSpline)
    {
        if(!CurrentSpline->IsClose())
        {
            CurrentSpline=CurrentSpline->Next;
            continue;
        }
        CnclSpline Spline;
        CnclSpline::SnclSegment *CurrentSegment=CurrentSpline->First;
        long direction=SIGN(CurrentSpline->Area());
        double ekv_distance=distance*direction;
        if(CurrentSpline!=First)
            ekv_distance=-ekv_distance;
        while(CurrentSegment)
        {
            //добавляем эквидестанту к текущему сегменту контура
            CnclPoint BeginPoint;
            CnclPoint EndPoint;
            double z;
            //эквидестанта для отрезка прямой
            if(CurrentSegment->z==0)
            {
                CnclVector Vector(CurrentSegment->RealBegin,CurrentSegment->RealEnd);
                double buf=Vector.x;
                Vector.x=-Vector.y;
                Vector.y=buf;
                Vector=Vector*ekv_distance/Vector.Len();
                BeginPoint=CurrentSegment->RealBegin+CnclPoint(Vector.x,Vector.y);
                EndPoint=CurrentSegment->RealEnd+CnclPoint(Vector.x,Vector.y);
                z=0;
            }
            else //эквидестанта для дуги окружности
            {
                CnclPoint Center=CurrentSegment->CenterArc(FALSE,TRUE,NULL);
                double Radius=CurrentSegment->RadiusArc(Center,FALSE,NULL);
                CnclVector BeginVector(Center,CurrentSegment->RealBegin);
                CnclVector EndVector(Center,CurrentSegment->RealEnd);
                BeginVector=BeginVector*ABS(ekv_distance)/BeginVector.Len();
                EndVector=EndVector*ABS(ekv_distance)/EndVector.Len();

                z=CurrentSegment->z;
                if(ekv_distance*CurrentSegment->z<0)
                {
                    BeginPoint=CurrentSegment->RealBegin+CnclPoint(BeginVector.x,BeginVector.y);
                    EndPoint=CurrentSegment->RealEnd+CnclPoint(EndVector.x,EndVector.y);
                }
                else
                {
                    BeginPoint=CurrentSegment->RealBegin+CnclPoint(-BeginVector.x,-BeginVector.y);
                    EndPoint=CurrentSegment->RealEnd+CnclPoint(-EndVector.x,-EndVector.y);
                    if(BeginVector.Len()>=Radius)
                        z=0;
                }
            }
            if(BeginPoint.Distance(EndPoint)>Epsilon && ABS(z)<10000)
            {
                Spline.Add(Spline.Last?Spline.Last->RealEnd:BeginPoint,EndPoint,z,FALSE);
            }

            //вставка дуги между текущим и следующим сегментом
            BeginPoint=EndPoint;
            CnclSpline::SnclSegment *NextSegment=CurrentSegment->Next;
            if(!NextSegment)
                NextSegment=CurrentSpline->First;
            if(NextSegment->z==0)
            {
                CnclVector Vector(NextSegment->RealBegin,NextSegment->RealEnd);
                double buf=Vector.x;
                Vector.x=-Vector.y;
                Vector.y=buf;
                Vector=Vector*ekv_distance/Vector.Len();
                EndPoint=NextSegment->RealBegin+CnclPoint(Vector.x,Vector.y);
            }
            else
            {
                CnclPoint Center=NextSegment->CenterArc(FALSE,TRUE,NULL);
                CnclVector BeginVector(Center,NextSegment->RealBegin);
                BeginVector=BeginVector*ABS(ekv_distance)/BeginVector.Len();
                if(ekv_distance*NextSegment->z<0)
                {
                    EndPoint=NextSegment->RealBegin+CnclPoint(BeginVector.x,BeginVector.y);
                }
                else
                {
                    EndPoint=NextSegment->RealBegin+CnclPoint(-BeginVector.x,-BeginVector.y);
                }
            }
            z=CalcZ(CnclVector(CurrentSegment->RealEnd,BeginPoint),
                    CnclVector(CurrentSegment->RealEnd,EndPoint),-direction);
            if(BeginPoint.Distance(EndPoint)>Epsilon)
            {
                Spline.Add(Spline.Last?Spline.Last->RealEnd:BeginPoint,EndPoint,z,FALSE);
            }

            CurrentSegment=CurrentSegment->Next;
        }
        if(Spline.Count()>0)
        {
            Spline.Last->End=Spline.First->Begin;
            Ecv->Add(&Spline);
        }
        CurrentSpline=CurrentSpline->Next;
    }
    Ecv->Matrix=Matrix;
    Ecv->DrawMatrix=DrawMatrix;
    Ecv->Recalc();
}

void CnclDetail::new_EcvSimpl(CnclDetail::SnclSpline *Spline)	//упрощение эквидестанты
{
    BOOL Loop;
    SnclSpline *buildSpline=Spline;
    do
    {
        if(!buildSpline)
            break;
        Loop=FALSE;
        CnclSpline::SnclSegment *SegmentFirst=buildSpline->First;
        CnclPoint Point;
        while(SegmentFirst && !Loop)
        {
            CnclSpline::SnclSegment *SegmentLast=buildSpline->First;
            while(SegmentLast && !Loop)
            {
                if(SegmentFirst!=SegmentLast)
                {
                    //если найдены пересекающиеся сегменты
                    //то проверяем, чтобы сегменты лежащие между
                    //ними не пересекались между собой
                    if(SegmentFirst->IsCross(SegmentLast,&Point,TRUE)==CROSS_YES)
                    {
                        CnclSpline::SnclSegment *CurrentFirst=SegmentFirst;
                        BOOL Cross=FALSE;
                        while(CurrentFirst!=SegmentLast && !Cross)
                        {
                            CnclSpline::SnclSegment *CurrentLast=SegmentFirst;
                            while(CurrentLast!=SegmentLast && !Cross)
                            {
                                if(CurrentFirst->IsCross(CurrentLast,NULL,TRUE)==CROSS_YES)
                                    Cross=TRUE;
                                CurrentLast=CurrentLast->Next;
                                if(!CurrentLast)
                                    CurrentLast=buildSpline->First;
                            }
                            CurrentFirst=CurrentFirst->Next;
                            if(!CurrentFirst)
                                CurrentFirst=buildSpline->First;
                        }
                        if(!Cross)
                        {
                            EcvBreak(buildSpline,SegmentFirst,SegmentLast,&Point);
                            buildSpline=buildSpline->Next;
                            Loop=TRUE;
                        }
                        break;
                    }
                }
                SegmentLast=SegmentLast->Next;
            }
            SegmentFirst=SegmentFirst->Next;
        }
    }while(Loop);
}

//если distance>0 - эквидестанта строиться наружу
//иначе вовнутрь
void CnclDetail::EcvBuild(double distance)
{
    if(Ecv)
        delete Ecv;
    Ecv=new CnclDetail;
    if(!Ecv)
    {
        return;
    }
    if(distance==0)
    {
        *Ecv=*this;
        Ecv->Recalc();
        return;
    }
    SnclSpline *Current=First;
    CnclSpline::SnclSegment *CurrentSegment;
    CnclSpline::SnclSegment *PrevSegment;
    CnclSpline::SnclSegment *NextSegment;
    double cur_dist=distance;
    double Sign_dist=SIGN(cur_dist);
    double Sign_z;
    double k_cur;
    double k_prev;
    CnclPoint Begin;
    CnclPoint End;
    CnclPoint Center;
    double z;
    CnclVector vBegin;
    CnclVector vEnd;
    CnclVector Vector;
    while(Current)
    {
        double Sign_area=Current->Area();;
        if(Current!=First)
        {
            if(Sign_area>0)
                cur_dist=-distance;
            else
                cur_dist=distance;
            Sign_dist=SIGN(cur_dist);
        }
        else
        {
            if(Sign_area>0)
                cur_dist=distance;
            else
                cur_dist=-distance;
            Sign_dist=SIGN(cur_dist);
        }
        CurrentSegment=Current->First;
        SnclSpline *Spline=new SnclSpline;
        Ecv->Add(Spline);
        while(CurrentSegment)
        {
            Vector.Set(CurrentSegment->RealBegin,CurrentSegment->RealEnd);
            if(CurrentSegment==Current->First)
                PrevSegment=Current->Last;
            else
                PrevSegment=CurrentSegment->Prev;
            if(CurrentSegment==Current->Last)
                NextSegment=Current->First;
            else
                NextSegment=CurrentSegment->Next;

            //вставка дуги между двумя сегментами
            k_cur=ABS(cur_dist)/Vector.Len();
            if(Spline->First)
                Begin=Spline->Last->RealEnd;
            if(PrevSegment->z==0)
            {
                if(!Spline->First)
                {
                    k_prev=ABS(cur_dist)/sqrt(
                                (PrevSegment->RealEnd.x-PrevSegment->RealBegin.x)*
                                (PrevSegment->RealEnd.x-PrevSegment->RealBegin.x)+
                                (PrevSegment->RealEnd.y-PrevSegment->RealBegin.y)*
                                (PrevSegment->RealEnd.y-PrevSegment->RealBegin.y));
                    Begin.Set(
                                PrevSegment->RealEnd.x-Sign_dist*(PrevSegment->RealEnd.y-PrevSegment->RealBegin.y)*k_prev,
                                PrevSegment->RealEnd.y+Sign_dist*(PrevSegment->RealEnd.x-PrevSegment->RealBegin.x)*k_prev);

                }
            }
            else
            {
                if(!Spline->First)
                {
                    CnclVector Vec;
                    Sign_z=SIGN(PrevSegment->z);
                    Center=PrevSegment->CenterArc(FALSE,TRUE,NULL);
                    Vec.Set(Center,PrevSegment->RealEnd);
                    Vec=Vec*(Vec.Len()-cur_dist*Sign_z)/Vec.Len();
                    Begin.Set(Center.x+Vec.x,Center.y+Vec.y);
                }
            }
            if(CurrentSegment->z==0)
            {
                End.Set(
                            CurrentSegment->RealBegin.x-Sign_dist*Vector.y*k_cur,
                            CurrentSegment->RealBegin.y+Sign_dist*Vector.x*k_cur);
                Sign_z=1;
            }
            else
            {
                Sign_z=SIGN(CurrentSegment->z);
                Center=CurrentSegment->CenterArc(FALSE,TRUE,NULL);
                Vector.Set(Center,CurrentSegment->RealBegin);
                Vector=Vector*(Vector.Len()-cur_dist*Sign_z)/Vector.Len();
                End.Set(Center.x+Vector.x,Center.y+Vector.y);
            }
            vBegin.Set(CurrentSegment->RealBegin,Begin);
            vEnd.Set(CurrentSegment->RealBegin,End);
            z=CalcZ(vBegin,vEnd,-Sign_dist);

            if(ABS(z)>Epsilon && ABS(z)<10e4)
            {
                Spline->Add(Begin,End,z,FALSE);
                Ecv->EcvSimpl(Spline);
            }

            //вставка эквидестанты к сегменту
            if(Spline->First)
                Begin=Spline->Last->RealEnd;
            if(!CurrentSegment->Next && Current->IsClose())
            {
                if(Spline->First)
                    End=Spline->First->RealBegin;
                else
                    End=Begin;
            }
            if(CurrentSegment->z==0)
            {
                if(!Spline->First)
                    Begin.Set(CurrentSegment->RealBegin.x-Sign_dist*Vector.y*k_cur,
                              CurrentSegment->RealBegin.y+Sign_dist*Vector.x*k_cur);
                if(CurrentSegment->Next || !Current->IsClose())
                    End.Set(CurrentSegment->RealEnd.x-Sign_dist*Vector.y*k_cur,
                            CurrentSegment->RealEnd.y+Sign_dist*Vector.x*k_cur);
                z=0;
            }
            else
            {
                if(!Spline->First)
                {
                    Center=CurrentSegment->CenterArc(FALSE,TRUE,NULL);
                    Vector.Set(Center,CurrentSegment->RealBegin);
                    Vector=Vector*(Vector.Len()-cur_dist*Sign_z)/Vector.Len();
                    Begin.Set(Center.x+Vector.x,Center.y+Vector.y);
                }
                if(CurrentSegment->Next || !Current->IsClose())
                {
                    Center=CurrentSegment->CenterArc(FALSE,TRUE,NULL);
                    Vector.Set(Center,CurrentSegment->RealEnd);
                    Vector=Vector*(Vector.Len()-cur_dist*Sign_z)/Vector.Len();
                    End.Set(Center.x+Vector.x,Center.y+Vector.y);
                }
                if(ABS(cur_dist)>CurrentSegment->RadiusArc(FALSE,NULL) &&
                        Sign_z>0)
                    z=0;
                else
                    z=CurrentSegment->z;
            }
            if(z==0 || (z!=0 && End.Distance(Begin)>Epsilon))
            {
                Spline->Add(Begin,End,z,FALSE);
            }
            CurrentSegment=CurrentSegment->Next;
        }
        if(!Spline->First)
        {
            Delete(Spline);
        }
        Current=Current->Next;
    }
    Ecv->Matrix=Matrix;
    Ecv->DrawMatrix=DrawMatrix;
    Ecv->Recalc();
}

//удаление самопересечений эквидестанты
void CnclDetail::EcvSimpl(CnclDetail::SnclSpline *Begin)
{
    Recalc();
    BOOL FindCross=TRUE;
    int Result=CROSS_NO;
    CnclPoint Point;
    SnclSpline *CurSpline;
    CnclSpline::SnclSegment *CurSegment;
    CnclSpline::SnclSegment *FirstSegment;


    while(FindCross)
    {
        FindCross=FALSE;
        if(!Begin)
            CurSpline=First;
        else
            CurSpline=Begin;
        while(CurSpline)
        {

            /*
            FirstSegment=CurSpline->First;
            while(FirstSegment)
            {
                CurSegment=FirstSegment->Next;
                while(CurSegment)
                {
                    if(FirstSegment->IsCross(CurSegment,&Point,TRUE)==CROSS_YES)
                        break;
                    CurSegment=CurSegment->Next;
                }
                //если сегмент FirstSegment не пересекается ни с одним из сегментов
                //то делаем его первым
                if(!CurSegment)
                {
                    CurSpline->First->Prev=CurSpline->Last;
                    CurSpline->Last->Next=CurSpline->First;
                    CurSpline->First=FirstSegment;
                    CurSpline->Last=CurSpline->First->Prev;
                    CurSpline->First->Prev=NULL;
                    CurSpline->Last->Next=NULL;
                    break;
                }
                FirstSegment=FirstSegment->Next;
            }
*/
            /*			FirstSegment=CurSpline->First;
            while(FirstSegment)
            {
                FirstSegment=FirstSegment->Next;
                CurSegment=FirstSegment->Next;
                while(CurSegment)
                {
                    CurSegment=CurSegment->Next;
                }
            }
*/			FirstSegment=CurSpline->First;
            while(FirstSegment)
            {
                CurSegment=FirstSegment->Next;
                while(CurSegment)
                {
                    Result=FirstSegment->IsCross(CurSegment,&Point,TRUE);
                    if(Result==CROSS_YES)
                    {
                        FindCross=TRUE;
                        //						Result=FirstSegment->IsCross(CurSegment,&Point,TRUE);
                        //						if(EcvCanBreak(CurSpline,FirstSegment,CurSegment))
                        {
                            //							MessageBox(NULL,"ss","s",0);
                            EcvBreak(CurSpline,FirstSegment,CurSegment,&Point);
                            goto loop;
                        }
                        //						l++;
                        //						if(l>0)
                        //							return;
                    }

                    CurSegment=CurSegment->Next;
                }
                FirstSegment=FirstSegment->Next;
            }
            CurSpline=CurSpline->Next;
        }
loop:;
        //		 return;
    }
}

void CnclDetail::EcvBreak(CnclDetail::SnclSpline *Spline, CnclSpline::SnclSegment *A,CnclSpline::SnclSegment *B,CnclPoint *Point)
{
    double z;
    CnclPoint Begin;
    CnclPoint End;
    //если сегмент А не первый, то делаем его первым
    //	if(Spline->First!=A)
    //	{
    ///		Spline->First->Prev=Spline->Last;
    //		Spline->Last->Next=Spline->First;
    //		Spline->First=A;
    //		Spline->Last=A->Prev;
    //		Spline->First->Prev=NULL;
    //		Spline->Last->Next=NULL;
    //	}

    //создаем второй сплайн, начинающийся с сегмента A->Next
    SnclSpline *newSpline=new SnclSpline;
    newSpline->First=A->Next;
    newSpline->Last=Spline->Last;
    Spline->Last=A;
    Spline->Last->Next=NULL;
    Add(newSpline);

    //добавляем к сплайну newSpline сегмент от Point до конца сегмента Начала newSpline
    End=newSpline->First->RealBegin;
    Begin=*Point;
    if(B->z==0)
    {
        CnclVector v(Begin,End);
        if(v.Len()>Epsilon)
            newSpline->Add(Begin,End,0,TRUE);
    }
    else
    {
        CnclPoint Center=B->CenterArc(FALSE,TRUE,NULL);
        CnclVector vBegin(Center,Begin);
        CnclVector vEnd(Center,End);
        z=CalcZ(vBegin,vEnd,B->z);
        if(ABS(z)>Epsilon && ABS(z)<10e7)
            newSpline->Add(Begin,End,z,TRUE);
    }

    //изменяем первый сегмент у Spline он должен заканчиваться в точке Point
    End=*Point;
    Begin=A->RealBegin;
    if(A->z==0)
    {
        CnclVector v(A->RealEnd,*Point);
        if(v.Len()>Epsilon)
        {
            A->RealEnd=*Point;
        }
        else
        {
            CnclSpline::SnclSegment *Segment;
            Segment=A;
            A=A->Prev;
            if(A)
            {
                A->Next=NULL;
                A->RealEnd=*Point;
                delete Segment;
            }
            else
            {
                Spline->Clear();
            }
        }
    }
    else
    {
        CnclPoint Center=A->CenterArc(FALSE,TRUE,NULL);
        CnclVector vBegin(Center,A->RealBegin);
        CnclVector vEnd(Center,*Point);
        z=CalcZ(vBegin,vEnd,A->z);
        if(ABS(z)>Epsilon && ABS(z)<10e7)
        {
            A->z=z;
            A->RealEnd=*Point;
        }
        else
        {
            CnclSpline::SnclSegment *Segment;
            Segment=A;
            A=A->Prev;
            if(A)
            {
                A->Next=NULL;
                A->z=z;
                A->RealEnd=*Point;
                delete Segment;
            }
            else
            {
                Spline->Clear();
            }
        }
    }

    //добавляем к сплайну Spline сегмент от точки Point до конца сегмента B
    End=B->RealEnd;
    Begin=*Point;
    if(B->z==0)
    {
        CnclVector v(Begin,End);
        if(v.Len()>Epsilon)
            Spline->Add(Begin,End,0,FALSE);
    }
    else
    {
        CnclPoint Center=B->CenterArc(FALSE,TRUE,NULL);
        CnclVector vBegin(Center,Begin);
        CnclVector vEnd(Center,End);
        z=CalcZ(vBegin,vEnd,B->z);
        if(ABS(z)>Epsilon && ABS(z)<10e7)
            Spline->Add(Begin,End,z,FALSE);
    }

    //изменяем последний сегмент у newSpline, он должен кончаться в точке newSpline->First->RealBegin
    if(B->z==0)
    {
        CnclVector v(B->RealEnd,*Point);
        if(v.Len()>Epsilon)
        {
            B->RealEnd=newSpline->First->RealBegin;
        }
        else
        {
            CnclSpline::SnclSegment *Segment;
            Segment=B;
            B=B->Prev;
            if(B)
            {
                B->Prev=NULL;
                delete Segment;
                B->RealEnd=newSpline->First->RealBegin;
            }
            else
            {
                newSpline->Clear();
            }
        }
    }
    else
    {
        CnclPoint Center=B->CenterArc(FALSE,TRUE,NULL);
        CnclVector vBegin(Center,B->RealBegin);
        CnclVector vEnd(Center,*Point);
        z=CalcZ(vBegin,vEnd,B->z);
        if(ABS(z)>Epsilon && ABS(z)<10e7)
        {
            B->z=z;
            B->RealEnd=*Point;
        }
        else
        {
            CnclSpline::SnclSegment *Segment;
            Segment=B;
            B=B->Prev;
            if(B)
            {
                B->Next=NULL;
                B->z=z;
                B->RealEnd=*Point;
                delete Segment;
            }
            else
            {
                newSpline->Clear();
            }
        }
    }

    newSpline->Recalc();
    Spline->Recalc();
}
/*
void CnclDetail::EcvBreak(CnclSpline *Spline, CnclSpline::SnclSegment *A,CnclSpline::SnclSegment *B,CnclPoint *Point)
{
    double z;
    CnclPoint Begin;
    CnclPoint End;
    //если сегмент А не первый, то делаем его первым
    if(Spline->First!=A)
    {
        Spline->First->Prev=Spline->Last;
        Spline->Last->Next=Spline->First;
        Spline->First=A;
        Spline->Last=A->Prev;
        Spline->First->Prev=NULL;
        Spline->Last->Next=NULL;
    }

    //создаем второй сплайн, начинающийся с сегмента B
    SnclSpline *newSpline=new SnclSpline;
    newSpline->First=B;
    newSpline->Last=Spline->Last;
    Spline->Last=B->Prev;
    Spline->Last->Next=NULL;
    Add(newSpline);

    //добавляем к сплайну Spline сегмент от начала сегмента В до точки Point
    End=*Point;
    Begin=B->RealBegin;
    if(B->z==0)
    {
        CnclVector v(Begin,End);
        if(v.Len()>Epsilon)
            Spline->Add(Begin,End,0);
    }
    else
    {
        CnclPoint Center=B->CenterArc(FALSE,TRUE,NULL);
        CnclVector vBegin(Center,Begin);
        CnclVector vEnd(Center,End);
//		z=tan(vEnd.Angle(vBegin)/4);
//		if(SIGN(z)!=SIGN(B->z))
        z=CalcZ(vBegin,vEnd,B->z);
//			z=tan(vBegin.Angle(vEnd)/4);
//		z=ABS(z)*SIGN(B->z);
        if(ABS(z)>Epsilon && ABS(z)<10e7)
            Spline->Add(Begin,End,z);
    }
    //изменяем первый сегмент у newSpline, он должен начинаться из точки Point
    if(newSpline->First->z==0)
    {
        CnclVector v(newSpline->First->RealEnd,*Point);
        if(v.Len()>Epsilon)
        {
            newSpline->First->RealBegin=*Point;
        }
        else
        {
            CnclSpline::SnclSegment *Segment;
            Segment=newSpline->First;
            newSpline->First=newSpline->First->Next;
            newSpline->First->Prev=NULL;
            delete Segment;
            newSpline->First->RealBegin=*Point;
        }
    }
    else
    {
        CnclPoint Center=newSpline->First->CenterArc(FALSE,TRUE,NULL);
        CnclVector vBegin(Center,*Point);
        CnclVector vEnd(Center,newSpline->First->RealEnd);
//		z=tan(vEnd.Angle(vBegin)/4);
//		if(SIGN(z)!=SIGN(newSpline->First->z))
        z=CalcZ(vBegin,vEnd,newSpline->First->z);
//			z=tan(vBegin.Angle(vEnd)/4);
//		z=ABS(z)*SIGN(newSpline->First->z);
        if(ABS(z)>Epsilon && ABS(z)<10e7)
        {
            newSpline->First->z=z;
            newSpline->First->RealBegin=*Point;
        }
        else
        {
            CnclSpline::SnclSegment *Segment;
            Segment=newSpline->First;
            newSpline->First=newSpline->First->Next;
            newSpline->First->Prev=NULL;
            delete Segment;
            newSpline->First->z=z;
            newSpline->First->RealBegin=*Point;
        }
    }
    //изменяем первый сегмент у Spline он должен начинаться в точке Spline->Last->RealEnd
    if(A->z==0)
    {
        CnclVector v(Spline->Last->RealEnd,Spline->First->RealEnd);
        if(v.Len()>Epsilon)
        {
            Spline->First->RealBegin=Spline->Last->RealEnd;
        }
        else
        {
            CnclSpline::SnclSegment *Segment;
            Segment=Spline->First;
            Spline->First=Spline->First->Next;
            Spline->First->Prev=NULL;
            delete Segment;
            Spline->First->RealBegin=Spline->Last->RealEnd;
        }
    }
    else
    {
        CnclPoint Center=Spline->First->CenterArc(FALSE,TRUE,NULL);
        CnclVector vBegin(Center,Spline->Last->RealEnd);
        CnclVector vEnd(Center,Spline->First->RealEnd);
//		z=tan(vEnd.Angle(vBegin)/4);
//		if(SIGN(z)!=SIGN(Spline->First->z))
        z=CalcZ(vBegin,vEnd,Spline->First->z);
//			z=tan(vBegin.Angle(vEnd)/4);
//		z=ABS(z)*SIGN(Spline->First->z);
        if(ABS(z)>Epsilon && ABS(z)<10e7)
        {
            Spline->First->z=z;
            Spline->First->RealBegin=Spline->Last->RealEnd;
        }
        else
        {
            CnclSpline::SnclSegment *Segment;
            Segment=Spline->First;
            Spline->First=newSpline->First->Next;
            Spline->First->Prev=NULL;
            delete Segment;
            Spline->First->z=z;
            Spline->First->RealBegin=Spline->Last->RealEnd;
        }
    }
    //добавляем к newSpline сегмент
    End=newSpline->First->RealBegin;
    Begin=newSpline->Last->RealEnd;
    if(A->z==0)
    {
        CnclVector v(Begin,End);
        if(v.Len()>Epsilon)
            newSpline->Add(Begin,End,0);
    }
    else
    {
        CnclPoint Center=A->CenterArc(FALSE,TRUE,NULL);
        CnclVector vBegin(Center,Begin);
        CnclVector vEnd(Center,End);
//		z=tan(vEnd.Angle(vBegin)/4);
//		if(SIGN(z)!=SIGN(A->z))
        z=CalcZ(vBegin,vEnd,A->z);
//			z=tan(vBegin.Angle(vEnd)/4);
//		z=ABS(z)*SIGN(A->z);
        if(ABS(z)>Epsilon && ABS(z)<10e7)
            newSpline->Add(Begin,End,z);
    }
    newSpline->Recalc();
    Spline->Recalc();
}
*/
//проверка и исправление геометрии детали
void CnclDetail::Check()
{
    //Проверяем имеет ли первый контур самые большие габариты
    SnclSpline *Current=First;
    SnclSpline *Biggest=First;
    while(Current)
    {
        if(ABS(Biggest->Area())<ABS(Current->Area()))
            Biggest=Current;
        Current=Current->Next;
    }
    if(Biggest!=First)
    {
        //		Err<<L"Ошибка в порядке контуров детали \""<<Name<<L"\" исправлена"<<ENDL;
        Biggest->Prev->Next=Biggest->Next;
        if(Biggest->Next)
            Biggest->Next->Prev=Biggest->Prev;
        else
            Last=Biggest->Prev;
        First->Prev=Biggest;
        Biggest->Prev=NULL;
        Biggest->Next=First;
        First=Biggest;
    }
    //Исправляем направление контуров
    Current=First;
    while(Current)
    {
        double area=Current->Area();
        if((Current==First && area>0) || (Current!=First && area<0))
        {
            Current->Reverse();
        }
        Current->Check();
        Current=Current->Next;
    }
    //проверяем на замнкнутость контуров детали
    Current=First;
    while(Current)
    {
        Current->Check();
        Current=Current->Next;
    }
}

//удаление сплайна из детали
void CnclDetail::Delete(CnclDetail::SnclSpline *Spline)
{
    if(!Spline)
        return;
    if(Spline->Next)
    {
        Spline->Next->Prev=Spline->Prev;
    }
    else
    {
        Last=Spline->Prev;
        if(Last)
            Last->Next=NULL;
    }
    if(Spline->Prev)
    {
        Spline->Prev->Next=Spline->Next;
    }
    else
    {
        First=Spline->Next;
        if(First)
            First->Prev=NULL;
    }
    delete Spline;
    //	Spline->Clear();
    SplineCount--;
}

//можно ли разорвать эквидестанту в точке пересечения этих сегментов
//еквидестанту можно разорвать только в том случае, если
//один из получаемых контуров не имеет самопересечений
BOOL CnclDetail::EcvCanBreak(CnclDetail::SnclSpline *Spline, CnclSpline::SnclSegment *A,CnclSpline::SnclSegment *B)
{
    CnclSpline::SnclSegment *CurSegment;
    CnclSpline::SnclSegment *FirstSegment;
    CnclSpline::SnclSegment *OldFirst=Spline->First;
    BOOL AB=FALSE;
    BOOL BA=FALSE;
    //проверяем часть сплайна от А до B
    Spline->First->Prev=Spline->Last;
    Spline->Last->Next=Spline->First;
    Spline->First=A;
    Spline->Last=Spline->First->Prev;
    Spline->First->Prev=NULL;
    Spline->Last->Next=NULL;
    FirstSegment=Spline->First;
    while(FirstSegment!=B && FirstSegment)
    {
        CurSegment=FirstSegment->Next;
        while(CurSegment!=B && CurSegment)
        {
            if(FirstSegment->IsCross(CurSegment,NULL,TRUE)==CROSS_YES)
            {
                AB=TRUE;
                goto end_AB;
            }
            CurSegment=CurSegment->Next;
        }
        FirstSegment=FirstSegment->Next;
    }
end_AB:;
    //проверяем часть сплайна от B до A
    Spline->First->Prev=Spline->Last;
    Spline->Last->Next=Spline->First;
    Spline->First=B;
    Spline->Last=Spline->First->Prev;
    Spline->First->Prev=NULL;
    Spline->Last->Next=NULL;
    FirstSegment=Spline->First;
    while(FirstSegment!=A && FirstSegment)
    {
        CurSegment=FirstSegment->Next;
        while(CurSegment!=A && CurSegment)
        {
            if(FirstSegment->IsCross(CurSegment,NULL,TRUE)==CROSS_YES)
            {
                BA=TRUE;
                goto end_BA;
            }
            CurSegment=CurSegment->Next;
        }
        FirstSegment=FirstSegment->Next;
    }
end_BA:;
    //восстанавливаем последовательность сегментов
    Spline->First->Prev=Spline->Last;
    Spline->Last->Next=Spline->First;
    Spline->First=OldFirst;
    Spline->Last=Spline->First->Prev;
    Spline->First->Prev=NULL;
    Spline->Last->Next=NULL;
    return !(AB && BA);
}
