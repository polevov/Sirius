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

#include "nclRect.h"

CnclRect::CnclRect()
{
    Bottom=0;
    Top=0;
    Left=0;
    Right=0;
}

CnclRect::~CnclRect()
{

}

//перегруженный оператор присваивания
CnclRect& CnclRect::operator = (const CnclRect &Rect)
{
    Bottom=Rect.Bottom;
    Left=Rect.Left;
    Right=Rect.Right;
    Top=Rect.Top;
    return *this;
}

//обьединение двух квадратор, полученный охватывает исходные
CnclRect CnclRect::operator & (CnclRect Rect)
{
    CnclRect RectResult;
    if(IsEmpty()) return Rect;
    if(Rect.IsEmpty()) return *this;
    Normalize();
    Rect.Normalize();
    RectResult.Bottom=(Rect.Bottom>Bottom ? Bottom : Rect.Bottom);
    RectResult.Left=(Rect.Left>Left ? Left : Rect.Left);
    RectResult.Right=(Rect.Right<Right ? Right : Rect.Right);
    RectResult.Top=(Rect.Top<Top ? Top : Rect.Top);
    return RectResult;
}

//возвращает TRUE если все переменные равны нулю
inline BOOL CnclRect::IsEmpty()
{
    if(Bottom==0 && Top==0 && Left==0 && Right==0) return TRUE;
    return FALSE;
}

//нормализация квадрата top>bottom, right>left
void CnclRect::Normalize()
{
    if(Bottom>Top)
    {
        double buffer=Top;
        Top=Bottom;
        Bottom=buffer;
    }
    if(Left>Right)
    {
        double buffer=Right;
        Right=Left;
        Left=buffer;
    }
}

//возвращает ширину прямоугольника
double CnclRect::Width()
{
    return Right-Left;
}

//возвращает высоту прямоугольника
double CnclRect::Height()
{
    return Top-Bottom;
}

//если прямоугольники пересекаются, то TRUE
BOOL CnclRect::operator && (CnclRect Rect)
{
    if(Bottom>Rect.Top) return FALSE;
    if(Top<Rect.Bottom) return FALSE;
    if(Left>Rect.Right) return FALSE;
    if(Right<Rect.Left) return FALSE;
    return TRUE;
}

CnclRect::CnclRect (double _Left, double _Bottom, double _Top, double _Right)
{
    Top=_Top;
    Bottom=_Bottom;
    Left=_Left;
    Right=_Right;
}

CnclRect::CnclRect(const CnclPoint &Point_1,const CnclPoint &Point_2)
{
    Top=Point_1.y;
    Bottom=Point_2.y;
    Left=Point_1.x;
    Right=Point_2.x;
    Normalize();
}

double CnclRect::Area()
{
    return Height()*Width();
}
