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

#include "Transform.h"
#include "define.h"

CTransform::CTransform()
{
    Singular();
}

CTransform::~CTransform()
{

}

void CTransform::Move(float x,float y)
{
    CTransform TMove;
    TMove.Matrix[2][0]=x;
    TMove.Matrix[2][1]=y;
    *this=(*this)*TMove;
}

void CTransform::Singular()
{
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            if(i==j) Matrix[i][j]=1;
            else Matrix[i][j]=0;
}

void	CTransform::operator=(CTransform A)
{
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            Matrix[i][j]=A.Matrix[i][j];
}

CTransform CTransform::operator *(CTransform A)
{
    CTransform B;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            B.Matrix[i][j]=Matrix[i][0]*A.Matrix[0][j]+Matrix[i][1]*A.Matrix[1][j]+Matrix[i][2]*A.Matrix[2][j];
    return B;
}

void CTransform::Rotate(double angle)
{
    CTransform TRotate;
    TRotate.Matrix[0][0]=float(cos(angle));
    TRotate.Matrix[0][1]=float(-sin(angle));
    TRotate.Matrix[1][0]=float(sin(angle));
    TRotate.Matrix[1][1]=float(cos(angle));
    *this=(*this)*TRotate;
}

CTransform CTransform::Inverse()
{
    CTransform TInverse;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            TInverse.Matrix[i][j]=Matrix[i==0?1:0][j==0?1:0]*Matrix[i==2?1:2][j==2?1:2]-
                                                                                   Matrix[i==2?1:2][j==0?1:0]*Matrix[i==0?i=1:1][j==2?1:2];
    float det=Matrix[0][0]*Matrix[1][1]*Matrix[2][2]+Matrix[1][0]*Matrix[2][1]*Matrix[0][2]+
            Matrix[0][1]*Matrix[1][2]*Matrix[2][0]-Matrix[0][2]*Matrix[1][1]*Matrix[2][0]-
            Matrix[1][0]*Matrix[0][1]*Matrix[2][2]-Matrix[2][1]*Matrix[1][2]*Matrix[0][0];
    if(det!=0)
        for(int i=0;i<3;i++)
            for(int j=0;j<3;j++)
                TInverse.Matrix[i][j]/=det;
    return TInverse;
}
