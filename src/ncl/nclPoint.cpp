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

#include "nclPoint.h"

//конструктор класса
CnclPoint::CnclPoint()
{
    x=0;
    y=0;
}

CnclPoint::CnclPoint(double _x, double _y)
{
    Set(_x,_y);
}

//деструктор класса
CnclPoint::~CnclPoint()
{

}

//оператор присваивания
CnclPoint& CnclPoint::operator = (const CnclPoint &Point)
{
    x=Point.x;
    y=Point.y;
    return *this;
}

//оператор сложения
CnclPoint CnclPoint::operator + (const CnclPoint &Point)
{
    CnclPoint newPoint;
    newPoint.x=x+Point.x;
    newPoint.y=y+Point.y;
    return newPoint;
}

//оператор сравнения
BOOL CnclPoint::operator == (const CnclPoint &Point)
{
    if(x!=Point.x) return FALSE;
    if(y!=Point.y) return FALSE;
    return TRUE;
}

//оператор сравнения
BOOL CnclPoint::operator != (const CnclPoint &Point)
{
    if(*this==Point) return FALSE;
    return TRUE;
}

//переносит точку вывода
//void CnclPoint::MoveTo(HDC hDC)
//{
//	::MoveToEx(hDC,int(x),int(y),NULL);
//}

//рисует линию от текущей точки до точки x,y
//void CnclPoint::LineTo(HDC hDC)
//{
//	::LineTo(hDC,int(x),int(y));
//}

//рисует точку радиусом Round
/*
void CnclPoint::Draw(int Round)
{
}
*/
//устанавливает координаты точки
void CnclPoint::Set(double _x,double _y)
{
    x=_x;
    y=_y;
}

//расстояние между двумя точками
double CnclPoint::Distance(const CnclPoint &Point)
{
    return sqrt((x-Point.x)*(x-Point.x)+(y-Point.y)*(y-Point.y));
}

