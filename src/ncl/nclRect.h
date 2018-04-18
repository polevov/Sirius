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

#ifndef _NCLRECT_
#define _NCLRECT_

#include "nclPoint.h"

class CnclRect
{
public:
    CnclRect();
    CnclRect(double _Left, double _Bottom, double _Top, double _Right);
    CnclRect(const CnclPoint &Point_1,const CnclPoint &Point_2);
    ~CnclRect();

    void Normalize(); //нормализация квадрата top>bottom, right>left
    inline BOOL IsEmpty(); //возвращает TRUE если все переменные равны нулю
    CnclRect& operator = (const CnclRect &Rect); //перегруженный оператор присваивания
    CnclRect operator & (CnclRect Rect); //обьединение двух квадратор, полученный охватывает исходные
    BOOL operator && (const CnclRect Rect); //если прямоугольники пересекаются, то TRUE

    double Top;		//координата Y верхнего края
    double Bottom;	//координата Y нижнего края
    double Left;	//координата X левого края
    double Right;	//координата X правого края

    double Width();		//возвращает ширину прямоугольника
    double Height();	//возвращает высоту прямоугольника
    double Area();		//возвращает площадь прямоугольника
};

#endif
