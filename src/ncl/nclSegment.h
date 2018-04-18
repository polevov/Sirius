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

#ifndef _NCLSEGMENT_
#define _NCLSEGMENT_

#include "nclPoint.h"
#include "nclVector.h"
#include "nclRect.h"
#include "nclMatrix.h"
#include "nclTransformView.h"

//вычисление парметра Z дуги
double CalcZ(CnclVector A,CnclVector B,double z);

//класс для работы с элементорным отрезком (дугой) кривой
class CnclSegment  
{
public:
    BOOL IsOnArc(CnclPoint Point);
    CnclSegment();
    CnclSegment(const CnclPoint &_Begin, const CnclPoint &_End,double _z, BOOL _cutting);
    ~CnclSegment();

    CnclSegment& operator = (const CnclSegment &Segment);
    BOOL operator == (const CnclSegment &Segment);
    BOOL operator != (const CnclSegment &Segment);
    void Set(const CnclPoint &_Begin,const CnclPoint &_End,double _z, BOOL _cutting,
             CnclMatrix **_Matrix,CnclTransformView ***_DrawMatrix);
    void Set(const CnclSegment &Segment);
    //	void Draw(GLUtesselator * tobj);
    BOOL IsContinue(const CnclSegment *Segment);
    BOOL IsNull();
    double Len();
    double Area(double x);
    int IsCross(CnclSegment *Segment,CnclPoint *Point,BOOL Fine);
    void Recalc();
    double GetNewZ();

    //после преобразования Matrix
    CnclPoint Begin;			//координаты точки начала сегмента
    CnclPoint End;				//координаты точки конца сегмента
    BOOL cutting; //совмещенный раз по этому сегменту

    //до преобразования Matrix
    CnclPoint RealBegin;		//координаты точки начала сегмента
    CnclPoint RealEnd;			//координаты точки конца сегмента
    CnclRect RealRect;			//прямоугольник по реальным координатам

    double z;					//tg(a/4), где a-угол дуги, если a=0, то это отрезок
    CnclMatrix **Matrix;		//указатель на ссылку на матрицу преобразования
    CnclTransformView ***DrawMatrix;//указатель на ссылку на матрицу преобразования рисования

    //	CnclPoint GetNewCoord(const CnclPoint &Point,BOOL Draw);		//вычисляет новые координаты точки в соответствии с матрицами преобразования
    CnclPoint GetNewCoord(const CnclPoint &Point,BOOL Draw,CnclTransformView *TransformView);		//вычисляет новые координаты точки в соответствии с матрицами преобразования
    double RadiusArc(BOOL Draw,CnclTransformView *TransformView);
    double RadiusArc(const CnclPoint &Center,BOOL Draw,CnclTransformView *TransformView);
    CnclPoint CenterArc(BOOL Draw,BOOL Real,CnclTransformView *TransformView);
    CnclVector ToVector(BOOL Draw,CnclTransformView *TransformView);
    CnclRect Rect(BOOL Draw,CnclTransformView *TransformView);
    double BeginAngl(CnclPoint Center);
    double EndAngl(CnclPoint Center);
    double Angl(CnclPoint Center);
    //void DrawArrow();	//рисует стрелку для сегмента кривой
};

#endif
