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

#ifndef _NCLNEST_
#define _NCLNEST_

#include "ncldetail.h"

//содержит раскройную карту
class CnclNest  
{
public:
    CnclNest();
    ~CnclNest();
    struct SnclDetail:public CnclDetail
    {
        //		CnclDetail Detail;
        SnclDetail();
        SnclDetail(const CnclDetail &_Detail);
        ~SnclDetail();
        SnclDetail *Next;	//ссылка на следующую деталь
        SnclDetail *Prev;	//ссылка на предыдущую деталь
    };
    void Calc();
    BOOL Add(CnclDetail *Detail);
    void SetSheet(CnclDetail *Detail);
    //void Draw(BOOL DrawArrows);
    void Clear();
    void Save(WCHAR *FileName);
    CnclRect Rect(BOOL Draw,CnclTransformView *TransformView);
    CnclDetail Sheet;
    SnclDetail *First;
    SnclDetail *Last;
    long Count();
    double Area();	//площадь раскроенных деталей
    double Len();	//периметр раскроенных деталей

    double NestFactor;		//коэффициент раскроя
    double CutNestFactor;	//коэффициент раскроя с отрезанной правой частью листа
    CnclRect NestRect;		//габаритный прямоугольник раскроенных делей
    //	double LenNest;			//занятая длина листа
};

#endif
