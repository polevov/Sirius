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

#include "common.h"

#include "define.h"
#include "Detail.h"

CDetail::CDetail()
{
    //	for(int i=0;i<30;i++)
    //		LongDetail_ID[i]='\0';
}

CDetail::~CDetail()
{

}

unsigned short CDetail::GetCountOfFigure()
{
    return Figure.GetCountOfFigure();
}

/*
WCHAR* CDetail::GetLongName(WCHAR* LongName)
{
  for(int i=0;i<15;i++)
  {
    LongName[i*2]=LongDetail_ID[i*2+1];
    LongName[i*2+1]=LongDetail_ID[i*2];
  }
  return &LongName[0];
}
*/

void CDetail::GetID(WCHAR *ID_str)
{
    MultiByteToWideChar(1251,MB_PRECOMPOSED,(char*)Detail_ID,8,ID_str,8);
    WCHAR buf;
    for(int i=0;i<4;i++)
    {
        buf=ID_str[i*2];
        ID_str[i*2]=ID_str[i*2+1];
        ID_str[i*2+1]=buf;
    }
    ID_str[8]='\0';
}

void CDetail::Write(CnclFile *OutFile)
{
    unsigned short Type26=TYPE_DETAIL_NAME;
    unsigned short Type8=TYPE_DETAIL_SPLINE;
    unsigned short Zerro=0;
    unsigned short Len=5;
    unsigned short IDWrite=(unsigned short)ID;
    Figure.Write(OutFile,ID,Transform);
    OutFile->write((const char*)&Len,2);
    OutFile->write((const char*)&IDWrite,2);
    OutFile->write((const char*)&Len,2);
    OutFile->write((const char*)&Zerro,2);
    OutFile->write((const char*)&Type26,2);
    OutFile->write((const char*)&Zerro,2);
    OutFile->write((const char*)&IDWrite,2);
    OutFile->write((const char*)&Zerro,2);
    OutFile->write((const char*)&Detail_ID,8);
    Len=(unsigned short)(Figure.GetCountOfFigure()+3);
    OutFile->write((const char*)&Len,2);
    OutFile->write((const char*)&IDWrite,2);
    OutFile->write((const char*)&Len,2);
    OutFile->write((const char*)&Zerro,2);
    OutFile->write((const char*)&Type8,2);
    OutFile->write((const char*)&Zerro,2);
    OutFile->write((const char*)&IDWrite,2);
    OutFile->write((const char*)&Zerro,2);
    Figure.WriteNumber(OutFile);
}
