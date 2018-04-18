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

#ifndef _nclCBS_H_
#define _nclCBS_H_

#include "nclCommonImport.h"
#include "../nclNest.h"
#include "../nclFile.h"
#include "ncl/define.h"

class CnclCBS:public CnclCommonImport
{
public:
    CnclCBS();
    ~CnclCBS();
    static void Save(CnclNest *Nest,WCHAR *FileName);
};

#endif
