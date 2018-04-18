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

#ifndef _NCLVECTOR_
#define _NCLVECTOR_

#include "nclPoint.h"

//класс для работы с векторами
class CnclVector
{
public:
    CnclVector();
    CnclVector(const CnclPoint &Begin, const CnclPoint &End);
    ~CnclVector();

    double Angle();							//угол между осью X и вектором
    double Len();							//длина вектора
    double Angle(CnclVector &Vector);		//угол между двумя векторами
    //отсчитывается против часовой стрелки
    //всегда > 0
    double AbsoluteAngle(CnclVector &Vector);	//угол между векторами
    //если угол откладывается по часовой стрелке, <0
    //в противном случае >0
    //значения этой функции лежат на отрезке [-PI, PI]
    void Set(double x1,double y1,double x2,double y2);	//изменение координат вектора
    void Set(const CnclPoint &Begin,const CnclPoint &End);	//изменение координат вектора
    CnclVector& operator =(const CnclVector &Vector);	//присваивание вектора вектору
    double operator *(const CnclVector &Vector);	//скалярное умножение векторов
    CnclVector operator *(double Scale);			//умножение вектора на число
    CnclVector operator /(double Scale);			//деление вектора на число
    double operator &(const CnclVector &Vector);	//векторное умножение векторов

    double y;	//координата X вектора
    double x;	//координата Y вектора
};

#endif
