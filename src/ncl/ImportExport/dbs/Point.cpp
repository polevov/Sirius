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

#include "point.h"

CdbsPoint::CdbsPoint()
{
    x=0;
    y=0;
    z=0;
}

CdbsPoint::~CdbsPoint()
{

}

void CdbsPoint::operator = (CdbsPoint Point)
{
    x=Point.x;
    y=Point.y;
    z=Point.z;
}

CdbsPoint CdbsPoint::operator * (CTransform &Transform)
{
    CdbsPoint Point;
    Point.x=x*Transform.Matrix[0][0]+y*Transform.Matrix[1][0]+Transform.Matrix[2][0];
    Point.y=x*Transform.Matrix[0][1]+y*Transform.Matrix[1][1]+Transform.Matrix[2][1];
    float h=x*Transform.Matrix[0][2]+y*Transform.Matrix[1][2]+Transform.Matrix[2][2];
    if(h!=0)
    {
        Point.x/=h;
        Point.y/=h;
    }
    return Point;
}


void CdbsPoint::SetPoint(float x0, float y0, float z0)
{
    x=x0;
    y=y0;
    z=z0;
}
