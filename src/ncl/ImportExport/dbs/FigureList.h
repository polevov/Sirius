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

#ifndef _FIGURELIST_
#define _FIGURELIST_

#include "Figure.h"
#include "ListOfFigure.h"
#include "../../nclFile.h"
//#include "Box.h"	// Added by ClassView

/*
Этот класс содержит контура типа 2 и параметры преобразования 
 контуров типа 1
*/

class CFigureList  
{
public:
    void WriteNumber(CnclFile *OutFile); //заись номеров контуров в файл для записи типа 8
    unsigned short GetCountOfFigure(); //возвращает количество контуров, составляющих деталь
    void Write(CnclFile *OutFile,long Group,CTransform &Transform); //запись контуров в файл
    void Clear(); //удаляет все динамически созданные данные из памяти
    CListOfFigure *First; //ссылка на первый контур
    CListOfFigure *Last; //ссылка на последний контур
    CFigureList();
    ~CFigureList();
    BOOL Read(char *Data,CSpline *Spline); //Считать список контуров из файла
    BOOL AddFigure(CFigure *FigureAdd); //добавить контур к списку

};

#endif
