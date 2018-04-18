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

#if !defined(_nclBMP_H_)
#define _nclBMP_H_
#include "../nclNest.h"

class CnclBMP
{
public:
    CnclBMP();
    ~CnclBMP();
    static void Save(CnclNest *Nest,WCHAR *FileName);
    static void Save(CnclDetail *Detail,WCHAR *FileName);
private:
    static void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
                  HBITMAP hBMP, HDC hDC);
    static PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
};

#endif
