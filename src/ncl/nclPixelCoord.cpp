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

//#include "stdafx.h"
#include "nclPixelCoord.h"

//конструктор класса
CnclPixelCoord::CnclPixelCoord()
{
    x=0;
    y=0;
}

CnclPixelCoord::CnclPixelCoord(const CnclPoint &Point)
{
    *this=Point;
}

CnclPixelCoord::CnclPixelCoord(long _x, long _y)
{
    Set(_x,_y);
}

//деструктор класса
CnclPixelCoord::~CnclPixelCoord()
{

}

//оператор присваивания
CnclPixelCoord& CnclPixelCoord::operator = (const CnclPixelCoord &PixelCoord)
{
    x=PixelCoord.x;
    y=PixelCoord.y;
    return *this;
}

//оператор присваивания
CnclPixelCoord& CnclPixelCoord::operator = (const CnclPoint &Point)
{
    x=(long)Point.x;
    y=(long)Point.y;
    return *this;
}

//оператор сложения
CnclPixelCoord CnclPixelCoord::operator + (const CnclPixelCoord &PixelCoord)
{
    CnclPixelCoord newPixelCoord;
    newPixelCoord.x=x+PixelCoord.x;
    newPixelCoord.y=y+PixelCoord.y;
    return newPixelCoord;
}

CnclPixelCoord CnclPixelCoord::operator + (const CnclPoint &Point)
{
    CnclPixelCoord newPixelCoord;
    newPixelCoord.x=x+long(Point.x);
    newPixelCoord.y=y+long(Point.y);
    return newPixelCoord;
}

//оператор сравнения
BOOL CnclPixelCoord::operator == (const CnclPixelCoord &PixelCoord)
{
    if(x!=PixelCoord.x) return FALSE;
    if(y!=PixelCoord.y) return FALSE;
    return TRUE;
}

//оператор сравнения
BOOL CnclPixelCoord::operator != (const CnclPixelCoord &PixelCoord)
{
    if(*this==PixelCoord) return FALSE;
    return TRUE;
}

//устанавливает координаты точки
void CnclPixelCoord::Set(long _x,long _y)
{
    x=_x;
    y=_y;
}
