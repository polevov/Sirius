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

#include "ncl/define.h"
#include "nclSegment.h"

//установки для рисование стрелок
#define ArrowLen	13
#define ArrowAngle	PI/16

double CalcZ(CnclVector A,CnclVector B,double z)
{
    double Cos;
    if(B.y-A.y!=0)
    {
        Cos=-(A.x+B.x)/(2*(B.y-A.y));
    }
    else
    {
        if(B.y-A.x==0)
            return 0;
        Cos=(A.y+B.y)/(2*(B.x-A.x));
    }

    if(4*Cos*Cos+1<0)
        return 0;
    if(z>0)
        return 2*Cos+sqrt(4*Cos*Cos+1);
    return 2*Cos-sqrt(4*Cos*Cos+1);
}

//Конструктор класса
CnclSegment::CnclSegment()
{
    z=0;
    Matrix=NULL;
    DrawMatrix=NULL;
    cutting=FALSE;
}

CnclSegment::CnclSegment(const CnclPoint &_Begin, const CnclPoint &_End,double _z,BOOL _cutting)
{
    Set(_Begin,_End,_z,_cutting,NULL,NULL);
}

//Деструктор класса
CnclSegment::~CnclSegment()
{

}

//Оператор присваивания для сегментов кривой
CnclSegment & CnclSegment::operator = (const CnclSegment &Segment)
{
    if(this==&Segment)
        return *this;
    Set(Segment);
    Recalc();
    return *this;
}

//Оперетор сравления для сегментов кривой
BOOL CnclSegment::operator == (const CnclSegment &Segment)
{
    if(this==&Segment)
        return TRUE;
    if(RealBegin!=Segment.RealBegin) return FALSE;
    if(RealEnd!=Segment.RealEnd) return FALSE;
    if(z==Segment.z) return FALSE;
    if(Matrix && Segment.Matrix)
    {
        if(*Matrix!=*Segment.Matrix) return FALSE;
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

//Оперетор сравнения для сегментов кривой
BOOL CnclSegment::operator != (const CnclSegment &Segment)
{
    return !(*this==Segment);
}

//Установить координаты кривой
void CnclSegment::Set(const CnclPoint &_Begin,const CnclPoint &_End,
                      double _z,BOOL _cutting,CnclMatrix **_Matrix,CnclTransformView ***_DrawMatrix)
{
    RealBegin=_Begin;
    RealEnd=_End;
    cutting=_cutting;
    z=_z;
    //			MessageBox(NULL,"dsd","ggggggdddsds",0);
    Matrix=_Matrix;
    //			MessageBox(NULL,"dsd","dsds",0);
    DrawMatrix=_DrawMatrix;
}

//возвращает TRUE, если конец сегмента совпадает с началом сегмента Segment
BOOL CnclSegment::IsContinue(const CnclSegment *Segment)
{
    if(RealEnd==Segment->RealBegin) return TRUE;
    if(RealEnd.Distance(Segment->RealBegin)<0.0001)
    {
        RealEnd=Segment->RealBegin;
        return TRUE;
    }
    return FALSE;
}

//установка параметров (координат) сегмента кривой
void CnclSegment::Set(const CnclSegment &Segment)
{
    Set(Segment.RealBegin,Segment.RealEnd,Segment.z,Segment.cutting,Segment.Matrix,Segment.DrawMatrix);
    Recalc();
}
/*
//вывод сегмента на экран
void CnclSegment::Draw(GLUtesselator * tobj)
{
    double *coord;
    CnclPoint newBegin=GetNewCoord(Begin,TRUE,NULL);
    CnclPoint newEnd=GetNewCoord(End,TRUE,NULL);
    if(z==0)
    {
        if(tobj)
        {
            coord=Application->WndView.AddPoint(newBegin.x,newBegin.y);
            gluTessVertex(tobj,coord,coord);
        }
        else
        {
            glVertex2d(newBegin.x,newBegin.y);
        }
    }
    else
    {
        CnclPoint Center=CenterArc(TRUE,FALSE,NULL);
        double Radius=RadiusArc(Center,TRUE,NULL);
        if(Radius<=0)
        {
            if(tobj)
            {
                coord=Application->WndView.AddPoint(newBegin.x,newBegin.y);
                gluTessVertex(tobj,coord,coord);
            }
            else
            {
                glVertex2d(newBegin.x,newBegin.y);
            }
        }
        else
        {
            double new_z=GetNewZ();
            double angl_begin,angl;
            CnclVector Vec_begin,Vec_end;
            Vec_begin.Set(Center,newBegin);
            Vec_end.Set(Center,newEnd);
            angl_begin=Vec_begin.Angle();
            if(new_z<0)
                angl=Vec_end.Angle(Vec_begin);
            else
                angl=Vec_begin.Angle(Vec_end);
            int PointNum=int((PI*sqrt(Radius)));
            if(PointNum>30) PointNum=30;
            if(PointNum<=2)
            {
                if(tobj)
                {
                    coord=Application->WndView.AddPoint(newBegin.x,newBegin.y);
                    gluTessVertex(tobj,coord,coord);
                }
                else
                {
                    glVertex2d(newBegin.x,newBegin.y);
                }
            }
            else
            {
                double step=angl/double(PointNum-1);
                glVertex2d(newBegin.x,newBegin.y);
                if(new_z<0)
                {
                    for(double i=angl_begin;i>angl_begin-angl;i-=step)
                    {
                        if(tobj)
                        {
                            coord=Application->WndView.AddPoint(cos(i)*Radius+Center.x,sin(i)*Radius+Center.y);
                            gluTessVertex(tobj,coord,coord);
                        }
                        else
                        {
                            glVertex2d(cos(i)*Radius+Center.x,sin(i)*Radius+Center.y);
                        }
                    }
                }
                else
                {
                    for(double i=angl_begin;i<angl_begin+angl;i+=step)
                    {
                        if(tobj)
                        {
                            coord=Application->WndView.AddPoint(cos(i)*Radius+Center.x,sin(i)*Radius+Center.y);
                            gluTessVertex(tobj,coord,coord);
                        }
                        else
                        {
                            glVertex2d(cos(i)*Radius+Center.x,sin(i)*Radius+Center.y);
                        }
                    }
                }
            }
        }
    }
    if(tobj)
    {
        coord=Application->WndView.AddPoint(newEnd.x,newEnd.y);
        gluTessVertex(tobj,coord,coord);
    }
    else
    {
        glVertex2d(newEnd.x,newEnd.y);
    }
}
*/

//вычисление центра дуги
//если это отрезок, то возвращается координаты центра отрезка
//Draw=TRUE - используются матрицы Matrix и DrawMatrix
//Draw=FALSE - используется матрица Matrix
//Real=TRUE - вычисление без использования матриц
CnclPoint CnclSegment::CenterArc(BOOL Draw,BOOL Real,CnclTransformView *TransformView)
{
    CnclPoint newBegin=(Real?RealBegin:GetNewCoord(Begin,Draw,TransformView));
    CnclPoint newEnd=(Real?RealEnd:GetNewCoord(End,Draw,TransformView));
    CnclPoint Point((newBegin.x+newEnd.x)/2,(newBegin.y+newEnd.y)/2);
    if(z==0)
        return Point;
    double new_z=(Real?z:GetNewZ());
    double Cos=(new_z-1/new_z)/4;
    Point.Set(
                (newBegin.x+newEnd.x)/2+(newEnd.y-newBegin.y)*Cos,
                (newBegin.y+newEnd.y)/2-(newEnd.x-newBegin.x)*Cos);
    return Point;
}

double CnclSegment::BeginAngl(CnclPoint Center)
{
    CnclVector Vec_begin,Vec_end;
    Vec_begin.Set(Center,Begin);
    Vec_end.Set(Center,End);
    double angl=Vec_begin.Angle();
    return angl;
}

double CnclSegment::EndAngl(CnclPoint Center)
{
    CnclVector Vec_begin,Vec_end;
    Vec_begin.Set(Center,Begin);
    Vec_end.Set(Center,End);
    double angl=Vec_end.Angle();
    return angl;
}

double CnclSegment::Angl(CnclPoint Center)
{
    CnclVector Vec_begin,Vec_end;
    Vec_begin.Set(Center,Begin);
    Vec_end.Set(Center,End);
    if(z<0)
        return Vec_end.Angle(Vec_begin);
    else
        return Vec_begin.Angle(Vec_end);
}
//вычисление радиуса дуги
//если это отрезок, то возвращаем 0
double CnclSegment::RadiusArc(BOOL Draw,CnclTransformView *TransformView)
{
    if(z==0)
        return 0;
    CnclPoint Center=CenterArc(Draw,FALSE,TransformView);
    return RadiusArc(Center,Draw,TransformView);
}

//вычисление радиуса дуги
//если это отрезок, то возвращаем 0
double CnclSegment::RadiusArc(const CnclPoint &Center,BOOL Draw,CnclTransformView *TransformView)
{
    CnclPoint newBegin=GetNewCoord(Begin,Draw,TransformView);
    CnclPoint newEnd=GetNewCoord(End,Draw,TransformView);
    if(z==0)
        return 0;
    return sqrt((newBegin.x-Center.x)*(newBegin.x-Center.x)+
                (newBegin.y-Center.y)*(newBegin.y-Center.y));
}

//проверка на совпадение конца и начала сегмента
//если конец сегмента совпадает с началом, то возвращаем TRUE
//в противном случае - FLASE
BOOL CnclSegment::IsNull()
{
    if(RealBegin==RealEnd) return TRUE;
    return FALSE;
}

//преобразование сегмента кривой в вектор
//вектор направлен от точки Begin к точке End,
//сегмента
CnclVector CnclSegment::ToVector(BOOL Draw,CnclTransformView *TransformView)
{
    CnclPoint newBegin=GetNewCoord(Begin,Draw,TransformView);
    CnclPoint newEnd=GetNewCoord(End,Draw,TransformView);
    CnclVector Vector(newBegin,newEnd);
    return Vector;
}

//вычисление габаритов сегмента кривой
CnclRect CnclSegment::Rect(BOOL Draw,CnclTransformView *TransformView)
{
    CnclPoint newBegin=GetNewCoord(Begin,Draw,TransformView);
    CnclPoint newEnd=GetNewCoord(End,Draw,TransformView);
    CnclRect SegmentRect;
    if(z==0)
    {
        if(newBegin.y>newEnd.y)
        {
            SegmentRect.Bottom=newEnd.y;
            SegmentRect.Top=newBegin.y;
        }
        else
        {
            SegmentRect.Bottom=newBegin.y;
            SegmentRect.Top=newEnd.y;
        }
        if(newBegin.x>newEnd.x)
        {
            SegmentRect.Left=newEnd.x;
            SegmentRect.Right=newBegin.x;
        }
        else
        {
            SegmentRect.Left=newBegin.x;
            SegmentRect.Right=newEnd.x;
        }
    }
    else
    {
        CnclPoint Center=CenterArc(Draw,FALSE,TransformView);
        double Radius=RadiusArc(Center,Draw,TransformView);
        SegmentRect.Top=Center.y+Radius;
        SegmentRect.Bottom=Center.y-Radius;
        SegmentRect.Right=Center.x+Radius;
        SegmentRect.Left=Center.x-Radius;
        CnclVector ToBegin,ToEnd,ToMiddle;
        ToBegin.Set(Center,newBegin);
        ToEnd.Set(Center,newEnd);
        //проверка верхней точки окружности
        ToMiddle.Set(Center.x,Center.y,Center.x,Center.y+Radius);
        double Pi=0;
        int Sign=1;
        double new_z=GetNewZ();
        if(new_z<0)
        {
            Pi=6*PI;
            Sign=-1;
        }
        if(Pi+Sign*(ToBegin.Angle(ToMiddle)+ToMiddle.Angle(ToEnd)+ToEnd.Angle(ToBegin))>2*PI+Epsilon)
        {
            if(newBegin.y>newEnd.y)
                SegmentRect.Top=newBegin.y;
            else
                SegmentRect.Top=newEnd.y;
        }
        //проверка нижней точки окржности
        ToMiddle.Set(Center.x,Center.y,Center.x,Center.y-Radius);
        if(Pi+Sign*(ToBegin.Angle(ToMiddle)+ToMiddle.Angle(ToEnd)+ToEnd.Angle(ToBegin))>2*PI+Epsilon)
        {
            if(newBegin.y>newEnd.y)
                SegmentRect.Bottom=newEnd.y;
            else
                SegmentRect.Bottom=newBegin.y;
        }
        //проверка правой точки окржности
        ToMiddle.Set(Center.x,Center.y,Center.x+Radius,Center.y);
        if(Pi+Sign*(ToBegin.Angle(ToMiddle)+ToMiddle.Angle(ToEnd)+ToEnd.Angle(ToBegin))>2*PI+Epsilon)
        {
            if(newBegin.x>newEnd.x)
                SegmentRect.Right=newBegin.x;
            else
                SegmentRect.Right=newEnd.x;
        }
        //проверка левой точки окржности
        ToMiddle.Set(Center.x,Center.y,Center.x-Radius,Center.y);
        if(Pi+Sign*(ToBegin.Angle(ToMiddle)+ToMiddle.Angle(ToEnd)+ToEnd.Angle(ToBegin))>2*PI+Epsilon)
        {
            if(newBegin.x>newEnd.x)
                SegmentRect.Left=newEnd.x;
            else
                SegmentRect.Left=newBegin.x;
        }
    }
    return SegmentRect;
}

//вычисление длины сегмента кривой
double CnclSegment::Len()
{
    double LenOfSpline;
    if(z==0)
    {
        LenOfSpline=sqrt((End.x-Begin.x)*(End.x-Begin.x)+
                         (End.y-Begin.y)*(End.y-Begin.y));
    }
    else
    {
        CnclPoint Center=CenterArc(FALSE,FALSE,NULL);
        double Radius=RadiusArc(Center,FALSE,NULL);
        CnclVector ToBegin,ToEnd;
        ToBegin.Set(Center,Begin);
        ToEnd.Set(Center,End);
        double Angle;
        double new_z=GetNewZ();
        if(new_z<0)
            Angle=ToEnd.Angle(ToBegin);
        else
            Angle=ToBegin.Angle(ToEnd);
        LenOfSpline=Angle*Radius;
    }
    return ABS(LenOfSpline);
}

//вычисление площади под сегментом
//до вертикальной оси с координатой X
double CnclSegment::Area(double x)
{
    double AreaOfSegment=(Begin.y-End.y)*(Begin.x+End.x-x*2)/2;
    if(z!=0)
    {
        double Radius=RadiusArc(FALSE,NULL);
        double new_z=GetNewZ();
        double angle=ABS(atan(new_z)*4);
        AreaOfSegment-=SIGN(new_z)*((angle*Radius*Radius-Radius*Radius*sin(angle))/2);
    }
    return AreaOfSegment;
}

//определение пересечения сегментов
//Point - точка пересечения
//Fine==TRUE - поиск первого пересечения (например совпадение концов отрезков)
int CnclSegment::IsCross(CnclSegment *Segment,CnclPoint *Point, BOOL Fine)
{
    int Result=CROSS_NO;
    if(!(RealRect&&Segment->RealRect))
        return Result;
    //если начало и конец одного из контуров совпадают, то возвращаем CROSS_NO
    if((IsNull()) || (Segment->IsNull()))
        return Result;
    if(Begin==Segment->Begin)
        Result=CROSS_BEGIN;
    if(End==Segment->End)
        Result=CROSS_END;
    if(Begin==Segment->End)
        Result=CROSS_PREV;
    if(End==Segment->Begin)
        Result=CROSS_NEXT;
    if(Result!=CROSS_NO && Fine)
        return Result;
    //если сравниваемые сегменты прямые, то возвращаем результат
    //if(z==0 && Segment->z==0) return Result;
    //пересечение отрзков
    if(z==0 && Segment->z==0)
    {
        double ax=End.x-Begin.x;
        double ay=End.y-Begin.y;
        double bx=Segment->End.x-Segment->Begin.x;
        double by=Segment->End.y-Segment->Begin.y;
        double dx=Segment->Begin.x-Begin.x;
        double dy=Segment->Begin.y-Begin.y;

        double D=-ax*by+bx*ay;
        double D1=-dx*by+dy*bx;
        double D2=ax*dy-ay*dx;

        if(!(D>-Epsilon && D<Epsilon))
        {
            double t1=D1/D;
            double t2=D2/D;
            if(((t1+Epsilon)>=0 && (t1-Epsilon)<=1 && (t2+Epsilon)>=0 && (t2-Epsilon)<=1))
            {
                Result=CROSS_YES;
                if(Point)
                {
                    Point->Set(Begin.x+t1*ax,Begin.y+t1*ay);
                    if((Begin.Distance(*Point)<Epsilon || End.Distance(*Point)<Epsilon) &&
                            (Segment->Begin.Distance(*Point)<Epsilon || Segment->End.Distance(*Point)<Epsilon))
                    {
                        Result=CROSS_NO;
                    }
                }
            }
        }
        else
        {
            if(Result!=CROSS_NO)
                return Result;
            double len_ab=ToVector(FALSE,NULL).Len();
            double len_ac=CnclVector(Begin,Segment->Begin).Len();
            double len_cb=CnclVector(End,Segment->Begin).Len();
            if(len_ab==len_ac+len_cb)
            {
                if(Point)
                    *Point=Segment->Begin;
                return CROSS_YES;
            }
            double len_ad=CnclVector(Begin,Segment->End).Len();
            double len_db=CnclVector(End,Segment->End).Len();
            if(len_ab==len_ad+len_db)
            {
                if(Point)
                    *Point=Segment->End;
                return CROSS_YES;
            }
            double len_cd=Segment->ToVector(FALSE,NULL).Len();
            if(len_cd==len_ac+len_ad)
            {
                if(Point)
                    *Point=Begin;
                return CROSS_YES;
            }
        }
    }
    else
    {
        //пересечение дуг окружностей
        if(z!=0 && Segment->z!=0)
        {
            //вычисление центра и радиуса 1 дуги
            CnclPoint Center1=CenterArc(FALSE,FALSE,NULL);
            double Radius1=RadiusArc(Center1,FALSE,NULL);
            //вычисление центра и радиуса 2 дуги
            CnclPoint Center2=Segment->CenterArc(FALSE,FALSE,NULL);
            double Radius2=Segment->RadiusArc(Center2,FALSE,NULL);
            if((Radius1+Radius2)<Center1.Distance(Center2))
                return Result;

            CnclVector Begin1,Begin2,End1,End2,Middle1,Middle2;

            //проверка при совпадении радиусов и центров дуг
            if(Center1.Distance(Center2)<Epsilon)
            {
                if(ABS(Radius1-Radius2)<Epsilon)
                {
                    //начальная точка Segment не лежит на текущем сегменте
                    Begin1.Set(Center1,Begin);
                    End1.Set(Center1,End);
                    Middle1.Set(Center1,Segment->Begin);
                    if(SIGN((Begin1&Middle1))==SIGN((Middle1&End1)))
                    {
                        if(Point)
                            *Point=Segment->Begin;
                        if(Begin.Distance(Segment->Begin)>Epsilon && End.Distance(Segment->Begin)>Epsilon)
                            return CROSS_YES;
                    }
                    //конечная точка Segment не лежит на текущем сегменте
                    Middle1.Set(Center1,Segment->End);
                    if(SIGN((Begin1&Middle1))==SIGN((Middle1&End1)))
                    {
                        if(Point)
                            *Point=Segment->End;
                        if(Begin.Distance(Segment->End)>Epsilon && End.Distance(Segment->End)>Epsilon)
                            return CROSS_YES;
                    }
                    //начальная точка текущего сегмента не лежин на Segment
                    Begin1.Set(Center2,Segment->Begin);
                    End1.Set(Center2,Segment->End);
                    Middle1.Set(Center2,Begin);
                    if(SIGN((Begin1&Middle1))==SIGN((Middle1&End1)))
                    {
                        if(Point)
                            *Point=Begin;
                        if(Segment->Begin.Distance(Begin)>Epsilon && Segment->End.Distance(Begin)>Epsilon)
                            return CROSS_YES;
                    }
                    //начальная точка текущего сегмента не лежин на Segment
                    Middle1.Set(Center2,End);
                    if(SIGN((Begin1&Middle1))==SIGN((Middle1&End1)))
                    {
                        if(Point)
                            *Point=End;
                        if(Segment->Begin.Distance(End)>Epsilon && Segment->End.Distance(End)>Epsilon)
                            return CROSS_YES;
                    }
                }
                return CROSS_NO;
            }

            //вычисление точек пересечения окружностей
            double y1;
            double y2;
            double x1;
            double x2;
            if(ABS(Center2.x-Center1.x)>Epsilon)
            {
                double b=(Center2.y-Center1.y)/(Center2.x-Center1.x);
                double a=(Radius1*Radius1-Radius2*Radius2-Center1.x*Center1.x+
                          Center2.x*Center2.x-Center1.y*Center1.y+Center2.y*Center2.y)/
                        (2*Center2.x-2*Center1.x);
                double n=2*((a-Center2.x)*b+Center2.y);
                double p=(a-Center2.x)*(a-Center2.x)+Center2.y*Center2.y-Radius2*Radius2;
                double d=n*n-4*(b*b+1)*p;
                if(d<0) return Result;
                y1=(n+sqrt(d))/(2*b*b+2);
                y2=(n-sqrt(d))/(2*b*b+2);
                x1=a-y1*b;
                x2=a-y2*b;
            }
            else
            {
                double b=(Center2.x-Center1.x)/(Center2.y-Center1.y);
                double a=(Radius1*Radius1-Radius2*Radius2-Center1.y*Center1.y+
                          Center2.y*Center2.y-Center1.x*Center1.x+Center2.x*Center2.x)/
                        (2*Center2.y-2*Center1.y);
                double n=2*((a-Center2.y)*b+Center2.x);
                double p=(a-Center2.y)*(a-Center2.y)+Center2.x*Center2.x-Radius2*Radius2;
                double d=n*n-4*(b*b+1)*p;
                if(d<0) return Result;
                x1=(n+sqrt(d))/(2*b*b+2);
                x2=(n-sqrt(d))/(2*b*b+2);
                y1=a-x1*b;
                y2=a-x2*b;
            }

            //			double Pi1=0,Pi2=0;
            //			int Sign1=1,Sign2=1;
            //вектор из центра дуги до начала
            Begin1.Set(Center1,Begin);
            Begin2.Set(Center2,Segment->Begin);
            //вектор из центра дуги до конца
            End1.Set(Center1,End);
            End2.Set(Center2,Segment->End);
            //вектор из центра дуги до 1 точки пересечения
            Middle1.Set(Center1.x,Center1.y,x1,y1);
            Middle2.Set(Center2.x,Center2.y,x1,y1);
            //			if(z<0)
            //			{
            ///				Pi1=6*PI;
            //				Sign1=-1;
            //			}
            //			if(Segment->z<0)
            //			{
            //				Pi2=6*PI;
            //				Sign2=-1;
            //			}
            //			if((Pi1+Sign1*(Begin1.Angle(Middle1)+Middle1.Angle(End1)+End1.Angle(Begin1))<2*PI) &&
            //				(Pi2+Sign2*(Begin2.Angle(Middle2)+Middle2.Angle(End2)+End2.Angle(Begin2))<2*PI))
            //			if(ABS(z)>ABS(tan(Begin1.Angle(Middle1)/4)) &&
            //			   ABS(Segment->z)>ABS(tan(Begin2.Angle(Middle2)/4)))
            if(Segment->IsOnArc(CnclPoint(x1,y1)) && IsOnArc(CnclPoint(x1,y1)))
                //			if(SIGN((Begin1&Middle1))==SIGN((Middle1&End1)) &&
                //			   SIGN((Begin2&Middle2))==SIGN((Middle2&End2)))
            {
                Result=CROSS_YES;
                if(Point)
                {
                    Point->Set(x1,y1);
                    if((Begin.Distance(*Point)<Epsilon || End.Distance(*Point)<Epsilon) &&
                            (Segment->Begin.Distance(*Point)<Epsilon || Segment->End.Distance(*Point)<Epsilon))
                    {
                        Result=CROSS_NO;
                    }
                }
            }
            //вектор из центра дуги до 2 точки пересечения
            if(Result!=CROSS_YES)
            {
                Middle1.Set(Center1.x,Center1.y,x2,y2);
                Middle2.Set(Center2.x,Center2.y,x2,y2);
                if(Segment->IsOnArc(CnclPoint(x2,y2)) && IsOnArc(CnclPoint(x2,y2)))
                    //			  if(ABS(z)>ABS(tan(Begin1.Angle(Middle1)/4)) &&
                    //					ABS(Segment->z)>ABS(tan(Begin2.Angle(Middle2)/4)))
                    //				if(SIGN((Begin1&Middle1))==SIGN((Middle1&End1)) &&
                    //				SIGN((Begin2&Middle2))==SIGN((Middle2&End2)))
                {
                    Result=CROSS_YES;
                    if(Point)
                    {
                        Point->Set(x2,y2);
                        if((Begin.Distance(*Point)<Epsilon || End.Distance(*Point)<Epsilon) &&
                                (Segment->Begin.Distance(*Point)<Epsilon || Segment->End.Distance(*Point)<Epsilon))
                        {
                            Result=CROSS_NO;
                        }
                    }
                }
            }
            //			if((Pi1+Sign1*(Begin1.Angle(Middle1)+Middle1.Angle(End1)+End1.Angle(Begin1))<2*PI) &&
            //				(Pi2+Sign2*(Begin2.Angle(Middle2)+Middle2.Angle(End2)+End2.Angle(Begin2))<2*PI))
        }
        //пересечение дуги окружности и отрезка
        else
        {
            double y1,y2,x1,x2;
            CnclVector VecBegin,VecEnd,VecMiddle;
            //			double Sign=1;
            //			double Pi=0;
            CnclPoint Center;
            double Radius;
            double z_d;
            CnclSegment *Arc;
            CnclSegment *Line;
            //если текущий сегмент - отрезок
            if(z==0)
            {
                Arc=Segment;
                Line=this;
                /*				Center=Segment->CenterArc(FALSE,FALSE);
                Radius=Segment->RadiusArc(Center,FALSE);
                x1=Begin.x;
                x2=End.x-Begin.x;
                y1=Begin.y;
                y2=End.y-Begin.y;
                VecBegin.Set(Center,Segment->Begin);
                VecEnd.Set(Center,Segment->End);
                z_d=Segment->z;
                */
                //				if(Segment->GetNewZ()<0)
                //				{
                //					Pi=6*PI;
                //					Sign=-1;
                //				}
            }
            //если сравниваемый сегмент - отрезок
            else
            {
                Arc=this;
                Line=Segment;
                /*
                Center=CenterArc(FALSE,FALSE);
                Radius=RadiusArc(Center,FALSE);
                x1=Segment->Begin.x;
                x2=Segment->End.x-Segment->Begin.x;
                y1=Segment->Begin.y;
                y2=Segment->End.y-Segment->Begin.y;
                VecBegin.Set(Center,Begin);
                VecEnd.Set(Center,End);
                z_d=z;
                */
                //				double new_z=GetNewZ();
                //				if(new_z<0)
                //				{
                //					Pi=6*PI;
                //					Sign=-1;
                //				}
            }
            Center=Arc->CenterArc(FALSE,FALSE,NULL);
            Radius=Arc->RadiusArc(Center,FALSE,NULL);
            x1=Line->Begin.x;
            x2=Line->End.x-Line->Begin.x;
            y1=Line->Begin.y;
            y2=Line->End.y-Line->Begin.y;
            VecBegin.Set(Center,Arc->Begin);
            VecEnd.Set(Center,Arc->End);
            z_d=Arc->z;
            double a=x2*x2+y2*y2;
            double b=2*(y1*y2-Center.y*y2+x1*x2-Center.x*x2);
            double c=(y1-Center.y)*(y1-Center.y)+(x1-Center.x)*(x1-Center.x)-Radius*Radius;
            double d=b*b-4*a*c;
            if(d<0) return Result;
            double t1=(-b+sqrt(d))/(a*2);
            double t2=(-b-sqrt(d))/(a*2);
            if(t1>=0 && t1<=1)
            {
                VecMiddle.Set(Center.x,Center.y,x1+x2*t1,y1+y2*t1);
                //				if(Pi+Sign*(VecBegin.Angle(VecMiddle)+VecMiddle.Angle(VecEnd)+
                //					VecEnd.Angle(VecBegin))<2*PI+Epsilon)
                //				if(SIGN((VecBegin&VecMiddle))==SIGN((VecMiddle&VecEnd)))
                if(Arc->IsOnArc(CnclPoint(x1+x2*t1,y1+y2*t1)))
                    //				if(ABS(z_d)>ABS(tan(VecBegin.Angle(VecMiddle)/4)))
                {
                    Result=CROSS_YES;
                    if(Point)
                    {
                        Point->Set(x1+x2*t1,y1+y2*t1);
                        if((Begin.Distance(*Point)<Epsilon || End.Distance(*Point)<Epsilon) &&
                                (Segment->Begin.Distance(*Point)<Epsilon || Segment->End.Distance(*Point)<Epsilon))
                        {
                            Result=CROSS_NO;
                        }
                    }
                }
            }
            if(t2>=0 && t2<=1 && Result!=CROSS_YES)
            {
                VecMiddle.Set(Center.x,Center.y,x1+x2*t2,y1+y2*t2);
                //				if(Pi+Sign*(VecBegin.Angle(VecMiddle)+VecMiddle.Angle(VecEnd)+
                //					VecEnd.Angle(VecBegin))<2*PI+Epsilon)
                if(Arc->IsOnArc(CnclPoint(x1+x2*t2,y1+y2*t2)))
                    //				if(ABS(z_d)>ABS(tan(VecBegin.Angle(VecMiddle)/4)))
                    //				if(SIGN((VecBegin&VecMiddle))==SIGN((VecMiddle&VecEnd)))
                {
                    Result=CROSS_YES;
                    if(Point)
                    {
                        Point->Set(x1+x2*t2,y1+y2*t2);
                        if((Begin.Distance(*Point)<Epsilon || End.Distance(*Point)<Epsilon) &&
                                (Segment->Begin.Distance(*Point)<Epsilon || Segment->End.Distance(*Point)<Epsilon))
                        {
                            Result=CROSS_NO;
                        }
                    }
                }
            }
        }
    }
    return Result;
}

//рисует стрелку для сегмента кривой
/*
void CnclSegment::DrawArrow()
{
    CnclPoint newBegin=GetNewCoord(Begin,TRUE,NULL);
    CnclPoint newEnd=GetNewCoord(End,TRUE,NULL);
    if(newBegin.Distance(newEnd)<ArrowLen*2)
        return;
    CnclVector Vector;
    if(z==0)
    {
        Vector.Set(newBegin,newEnd);
    }
    else
    {
        double new_z=GetNewZ();
        CnclPoint Center=CenterArc(TRUE,FALSE,NULL);
        Vector.Set(CnclPoint(newEnd.x+SIGN(new_z)*(newEnd.y-Center.y),
                  newEnd.y-SIGN(new_z)*(newEnd.x-Center.x)),newEnd);
    }
    glBegin(GL_POLYGON);
    glVertex2d(newEnd.x,newEnd.y);
    Vector=Vector/Vector.Len()*ArrowLen;
    double x=Vector.x*cos(ArrowAngle)-Vector.y*sin(ArrowAngle);
    double y=Vector.x*sin(ArrowAngle)+Vector.y*cos(ArrowAngle);
    glVertex2d(-x+newEnd.x,-y+newEnd.y);
    x=Vector.x*cos(-ArrowAngle)-Vector.y*sin(-ArrowAngle);
    y=Vector.x*sin(-ArrowAngle)+Vector.y*cos(-ArrowAngle);
    glVertex2d(-x+newEnd.x,-y+newEnd.y);
    glVertex2d(newEnd.x,newEnd.y);
    glEnd();
}
*/

/*
CnclPoint CnclSegment::GetNewCoord(const CnclPoint &Point,BOOL Draw)
{
    CnclPoint newPoint(Point);
//	if(Matrix)
//		if(*Matrix)
//			newPoint=((**Matrix)*newPoint);
    if(Draw && DrawMatrix)
        if(*DrawMatrix)
            if(**DrawMatrix)
                newPoint=(***DrawMatrix*newPoint);
    return newPoint;
}
*/
//вычисляет новые координаты точки в соответствии с матрицами преобразования
CnclPoint CnclSegment::GetNewCoord(const CnclPoint &Point,BOOL Draw,CnclTransformView *TransformView)
{
    CnclPoint newPoint(Point);
    if(TransformView)
        newPoint=(*TransformView)*newPoint;
    else
    {
        if(Draw && DrawMatrix)
            if(*DrawMatrix)
                if(**DrawMatrix)
                    newPoint=(***DrawMatrix*newPoint);
    }
    return newPoint;
}

//вычисляет координаты Begin и End
void CnclSegment::Recalc()
{
    Begin=RealBegin;
    End=RealEnd;
    if(Matrix)
        if(*Matrix)
        {
            Begin=**Matrix*RealBegin;
            End=**Matrix*RealEnd;
        }
}

//вычисляет Z для случая зеркально отражения
double CnclSegment::GetNewZ()
{
    if(Matrix)
        if(*Matrix)
            return(*Matrix)->GetMirror()*z;
    return z;
}

BOOL CnclSegment::IsOnArc(CnclPoint Point)
{
    if(z==0)
        return FALSE;
    double z_abs=ABS(z);
    CnclPoint Center=CenterArc(FALSE,FALSE,NULL);
    CnclVector vBegin(Center,Begin);
    CnclVector vMiddle(Center,Point);
    double z_mid;
    if(z<0)
        z_mid=tan(vMiddle.Angle(vBegin)/4);
    else
        z_mid=tan(vBegin.Angle(vMiddle)/4);
    if(ABS(z_abs)>ABS(z_mid))
        return TRUE;
    return FALSE;
}
