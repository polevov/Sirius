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
#include "nclVector.h"

//конструктор класса
CnclVector::CnclVector()
{
    y=0;
    x=0;
}

//конструктор класса
CnclVector::CnclVector(const CnclPoint &Begin, const CnclPoint &End)
{
    Set(Begin.x, Begin.y, End.x, End.y);
}


//деструктор класса
CnclVector::~CnclVector()
{

}

//оператор присваивания для класса
CnclVector& CnclVector::operator =(const CnclVector &Vector)
{
    y=Vector.y;
    x=Vector.x;
    return *this;
}

//скалярное умножение векторов
double CnclVector::operator *(const CnclVector &Vector)
{
    return x*Vector.x+y*Vector.y;
}

//установка координат вектора
void CnclVector::Set(double x1, double y1, double x2, double y2)
{
    x=x2-x1;
    y=y2-y1;
}

//угол между векторами в радианах
double CnclVector::Angle(CnclVector &Vector)
{
    double angle1=Angle();
    double angle2=Vector.Angle();
    if(angle1>angle2)
        angle1-=2*PI;
    //		angle1-=PI;
    //	if(angle2-angle1<0) return 2*PI+(angle2-angle1);
    return angle2-angle1;
}

//длина вектора
double CnclVector::Len()
{
    return sqrt(x*x+y*y);
}

//отклонение вектора от оси OX в радианах
double CnclVector::Angle()
{
    double LenVector=Len();
    if(LenVector<=0) return 0;
    //почему-то бывает, что значения a превышают |1|,
    //возможно это связано с неточностью вычислений
    //поэтому необходима проверка иначе функия acos возвращает неверное значение
    double a=x/LenVector;
    double angl;
    if(a>=1)
    {
        angl=0;
    }
    else
    {
        if(a<=-1)
            angl=PI;
        else
            angl=acos(a);
    }
    if(y<0)
        angl=2*PI-angl;
    return angl;
}

//умножение вектора на скаляр
CnclVector CnclVector::operator *(double Scale)
{
    CnclVector Vector;
    Vector=*this;
    Vector.x*=Scale;
    Vector.y*=Scale;
    return Vector;
}

//установка координат вектора
void CnclVector::Set(const CnclPoint &Begin,const CnclPoint &End)
{
    Set(Begin.x,Begin.y,End.x,End.y);
}

//деление вектора на скаляр
CnclVector CnclVector::operator /(double Scale)
{
    CnclVector Vector;
    Vector=*this;
    Vector.x/=Scale;
    Vector.y/=Scale;
    return Vector;
}

//векторное умножение векторов
double CnclVector::operator &(const CnclVector &Vector)
{
    return x*Vector.y-y*Vector.x;
}

//угол между векторами в радианах
//если угол против часовой стрелки то > 0
//иначе < 0
double CnclVector::AbsoluteAngle(CnclVector &Vector)
{
    double len=Len()*Vector.Len();
    if(len==0)
        return 0;
    double a=((*this)*Vector)/len;
    //почему-то бывает, что значения a превышают |1|,
    //поэтому необходима проверка иначе функия acos возвращает неверное значение
    if(a>=1)
        return 0;
    if(a<=-1)
        return -PI;
    return SIGN((*this)&Vector)*acos(a);
}
