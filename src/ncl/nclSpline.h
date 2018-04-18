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

#ifndef _NCLSPLINE_
#define _NCLSPLINE_

#include "nclSegment.h"
#include "nclRect.h"

//класс для работы с кривой
class CnclSpline  
{
public:
    CnclSpline(const CnclSpline &Spline);
    CnclSpline();
    ~CnclSpline();

    //структура данных для организации списка сегментов кривой
    struct SnclSegment:public CnclSegment
    {
        SnclSegment();
        ~SnclSegment();
        SnclSegment(const CnclPoint &_Begin, const CnclPoint &_End, double _z,BOOL _cutting);

        SnclSegment *Next;	//указатель на следующий сегмент кривой
        SnclSegment *Prev;	//указатель на предыдущий сегмент кривой
    };

    CnclMatrix *Matrix;			//ссылка на матрицу преобразования
    CnclTransformView **DrawMatrix;	//ссылка на ссылку матрицы рисования

    SnclSegment *First;		//указатель на первый сегмент кривой
    SnclSegment *Last;		//указатель на последний сегмент кривой
private:
    long SegmentCount;		//количество сегментов кривой
    CnclRect RealRect;		//ограничивающий прямоугольник

public:
    void Insert(SnclSegment *Segment,SnclSegment *newSegment);
    void Check();
    CnclSpline& operator = (const CnclSpline &Spline);
    void Clear();
    long Count();
    BOOL Add(SnclSegment *Segment,BOOL AsFirst);
    BOOL Add(const CnclPoint &Begin, const CnclPoint &End, double z,BOOL AsFirst);
    //	void Draw(GLUtesselator * tobj,BOOL DrawArrows);
    CnclRect Rect(BOOL Draw,CnclTransformView *TransformView);
    double Len();
    double Area();
    int IsCrossing();
    int IsCross(CnclSpline *Spline);
    int IsInSpline(const CnclPoint &Point);
    int IsInSpline(double x,double y);
    BOOL IsClose();
    void Recalc();
    void Delete(SnclSegment *Segment);
    void Reverse();
};

#endif
