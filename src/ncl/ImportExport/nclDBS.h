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

#ifndef _nclDBS_H_
#define _nclDBS_H_

#include "nclCommonImport.h"
#include "..\\nclNest.h"

class CnclDBS:public CnclCommonImport
{
public:
    CnclDBS();
    ~CnclDBS();
    void Read(const WCHAR *FileName);
    static void Save(CnclNest *Nest,WCHAR *FileName);
    WCHAR* Trim(WCHAR *str);
};

#endif
