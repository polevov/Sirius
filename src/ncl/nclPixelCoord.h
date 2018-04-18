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

#ifndef _NCLPIXELCOORD_H_
#define _NCLPIXELCOORD_H_

#include "nclPoint.h"

//класс для работы с пикселами
class CnclPixelCoord
{
public:
    CnclPixelCoord();
    CnclPixelCoord(const CnclPoint &Point);
    CnclPixelCoord(long _x, long _y);
    ~CnclPixelCoord();

    CnclPixelCoord& operator = (const CnclPixelCoord &PixelCoord);
    CnclPixelCoord& operator = (const CnclPoint &Point);
    BOOL operator == (const CnclPixelCoord &PixelCoord);
    BOOL operator != (const CnclPixelCoord &PixelCoord);
    CnclPixelCoord operator + (const CnclPixelCoord &PixelCoord);
    CnclPixelCoord operator + (const CnclPoint &Point);
    void Set(long _x,long _y);

    long x;	//X координата точки
    long y;	//Y координата точки
};

#endif
