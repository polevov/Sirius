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

#ifndef _nclDXF_H_
#define _nclDXF_H_
#include "ncl/nclFile.h"

#include "nclCommonImport.h"

class CnclDXF:public CnclCommonImport
{
public:
    CnclDXF();
    ~CnclDXF();
    void Read(const WCHAR *FileName,BOOL Build,BOOL nest);
    static void Save(CnclNest *Nest,WCHAR *FileName);
private:
    void GetVertex(CnclFile* DXF,double &x,double &y,double &z);
    void ReadLine(CnclFile* DXF);
    void ReadArc(CnclFile* DXF);
    void ReadCircle(CnclFile* DXF);
    void ReadPolyline(CnclFile* DXF);
    void ReadLwPolyline(CnclFile* DXF);
    void ReadBlock(CnclFile* DXF);
    void ReadText(CnclFile* DXF);
    BOOL GetNextLine(CnclFile* DXF);
    WCHAR code[1024];
    WCHAR val[1024];
};

#endif
