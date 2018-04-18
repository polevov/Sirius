////////////////////////////////////////////////////////
//                                                    //
//           Библиотека классов для работы            //
//              с файлами в формате DBS               //
//                      ©2002                         //
//     Разработчик: Полевов Александр Валерьевич      //
//             аспирантура УГТУ-УПИ                   //
//       Механико Машиностроительный Факультет        //
//                 Кафедра САПР                       //
//                                                    //
////////////////////////////////////////////////////////

#ifndef _SPLINELIST_
#define _SPLINELIST_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Spline.h"
#include "ListOfSpline.h"

/*
Этот класс содержит список контуров типа 1 без указания
угла поворота, смещения и точки отсчета
*/

class CSplineList  
{
public:
    void Clear(); //удаляет все динамически созданные данные из памяти
    CSpline * GetSplineByID(unsigned short ID); //возвращает ссылку на контур по его номеру
    CListOfSpline *First; //ссылка на первый контур
    CListOfSpline *Last; //ссылка на последний контур
    CSplineList();
    ~CSplineList();
    BOOL Read(char *Data); //Считать список контуров из файла
    BOOL AddSpline(CSpline *SplineAdd); //добавить контур к списку
};

#endif 
