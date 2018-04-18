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

#include "nclMatrix.h"

CnclMatrix::CnclMatrix()
{
    Identity();
}

CnclMatrix::~CnclMatrix()
{
    MirrorFlag=FALSE;
}

//оператр присваивания для матриц
CnclMatrix& CnclMatrix::operator = (const CnclMatrix& otherMatrix)
{
    memcpy(Matrix,otherMatrix.Matrix,sizeof(Matrix));
    MirrorFlag=otherMatrix.MirrorFlag;
    return *this;
}

//умножение точки на матрицу
CnclPoint CnclMatrix::operator *(const CnclPoint &Point)
{
    CnclPoint Result;
    double x=Matrix[0][0]*Point.x+Matrix[1][0]*Point.y+Matrix[2][0];
    double y=Matrix[0][1]*Point.x+Matrix[1][1]*Point.y+Matrix[2][1];
    double h=Matrix[0][2]*Point.x+Matrix[1][2]*Point.y+Matrix[2][2];
    if(h!=0)
        Result.Set(x/h,y/h);
    return Result;
}

//умножение матрицы на матрицу
CnclMatrix CnclMatrix::operator *(const CnclMatrix& otherMatrix)
{
    CnclMatrix Result;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            Result.Matrix[i][j]=
                    Matrix[i][0]*otherMatrix.Matrix[0][j]+
                    Matrix[i][1]*otherMatrix.Matrix[1][j]+
                    Matrix[i][2]*otherMatrix.Matrix[2][j];
    Result.MirrorFlag=MirrorFlag^otherMatrix.MirrorFlag;
    return Result;
}

//масштабирование
void CnclMatrix::Zoom(double x_zoom,double y_zoom)
{
    CnclMatrix Buf;
    Buf.Matrix[0][0]=x_zoom;
    Buf.Matrix[1][1]=y_zoom;
    *this=*this*Buf;
}

//параллельный перенос
void CnclMatrix::Move(double x,double y)
{
    CnclMatrix Buf;
    Buf.Matrix[2][0]=x;
    Buf.Matrix[2][1]=y;
    *this=*this*Buf;
}

//делает матрицу единичной
void CnclMatrix::Identity()
{
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
        {
            Matrix[i][j]=0;
            if(i==j) Matrix[i][j]=1;
        }
    MirrorFlag=FALSE;
}

//вращение вокруг точки (0,0)
void CnclMatrix::Rotate(double angle)
{
    CnclMatrix Buf;
    Buf.Matrix[0][0]=float(cos(angle));
    Buf.Matrix[0][1]=float(-sin(angle));
    Buf.Matrix[1][0]=float(sin(angle));
    Buf.Matrix[1][1]=float(cos(angle));
    *this=*this*Buf;
}

//возвращает матрицу обратную данной
CnclMatrix CnclMatrix::Inverse()
{
    CnclMatrix TInverse;
    double det=Matrix[0][0]*(Matrix[1][1]*Matrix[2][2]-Matrix[1][2]*Matrix[2][1])-
            Matrix[0][1]*(Matrix[1][0]*Matrix[2][2]-Matrix[1][2]*Matrix[2][0])+
            Matrix[0][2]*(Matrix[1][0]*Matrix[2][1]-Matrix[1][1]*Matrix[2][0]);
    if(det==0)
        return TInverse;
    TInverse.Matrix[0][0]= (Matrix[1][1]*Matrix[2][2]-Matrix[1][2]*Matrix[2][1]);
    TInverse.Matrix[1][0]=-(Matrix[1][0]*Matrix[2][2]-Matrix[1][2]*Matrix[2][0]);
    TInverse.Matrix[2][0]= (Matrix[1][0]*Matrix[2][1]-Matrix[1][1]*Matrix[2][0]);

    TInverse.Matrix[0][1]=-(Matrix[0][1]*Matrix[2][2]-Matrix[0][2]*Matrix[2][1]);
    TInverse.Matrix[1][1]= (Matrix[0][0]*Matrix[2][2]-Matrix[0][2]*Matrix[2][0]);
    TInverse.Matrix[2][1]=-(Matrix[0][0]*Matrix[2][1]-Matrix[0][1]*Matrix[2][0]);

    TInverse.Matrix[0][2]= (Matrix[0][1]*Matrix[1][2]-Matrix[0][2]*Matrix[1][1]);
    TInverse.Matrix[1][2]=-(Matrix[0][0]*Matrix[1][2]-Matrix[0][2]*Matrix[1][0]);
    TInverse.Matrix[2][2]= (Matrix[0][0]*Matrix[1][1]-Matrix[0][1]*Matrix[1][0]);
    for(long i=0;i<3;i++)
        for(long j=0;j<3;j++)
            TInverse.Matrix[i][j]/=det;
    return TInverse;
}

//зеркальное отражение вдоль оси Y
void CnclMatrix::Mirror(int Flag)
{
    CnclMatrix MirrorMatrix;
    if(Flag==INVERSE_VERTICAL)
        MirrorMatrix.Matrix[0][0]=-1;
    else
        MirrorMatrix.Matrix[1][1]=-1;
    *this=*this*MirrorMatrix;
    MirrorFlag=!MirrorFlag;
}

//если зеркальное отражение выполнено
//только вдоль одной оси, т.е. только
//одна составляющая масштаба то возвращаем -1
//иначе 1
int CnclMatrix::GetMirror()
{
    if(MirrorFlag)
        return -1;
    return 1;
}

//поворот вокруг точки Point
void CnclMatrix::Rotate(double angle,CnclPoint &Point)
{
    Move(-(Point.x),-(Point.y));
    Rotate(angle);
    Move(Point.x,Point.y);
}
