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

#include "common.h"
#include "nclCBS.h"

CnclCBS::CnclCBS():CnclCommonImport()
{
}

CnclCBS::~CnclCBS()
{

}

void CnclCBS::Save(CnclNest *Nest,WCHAR *FileName)
{
    CnclNest::SnclDetail *Current;
    Current=Nest->First;
    long count=0;
    WCHAR CBSFile[MAX_PATH];
    WCHAR CBSstr[64];
    lstrcpy(CBSFile,FileName);
    lstrcpy(&CBSFile[lstrlen(CBSFile)-3],L"cbs");
    CnclFile OutCBS(CBSFile,GENERIC_WRITE,CREATE_ALWAYS);
    swprintf(CBSstr,L"%-8s 1",Nest->Sheet.Name);
    OutCBS<<CBSstr<<ENDL;
    long id;
    WCHAR Name[MAX_PATH];
    do
    {
        Current=Nest->First;
        id=-1;
        count=0;
        while(Current)
        {
            if(Current->ID>0)
            {
                if(id<0)
                {
                    lstrcpy(Name,Current->Name);
                    id=Current->ID;
                }
                if(id==Current->ID)
                {
                    Current->ID=-Current->ID;
                    count++;
                }
            }
            Current=Current->Next;
        }
        if(count>0)
        {
            swprintf(CBSstr,L"%-8s %ld",Name,count);
            OutCBS<<CBSstr<<ENDL;
        }
    }while(count>0);

    Current=Nest->First;
    while(Current)
    {
        Current->ID=ABS(Current->ID);
        Current=Current->Next;
    }
    OutCBS.close();
}
