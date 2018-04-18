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

#ifndef _DETAIL_
#define _DETAIL_

/*
Этот класс служит для хранения информации о записях
типа 8 и типа 26, а также содержит список контуров составляющих деталь
*/

#include "define.h"
#include "ListOfFigure.h"
#include "FigureList.h"
#include "InFile.h"
#include "Transform.h"

class CDetail  
{
public:
    void GetID(WCHAR *ID_str);
    float Perimeter();
    float Area(); //вычислене площади детали
    void Write(CnclFile *OutFile); //запись детали в файл
    unsigned short GetCountOfFigure(); //число контуров, составляющих деталь
    //        WCHAR* GetLongName(WCHAR* LongName);
    CFigureList Figure; //список контуров, составляющие деталь
    unsigned char Detail_ID[8]; //идентификатор детали
    //	WCHAR LongDetail_ID[30]; //длинное имя детали
    long Count; //количество деталей
    long ID; //номер детали
    CTransform Transform;
    CDetail();
    ~CDetail();
};

#endif
