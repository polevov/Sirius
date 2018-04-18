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

#ifndef _NCLDETAIL_
#define _NCLDETAIL_

//#include "nclPoint.h"
#include "nclSpline.h"

class CnclDetail  
{
    friend class CnclRastr;
public:
    CnclDetail();
    ~CnclDetail();
    CnclDetail(const CnclDetail &Detail);
    struct SnclSpline:public CnclSpline
    {
        SnclSpline();
        SnclSpline(const CnclSpline &Spline);
        SnclSpline(const SnclSpline &Spline);
        ~SnclSpline();
        SnclSpline *Next;	//указатель на следующую кривую детали
        SnclSpline *Prev;	//указатель на пердывущую кривую детали
        //		operator = (SnclSpline *Spline);
    };

    CnclMatrix Matrix;	//матрица преобразования для детали
    CnclTransformView *DrawMatrix;	//матрица преобразования для вывода детали на экран
    long ID;				//идентификатор детали
    BOOL IsInDetail(CnclPoint Point);
    void EcvBuild(double distance); //построение эквидестанты
    void new_EcvBuild(double distance); //построение эквидестанты

    WCHAR Name[512];	//имя детали
    CnclDetail *Ecv;		//эквидестанта
    SnclSpline *First;		//ссылка на первый контур детали
    SnclSpline *Last;		//ссылка на последний контур детали
    CnclRect RealRect;

    int angle_step;

private:
    void new_EcvSimpl(SnclSpline *Spline);	//упрощение эквидестанты

    void EcvSimpl(SnclSpline *Begin);	//упрощение эквидестанты
    void EcvBreak(SnclSpline *Spline, CnclSpline::SnclSegment *A,CnclSpline::SnclSegment *B,CnclPoint *Point);	//разрыв эквидестанты в точке пересечения на 2 контура
    BOOL EcvCanBreak(SnclSpline *Spline, CnclSpline::SnclSegment *A,CnclSpline::SnclSegment *B);	//можно ли разорвать эквидестанту в точке пересечения этих сегментов
    long SplineCount;		//количество контуров детали

public:

    CnclDetail& operator = (const CnclDetail &Detail); //оператор присваивания для деталей
    void Clear();	//удаление всех контуров из детали
    BOOL Add(SnclSpline *Spline);	//добавляет сплайн к детали
    BOOL Add(CnclSpline *Spline);	//добавляет сплайн к детали
    double Len();	//длина контуров детали
    long Count();	//возвращает количество контуров детали
    double Area();	//расчет площади детали
    //void Draw(BOOL Fill,BOOL DrawArrows);	//вывод детали на экран
    CnclRect Rect(BOOL Draw,CnclTransformView *TransformView);	//расчет габаритного прямоугольника детали
    int IsCrossing();	//проверка на самопересечение детали
    int IsCross(CnclDetail *Detail);	//проверка пересечения с другой деталью
    void Recalc();	//расчет координат детали в соответствии с матрицей Matrix
    void Check();	//проверка и исправление ошибок в геометрии детали
    void Delete(SnclSpline *Spline);	//удаление сплайна из детали
};

#endif
