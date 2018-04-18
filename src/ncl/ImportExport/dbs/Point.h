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

#ifndef _CPOINT_
#define _CPOINT_

#include "Transform.h"

class CdbsPoint  
{
public:
    void SetPoint(float x0=0, float y0=0, float z0=0);
    void operator = (CdbsPoint Point);
    CdbsPoint operator * (CTransform &Transform);
    CdbsPoint();
    ~CdbsPoint();
    float x;
    float y;
    float z;
};

#endif
