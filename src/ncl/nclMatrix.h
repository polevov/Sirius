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

#ifndef _NCLMATRIX_
#define _NCLMATRIX_

#include "nclPoint.h"

#define INVERSE_VERTICAL    0	//отобразить деталь вокруг оси Y
#define INVERSE_HORIZONTAL	1   //отобразить деталь вокруг оси X

class CnclMatrix  
{
public:
    CnclMatrix();
    ~CnclMatrix();

    CnclPoint operator *(const CnclPoint &Point);
    CnclMatrix operator *(const CnclMatrix &otherMatrix);
    CnclMatrix& operator = (const CnclMatrix &otherMatrix);
    void Move(double x,double y);
    void Zoom(double x_zoom,double y_zoom);
    void Identity();
    void Rotate(double angle);
    void Rotate(double angle,CnclPoint &Point);
    CnclMatrix Inverse();
    void Mirror(int Flag);
    int GetMirror();
    double Matrix[3][3];	//матрица преобразований 3х3

private:
    BOOL MirrorFlag;		//флаг показывающий производилось ли зеркальное
    //отражение вдоль оси Y
};

#endif
