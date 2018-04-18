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

#ifndef _NCLRASRT_H_
#define _NCLRASRT_H_

#include "nclRect.h"
#include "nclSegment.h"
#include "nclSpline.h"
#include "nclDetail.h"
#include "nclPixelCoord.h"

#define BIT_COLOR	RGB(0,255,0)	//цвет растра при выводе на экран
#define BIT_LEN		32				//длина элементарного растра

#define SERIAL_CURRENT	0
#define SERIAL_LAST		1
#define SERIAL_FIRST	2
#define SERIAL_UNNOW	-1

#define INTEGER_MAX	0
#define INTEGER_MIN	1

class CnclRastr  
{
public:
    CnclRastr();
    ~CnclRastr();
    void Clear();
    void Create(CnclRect &RealRect,CnclTransformView *_TransformView);
    //	void Draw(unsigned long scale);
    //	void PaintDetail(CnclDetail *Detail, BOOL NeedFill, BOOL Inverse);
    void PaintDetail(const CnclDetail *Detail, BOOL NeedFill, BOOL Inverse);
    int IsCross(unsigned long x_move, unsigned long y_move,CnclRastr &Rastr);
    BOOL Add(unsigned long x_move, unsigned long y_move,CnclRastr &newRastr);
    unsigned long real_width;	//ширина изображения в пикселях
    unsigned long real_height;	//высота изображения в пикселях
    COLORREF Color;
    CnclTransformView *TransformView;

private:
    //	void Rectangle(long x1,long y1, long x2, long y2);
    //точка с координатами (0,0) находится в левом нижнем углу изображения
    unsigned long **Rastr;	//растровое изобрашение
    unsigned long *SinglLine; //линия раскроа
    unsigned long width;		//ширина растра в пикселях
    unsigned long width_bite;	//ширина растра в едининичных растрах
    unsigned long height;		//высота растра в пикселях
    //эти координаты можно назвать относительными
    //так как при рисовании детали в растр она всегда
    //располагается в его левом нижнем углу не зависимо от
    //координат детали в пространстве
    //поэтому для перехода от координат в растре к реальным
    //координатам детали необходимо прибавить к координатам в растре
    //значения left (к X) и bottom (к Y)
    double left;				//координата левой границы растра
    double bottom;			//координата нижней границы растра
    /*
    struct SBit		//структура для доступа к отдельным битам элементарного растра
    {
        unsigned a0: 1;		unsigned a1: 1;		unsigned a2: 1;		unsigned a3: 1;
        unsigned a4: 1;		unsigned a5: 1;		unsigned a6: 1;		unsigned a7: 1;
        unsigned a8: 1;		unsigned a9: 1;		unsigned a10:1;		unsigned a11:1;
        unsigned a12:1;		unsigned a13:1;		unsigned a14:1;		unsigned a15:1;
        unsigned a16:1;		unsigned a17:1;		unsigned a18:1;		unsigned a19:1;
        unsigned a20:1;		unsigned a21:1;		unsigned a22:1;		unsigned a23:1;
        unsigned a24:1;		unsigned a25:1;		unsigned a26:1;		unsigned a27:1;
        unsigned a28:1;		unsigned a29:1;		unsigned a30:1;		unsigned a31:1;
    }*Bit;
*/

    //	CnclPoint LastPoint;	//координаты последней нарисованной точки
    long Moving;			//сдвиг растра вправо относительно x=0

    //переменные используемые при заполнении контура
    //	long last_pixel_y;		//координата Y последнего пиксела установленного SetPixel
    int last_pixel_dir;				//направление последней линии
    int last_nonzerro_dir;	//направление последней негоризонтальной линии
    CnclPixelCoord LastPixelCoord;
    CnclPoint LastPointCoord;
    //	CnclPixelCoord FirstPixelCoord;
    //	CnclPixelCoord LastLinePixel;
    //	int first_nonzerro_dir;
    long Serial;
    int dir_change;

    //	BOOL IsPixel(unsigned long x,unsigned long y);
    //	void Line(CnclPoint Begin,CnclPoint End);
    //	void LineTo(CnclPoint Point);
    void MoveTo(const CnclPoint &Point);
    //	void SetPixel(CnclPoint Point);
    //	void PaintSegment(CnclSegment *Segment);
    //	void PaintSpline(CnclSpline *Spline);
    //	void Fill(CnclDetail *Detail);
    void Invert(const CnclDetail *Detail);
    void Move(long count);
    void new_Move(long count);

    void Fill_Line(const CnclPixelCoord &Begin,const CnclPixelCoord &End);
    void NoFill_Line(const CnclPoint &Begin,const CnclPoint &End);
    void Fill_LineTo(const CnclPixelCoord &Point);
    void NoFill_LineTo(const CnclPoint &Point);
    void Fill_SetPixel(const CnclPixelCoord &Point,int Direction);
    void NoFill_SetPixel(const CnclPoint &Point,BOOL Move);
    void NoFill_PaintSegment(CnclSegment *Segment);
    void Fill_PaintSegment(CnclSegment *Segment);
    void PaintSpline(CnclSpline *Spline, BOOL NeedFill);
    long Integer(double number,long flag);
};

#endif
