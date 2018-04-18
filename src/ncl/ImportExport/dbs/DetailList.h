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

#ifndef _DETAILLIST_
#define _DETAILLIST_

#include "define.h"
#include "InFile.h"
#include "Detail.h"
#include "ListOfDetail.h"
#include "ListOfSpline.h"

class CDetailList  
{
public:
    long GetCountOfDetail(BOOL WithCount);
    long Rename(long Number); //смена идентификаторов для элементов списка
    void Write(CnclFile *OutFile); //запись деталей в файл
    CListOfDetail *First; //ссылка на первую деталь
    CListOfDetail *Last; //ссылка на последнюю деталь
    void Clear(); //удаляет все динамически созданные данные из памяти
    CDetailList();
    virtual ~CDetailList();
    BOOL Read(CListInFile *Current,CInFile *File,long Count); //считать список деталей из файла
    //Current - указатель на запись типа 8
    //File - узазатель на данные, загруженные из файла геометрии
    //Count - количество деталей данного типа
    BOOL AddDetail(CDetail *DetailAdd); //добавить деталь к списку
    //DetailAdd - ссылка на добавляемую деталь

};

#endif
