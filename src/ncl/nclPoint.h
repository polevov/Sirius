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

#ifndef _NCLPOINT_H_
#define _NCLPOINT_H_
#include "common.h"

class CnclPoint  
{
public:
    CnclPoint();
    CnclPoint(double _x, double _y);
    ~CnclPoint();

    CnclPoint& operator = (const CnclPoint &Point);
    BOOL operator == (const CnclPoint &Point);
    BOOL operator != (const CnclPoint &Point);
    CnclPoint operator + (const CnclPoint &Point);
    //	void MoveTo(HDC hDC);
    //	void LineTo(HDC hDC);
    //	void Draw(/*HDC hDC,*/int Round/*,BOOL GDI,*//*HWND hWnd*/);
    void Set(double _x,double _y);
    double Distance(const CnclPoint &Point);

    double x;	//X координата точки
    double y;	//Y координата точки
};

#endif
