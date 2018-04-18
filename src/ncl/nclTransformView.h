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

#ifndef _NCLTRANSFORMVIEW_
#define _NCLTRANSFORMVIEW_

#include "nclMatrix.h"
#include "nclRect.h"

//класс использующийся для операций над деталями
//для вывода их на экран
class CnclTransformView:public CnclMatrix 
{
public:
    CnclTransformView();
    ~CnclTransformView();
    double ZoomAll(long x,long y,CnclRect &Rect,BOOL ForView);
};

#endif
