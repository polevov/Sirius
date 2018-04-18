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

#ifndef _TRANSFORM_
#define _TRANSFORM_

class CTransform  
{
public:
    CTransform Inverse();
    void Rotate(double angle);
    CTransform operator *(CTransform A);
    void Singular();
    void Move(float x,float y);
    float Matrix[3][3];
    void operator=(CTransform A);
    CTransform();
    ~CTransform();

};

#endif
