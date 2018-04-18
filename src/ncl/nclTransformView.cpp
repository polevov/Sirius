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

#include "nclTransformView.h"

CnclTransformView::CnclTransformView():CnclMatrix()
{

}

CnclTransformView::~CnclTransformView()
{

}

//вписывание изображения в экран ForView=TRUE или растр ForView=FALSE
double CnclTransformView::ZoomAll(long x,long y,CnclRect &Rect,BOOL ForView)
{
    Rect.Normalize();
    double width=Rect.Width();
    double height=Rect.Height();
    if(x<=0) x=1;
    if(y<=0) y=1;
    if(width==0) width=1;
    if(height==0) height=1;
    if(ForView)
    {
        x-=1;
        y-=1;
    }
    if(x==0) x=1;
    if(y==0) y=1;
    double scale;
    if(x/width>y/height)
        scale=y/height;
    else
        scale=x/width;
    Zoom(scale,scale);
    if(ForView)
        Move((x-width*scale)/2-Rect.Left*scale-1,
             (y-height*scale)/2-Rect.Bottom*scale+1);
    else
        Move((x-width*scale)/2-Rect.Left*scale,
             (y-height*scale)/2-Rect.Bottom*scale);
    return scale;
}
