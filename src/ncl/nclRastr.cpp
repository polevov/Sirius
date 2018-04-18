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

#include "define.h"
#include "nclRastr.h"

CnclRastr::CnclRastr()
{
    Rastr=NULL;
    Moving=0;
    Color=RGB(255,0,0);
    //    last_pixel_dir=-2;
    last_nonzerro_dir=-2;
    //    last_pixel_y=-1;
    Serial=SERIAL_UNNOW;
    TransformView=NULL;
    SinglLine=NULL;
}

CnclRastr::~CnclRastr()
{
    Clear();
}

//освобождение памяти
void CnclRastr::Clear()
{
    if(Rastr)
    {
        for(unsigned long i=0;i<height;i++)
            delete [] Rastr[i];
        delete [] Rastr;
        Rastr=NULL;
    }
    if(SinglLine)
    {
        delete [] SinglLine;
        SinglLine=NULL;
    }
    height=0;
    width=0;
    Moving=0;
    TransformView=NULL;
}

#define BIT_01    0x00000001
#define BIT_02    0x00000002
#define BIT_03    0x00000004
#define BIT_04    0x00000008
#define BIT_05    0x00000010
#define BIT_06    0x00000020
#define BIT_07    0x00000040
#define BIT_08    0x00000080
#define BIT_09    0x00000100
#define BIT_10    0x00000200
#define BIT_11    0x00000400
#define BIT_12    0x00000800
#define BIT_13    0x00001000
#define BIT_14    0x00002000
#define BIT_15    0x00004000
#define BIT_16    0x00008000
#define BIT_17    0x00010000
#define BIT_18    0x00020000
#define BIT_19    0x00040000
#define BIT_20    0x00080000
#define BIT_21    0x00100000
#define BIT_22    0x00200000
#define BIT_23    0x00400000
#define BIT_24    0x00800000
#define BIT_25    0x01000000
#define BIT_26    0x02000000
#define BIT_27    0x04000000
#define BIT_28    0x08000000
#define BIT_29    0x10000000
#define BIT_30    0x20000000
#define BIT_31    0x40000000
#define BIT_32    0x80000000

//создание нового растра
//RealRect - размер изображения, которое необходимо вписать в растр
void CnclRastr::Create(CnclRect &RealRect,CnclTransformView *_TransformView)
{
    if(Rastr)
        Clear();
    TransformView=_TransformView;
    if(RealRect.Width()*RealRect.Height()==0)
        return;
    
    //штрина и длина растра всегда должна быть больше на 1
    //даже в том случае когда RealRect Width и Height
    //целые чила и не изменяются при округлении
    width=long(RealRect.Width())+1;
    height=long(RealRect.Height())+1;

    left=RealRect.Left;
    bottom=RealRect.Bottom;

    Rastr=new unsigned long*[height];
    real_height=height;
    real_width=width;

    //делаем ширину кратную размеру элементарного растра
    width=Integer(double(width)/BIT_LEN,INTEGER_MAX)*BIT_LEN;

    width_bite=width/BIT_LEN+1;    //ширина берется на BIT_LEN бита больше
    //чем требуется для возможности накладывания
    //одного изображения на другое в любом месте
    for(unsigned long i=0;i<height;i++)
    {
        Rastr[i]=new unsigned long[width_bite];
        for(unsigned long j=0;j<width_bite;j++)
            Rastr[i][j]=0;
    }
    SinglLine=new unsigned long[width_bite];
    for(unsigned long j=0;j<width_bite;j++)
        SinglLine[j]=0;
}

//вывод растра в левый нижний угол экрана
/*
void CnclRastr::Draw(unsigned long scale)
{
    if(!Rastr) return;
    glColor3d(1.0,0,0);
    for(unsigned long i=0;i<height;i++)
        for(unsigned long j=0;j<width_bite;j++)
        {
            unsigned long *Bit=&Rastr[i][j];
            if(*Bit&BIT_01)Rectangle(j*BIT_LEN*scale,i*scale-1,     (j*BIT_LEN)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_02)Rectangle((j*BIT_LEN+1)*scale,i*scale-1, (j*BIT_LEN+1)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_03)Rectangle((j*BIT_LEN+2)*scale,i*scale-1, (j*BIT_LEN+2)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_04)Rectangle((j*BIT_LEN+3)*scale,i*scale-1, (j*BIT_LEN+3)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_05)Rectangle((j*BIT_LEN+4)*scale,i*scale-1, (j*BIT_LEN+4)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_06)Rectangle((j*BIT_LEN+5)*scale,i*scale-1, (j*BIT_LEN+5)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_07)Rectangle((j*BIT_LEN+6)*scale,i*scale-1, (j*BIT_LEN+6)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_08)Rectangle((j*BIT_LEN+7)*scale,i*scale-1, (j*BIT_LEN+7)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_09)Rectangle((j*BIT_LEN+8)*scale,i*scale-1, (j*BIT_LEN+8)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_10)Rectangle((j*BIT_LEN+9)*scale,i*scale-1, (j*BIT_LEN+9)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_11)Rectangle((j*BIT_LEN+10)*scale,i*scale-1,(j*BIT_LEN+10)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_12)Rectangle((j*BIT_LEN+11)*scale,i*scale-1,(j*BIT_LEN+11)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_13)Rectangle((j*BIT_LEN+12)*scale,i*scale-1,(j*BIT_LEN+12)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_14)Rectangle((j*BIT_LEN+13)*scale,i*scale-1,(j*BIT_LEN+13)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_15)Rectangle((j*BIT_LEN+14)*scale,i*scale-1,(j*BIT_LEN+14)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_16)Rectangle((j*BIT_LEN+15)*scale,i*scale-1,(j*BIT_LEN+15)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_17)Rectangle((j*BIT_LEN+16)*scale,i*scale-1,(j*BIT_LEN+16)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_18)Rectangle((j*BIT_LEN+17)*scale,i*scale-1,(j*BIT_LEN+17)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_19)Rectangle((j*BIT_LEN+18)*scale,i*scale-1,(j*BIT_LEN+18)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_20)Rectangle((j*BIT_LEN+19)*scale,i*scale-1,(j*BIT_LEN+19)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_21)Rectangle((j*BIT_LEN+20)*scale,i*scale-1,(j*BIT_LEN+20)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_22)Rectangle((j*BIT_LEN+21)*scale,i*scale-1,(j*BIT_LEN+21)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_23)Rectangle((j*BIT_LEN+22)*scale,i*scale-1,(j*BIT_LEN+22)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_24)Rectangle((j*BIT_LEN+23)*scale,i*scale-1,(j*BIT_LEN+23)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_25)Rectangle((j*BIT_LEN+24)*scale,i*scale-1,(j*BIT_LEN+24)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_26)Rectangle((j*BIT_LEN+25)*scale,i*scale-1,(j*BIT_LEN+25)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_27)Rectangle((j*BIT_LEN+26)*scale,i*scale-1,(j*BIT_LEN+26)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_28)Rectangle((j*BIT_LEN+27)*scale,i*scale-1,(j*BIT_LEN+27)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_29)Rectangle((j*BIT_LEN+28)*scale,i*scale-1,(j*BIT_LEN+28)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_30)Rectangle((j*BIT_LEN+29)*scale,i*scale-1,(j*BIT_LEN+29)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_31)Rectangle((j*BIT_LEN+30)*scale,i*scale-1,(j*BIT_LEN+30)*scale+scale,i*scale+scale-1);
            if(*Bit&BIT_32)Rectangle((j*BIT_LEN+31)*scale,i*scale-1,(j*BIT_LEN+31)*scale+scale,i*scale+scale-1);
        }
}
*/

/*
//рисование сегмента в битовом массиве
void CnclRastr::PaintSegment(CnclSegment *Segment)
{
    CnclPoint newBegin=Segment->GetNewCoord(Segment->Begin,TRUE);
    CnclPoint newEnd=Segment->GetNewCoord(Segment->End,TRUE);
    if(Segment->z==0)
    {
        Line(CnclPixelCoord(CnclPoint(-left,-bottom)+newBegin),CnclPixelCoord(CnclPoint(-left,-bottom)+newEnd));
    }
    else
    {
        CnclPoint Center=Segment->CenterArc(TRUE);
        double Radius=Segment->RadiusArc(Center,TRUE);
        if(Radius<=0)
        {
            Line(newBegin+CnclPoint(-left,-bottom),newEnd+CnclPoint(-left,-bottom));
        }
        else
        {
            double angl_begin,angl;
            CnclVector Vec_begin,Vec_end;
            Vec_begin.Set(Center,newBegin);
            Vec_end.Set(Center,newEnd);
            angl_begin=Vec_begin.Angle();
            double z=Segment->GetNewZ();
            if(z<0)
                angl=Vec_end.Angle(Vec_begin);
            else
                angl=Vec_begin.Angle(Vec_end);

            //точность рисования кривой
            //из скольки сегментов будет составления кривая
            //если сегментов слишком мало, то возможно наложение деталей
            int PointNum=int(PI*sqrt(Radius))*100;

            if(PointNum<=2)
            {
                Line(newBegin+CnclPoint(-left,-bottom),newEnd+CnclPoint(-left,-bottom));
            }
            else
            {
                double step=angl/double(PointNum-1);
                MoveTo(newBegin+CnclPoint(-left,-bottom));
                if(z<0)
                {
                    for(double i=angl_begin;i>angl_begin-angl;i-=step)
                        LineTo(CnclPoint(cos(i)*Radius+Center.x-left,sin(i)*Radius+Center.y-bottom));
                }
                else
                {
                    for(double i=angl_begin;i<angl_begin+angl;i+=step)
                        LineTo(CnclPoint(cos(i)*Radius+Center.x-left,sin(i)*Radius+Center.y-bottom));
//                        LineTo(CnclPoint(cos(angl_begin)*Radius+Center.x,sin(angl_begin)*Radius+Center.y));
                }
                LineTo(newEnd+CnclPoint(-left,-bottom));
            }
        }
    }
}
*/
/*
//рисование точки в битовом массиве
void CnclRastr::SetPixel(CnclPoint Point)
{
//    if((Point.x+Moving)<0 || Point.y<0 || unsigned long(Point.x+Moving)>(width-1) || unsigned long(Point.y)>(height-1))
    if((Point.x+Moving)<0 || Point.y<0 || (unsigned long)(Point.x+Moving)>(width-1) || (unsigned long)(Point.y)>(height-1))
        return;
    unsigned long x=(unsigned long)(Point.x+Moving)/BIT_LEN;
//    unsigned long y=(height-1)-unsigned long(Point.y);
    unsigned long y=(unsigned long)(Point.y);
    Bit=(SBit*)&Rastr[y][x];
    switch((unsigned long)(Point.x+Moving)-x*BIT_LEN)
    {
        case 0:  Bit->a0=1;  break;
        case 1:  Bit->a1=1;  break;
        case 2:  Bit->a2=1;  break;
        case 3:  Bit->a3=1;  break;
        case 4:  Bit->a4=1;  break;
        case 5:  Bit->a5=1;  break;
        case 6:  Bit->a6=1;  break;
        case 7:  Bit->a7=1;  break;
        case 8:  Bit->a8=1;  break;
        case 9:  Bit->a9=1;  break;
        case 10: Bit->a10=1; break;
        case 11: Bit->a11=1; break;
        case 12: Bit->a12=1; break;
        case 13: Bit->a13=1; break;
        case 14: Bit->a14=1; break;
        case 15: Bit->a15=1; break;
        case 16: Bit->a16=1; break;
        case 17: Bit->a17=1; break;
        case 18: Bit->a18=1; break;
        case 19: Bit->a19=1; break;
        case 20: Bit->a20=1; break;
        case 21: Bit->a21=1; break;
        case 22: Bit->a22=1; break;
        case 23: Bit->a23=1; break;
        case 24: Bit->a24=1; break;
        case 25: Bit->a25=1; break;
        case 26: Bit->a26=1; break;
        case 27: Bit->a27=1; break;
        case 28: Bit->a28=1; break;
        case 29: Bit->a29=1; break;
        case 30: Bit->a30=1; break;
        case 31: Bit->a31=1; break;
    }
}
*/
/*
//рисование линии в битовом массиве
//от текущей точки до Point
void CnclRastr::LineTo(CnclPoint Point)
{
    Point.x=long(Point.x);
    Point.y=long(Point.y);
    long x_size=long(Point.x)-LastPixelCoord.x;
    long y_size=long(Point.y)-LastPixelCoord.y;
    if(ABS(x_size)>=ABS(y_size))
    {
        if(x_size==0)
        {
            SetPixel(CnclPoint(Point.x,Point.y));
            return;
        }
        if(x_size>0)
        {
            for(long i=0;i<=x_size;i++)
                SetPixel(CnclPoint(i+LastPixelCoord.x,i*y_size/x_size+LastPixelCoord.y));
        }
        else
        {
            for(long i=x_size;i<=0;i++)
                SetPixel(CnclPoint(i+LastPixelCoord.x,i*y_size/x_size+LastPixelCoord.y));
        }
    }
    else
    {
        if(y_size>0)
        {
            for(long i=0;i<=y_size;i++)
                SetPixel(CnclPoint(i*x_size/y_size+LastPixelCoord.x,i+LastPixelCoord.y));
        }
        else
        {
            for(long i=y_size;i<=0;i++)
                SetPixel(CnclPoint(i*x_size/y_size+LastPixelCoord.x,i+LastPixelCoord.y));
        }
    }
//    LastPixelCoord=Point
//    LastLinePixel.x=long(Point.x);
//    LastLinePixel.y=long(Point.y);
}
*/

//смещение текущей точки рисования
void CnclRastr::MoveTo(const CnclPoint &Point)
{
    LastPixelCoord=Point;
    LastPointCoord=Point;
    //    LastPixelCoord.y=long(Point.y);
}

/*
//рисование линии в битовом массиве
//от точки Begin до точки End
void CnclRastr::Line(CnclPoint Begin,CnclPoint End)
{
    MoveTo(Begin);
    LineTo(End);
}
*/

/*
//вывод кривой в битовый массив
void CnclRastr::PaintSpline(CnclSpline *Spline)
{
    CnclSpline::SnclSegment *Current=Spline->First;
    while(Current)
    {
        PaintSegment(Current);
        Current=Current->Next;
    }
}
*/

/*
//вывод детли в битовый массив
void CnclRastr::PaintDetail(CnclDetail *Detail, BOOL NeedFill, BOOL Inverse)
{
    CnclDetail::SnclSpline *Current=Detail->First;
    while(Current)
    {
        PaintSpline(Current);
        Current=Current->Next;
    }
    if(NeedFill)
        Fill(Detail);
    if(Inverse)
        Invert(Detail);
}
*/

/*
//возвращает TRUE если пиксел с координатами x,y заполнен
//FALSE - в противном случае
BOOL CnclRastr::IsPixel(unsigned long PointX,unsigned long PointY)
{
    PointX+=Moving;
    if(PointX<0 || PointY<0 || PointX>(width-1) || PointY>(height-1))
        return FALSE;
    unsigned long x=(unsigned long)(PointX)/BIT_LEN;
    Bit=(SBit*)&Rastr[(unsigned long)(PointY)][x];
    switch((unsigned long)(PointX)-x*BIT_LEN)
    {
        case 0:  return Bit->a0;
        case 1:  return Bit->a1;
        case 2:  return Bit->a2;
        case 3:  return Bit->a3;
        case 4:  return Bit->a4;
        case 5:  return Bit->a5;
        case 6:  return Bit->a6;
        case 7:  return Bit->a7;
        case 8:  return Bit->a8;
        case 9:  return Bit->a9;
        case 10: return Bit->a10;
        case 11: return Bit->a11;
        case 12: return Bit->a12;
        case 13: return Bit->a13;
        case 14: return Bit->a14;
        case 15: return Bit->a15;
        case 16: return Bit->a16;
        case 17: return Bit->a17;
        case 18: return Bit->a18;
        case 19: return Bit->a19;
        case 20: return Bit->a20;
        case 21: return Bit->a21;
        case 22: return Bit->a22;
        case 23: return Bit->a23;
        case 24: return Bit->a24;
        case 25: return Bit->a25;
        case 26: return Bit->a26;
        case 27: return Bit->a27;
        case 28: return Bit->a28;
        case 29: return Bit->a29;
        case 30: return Bit->a30;
        case 31: return Bit->a31;
    }
    return FALSE;
}
*/

/*
//закрасить деталь
void CnclRastr::Fill(CnclDetail *Detail)
{
    long First_x;
    for(unsigned long y=0;y<height;y++)
    {
        First_x=-1;
        for(unsigned long x=0;x<width;x++)
        {
            if(IsPixel(x,y))
            {
                if(First_x<0)
                {
                    First_x=x;
                }
                else
                {
                    if(x-First_x>1)
                    {
                        CnclPoint RealPoint(double(First_x+x)/2+left,y+bottom);
                        if(Detail->IsInDetail(Detail->DrawMatrix->Inverse()*RealPoint))
                            Line(CnclPoint(First_x,y),CnclPoint(x,y));
                    }
                    First_x=x;
                }
            }
        }
    }
}
*/

#define MOVE_R_1    0x80000000
#define MOVE_R_2    0xC0000000
#define MOVE_R_3    0xE0000000
#define MOVE_R_4    0xF0000000
#define MOVE_R_5    0xF8000000
#define MOVE_R_6    0xFC000000
#define MOVE_R_7    0xFE000000
#define MOVE_R_8    0xFF000000
#define MOVE_R_9    0xFF800000
#define MOVE_R_10    0xFFC00000
#define MOVE_R_11    0xFFE00000
#define MOVE_R_12    0xFFF00000
#define MOVE_R_13    0xFFF80000
#define MOVE_R_14    0xFFFC0000
#define MOVE_R_15    0xFFFE0000
#define MOVE_R_16    0xFFFF0000
#define MOVE_R_17    0xFFFF8000
#define MOVE_R_18    0xFFFFC000
#define MOVE_R_19    0xFFFFE000
#define MOVE_R_20    0xFFFFF000
#define MOVE_R_21    0xFFFFF800
#define MOVE_R_22    0xFFFFFC00
#define MOVE_R_23    0xFFFFFE00
#define MOVE_R_24    0xFFFFFF00
#define MOVE_R_25    0xFFFFFF80
#define MOVE_R_26    0xFFFFFFC0
#define MOVE_R_27    0xFFFFFFE0
#define MOVE_R_28    0xFFFFFFF0
#define MOVE_R_29    0xFFFFFFF8
#define MOVE_R_30    0xFFFFFFFC
#define MOVE_R_31    0xFFFFFFFE

#define MOVE_L_1    0x00000001
#define MOVE_L_2    0x00000003
#define MOVE_L_3    0x00000007
#define MOVE_L_4    0x0000000F
#define MOVE_L_5    0x0000001F
#define MOVE_L_6    0x0000003F
#define MOVE_L_7    0x0000007F
#define MOVE_L_8    0x000000FF
#define MOVE_L_9    0x000001FF
#define MOVE_L_10    0x000003FF
#define MOVE_L_11    0x000007FF
#define MOVE_L_12    0x00000FFF
#define MOVE_L_13    0x00001FFF
#define MOVE_L_14    0x00003FFF
#define MOVE_L_15    0x00007FFF
#define MOVE_L_16    0x0000FFFF
#define MOVE_L_17    0x0001FFFF
#define MOVE_L_18    0x0003FFFF
#define MOVE_L_19    0x0007FFFF
#define MOVE_L_20    0x000FFFFF
#define MOVE_L_21    0x001FFFFF
#define MOVE_L_22    0x003FFFFF
#define MOVE_L_23    0x007FFFFF
#define MOVE_L_24    0x00FFFFFF
#define MOVE_L_25    0x01FFFFFF
#define MOVE_L_26    0x03FFFFFF
#define MOVE_L_27    0x07FFFFFF
#define MOVE_L_28    0x0FFFFFFF
#define MOVE_L_29    0x1FFFFFFF
#define MOVE_L_30    0x3FFFFFFF
#define MOVE_L_31    0x7FFFFFFF
#define MOVE_L_32    0xFFFFFFFF

//сдвиг растра вправо на count пикселей
//если count > 0, то сдвиг вправо
//иначе влево
void CnclRastr::Move(long count)
{
    if(Moving+count>31 || Moving+count<0 || count==0) return;
    Moving+=count;
    if(count>0)
    {
        for(unsigned long y=0;y<height;y++)
            for(unsigned long x=width_bite;x>0;x--)
            {
                if(x>1)
                {
                    switch(count)
                    {
                    case 1:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<1)|((Rastr[y][x-2]>>31)&MOVE_L_1);
                        break;
                    case 2:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<2)|((Rastr[y][x-2]>>30)&MOVE_L_2);
                        break;
                    case 3:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<3)|((Rastr[y][x-2]>>28)&MOVE_L_3);
                        break;
                    case 4:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<4)|((Rastr[y][x-2]>>28)&MOVE_L_4);
                        break;
                    case 5:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<5)|((Rastr[y][x-2]>>27)&MOVE_L_5);
                        break;
                    case 6:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<6)|((Rastr[y][x-2]>>26)&MOVE_L_6);
                        break;
                    case 7:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<7)|((Rastr[y][x-2]>>25)&MOVE_L_7);
                        break;
                    case 8:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<8)|((Rastr[y][x-2]>>24)&MOVE_L_8);
                        break;
                    case 9:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<9)|((Rastr[y][x-2]>>23)&MOVE_L_9);
                        break;
                    case 10:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<10)|((Rastr[y][x-2]>>22)&MOVE_L_10);
                        break;
                    case 11:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<11)|((Rastr[y][x-2]>>21)&MOVE_L_11);
                        break;
                    case 12:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<12)|((Rastr[y][x-2]>>20)&MOVE_L_12);
                        break;
                    case 13:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<13)|((Rastr[y][x-2]>>19)&MOVE_L_13);
                        break;
                    case 14:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<14)|((Rastr[y][x-2]>>18)&MOVE_L_14);
                        break;
                    case 15:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<15)|((Rastr[y][x-2]>>17)&MOVE_L_15);
                        break;
                    case 16:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<16)|((Rastr[y][x-2]>>16)&MOVE_L_16);
                        break;
                    case 17:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<17)|((Rastr[y][x-2]>>15)&MOVE_L_17);
                        break;
                    case 18:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<18)|((Rastr[y][x-2]>>14)&MOVE_L_18);
                        break;
                    case 19:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<19)|((Rastr[y][x-2]>>13)&MOVE_L_19);
                        break;
                    case 20:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<20)|((Rastr[y][x-2]>>12)&MOVE_L_20);
                        break;
                    case 21:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<21)|((Rastr[y][x-2]>>11)&MOVE_L_21);
                        break;
                    case 22:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<22)|((Rastr[y][x-2]>>10)&MOVE_L_22);
                        break;
                    case 23:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<23)|((Rastr[y][x-2]>>9)&MOVE_L_23);
                        break;
                    case 24:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<24)|((Rastr[y][x-2]>>8)&MOVE_L_24);
                        break;
                    case 25:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<25)|((Rastr[y][x-2]>>7)&MOVE_L_25);
                        break;
                    case 26:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<26)|((Rastr[y][x-2]>>6)&MOVE_L_26);
                        break;
                    case 27:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<27)|((Rastr[y][x-2]>>5)&MOVE_L_27);
                        break;
                    case 28:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<28)|((Rastr[y][x-2]>>4)&MOVE_L_28);
                        break;
                    case 29:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<29)|((Rastr[y][x-2]>>3)&MOVE_L_29);
                        break;
                    case 30:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<30)|((Rastr[y][x-2]>>2)&MOVE_L_30);
                        break;
                    case 31:
                        Rastr[y][x-1]=(Rastr[y][x-1]<<31)|((Rastr[y][x-2]>>1)&MOVE_L_31);
                        break;
                    }
                }
                else
                {
                    Rastr[y][x-1]=Rastr[y][x-1]<<count;
                }
            }
    }
    else
    {
        count=-count;
        for(unsigned long y=0;y<height;y++)
            for(unsigned long x=0;x<width_bite;x++)
            {
                if(x<(width_bite-1))
                {
                    switch(count)
                    {
                    case 1:
                        Rastr[y][x]=((Rastr[y][x]>>1)&MOVE_L_31)|(Rastr[y][x+1]<<31);
                        break;
                    case 2:
                        Rastr[y][x]=((Rastr[y][x]>>2)&MOVE_L_30)|(Rastr[y][x+1]<<30);
                        break;
                    case 3:
                        Rastr[y][x]=((Rastr[y][x]>>3)&MOVE_L_29)|(Rastr[y][x+1]<<29);
                        break;
                    case 4:
                        Rastr[y][x]=((Rastr[y][x]>>4)&MOVE_L_28)|(Rastr[y][x+1]<<28);
                        break;
                    case 5:
                        Rastr[y][x]=((Rastr[y][x]>>5)&MOVE_L_27)|(Rastr[y][x+1]<<27);
                        break;
                    case 6:
                        Rastr[y][x]=((Rastr[y][x]>>6)&MOVE_L_26)|(Rastr[y][x+1]<<26);
                        break;
                    case 7:
                        Rastr[y][x]=((Rastr[y][x]>>7)&MOVE_L_25)|(Rastr[y][x+1]<<25);
                        break;
                    case 8:
                        Rastr[y][x]=((Rastr[y][x]>>8)&MOVE_L_24)|(Rastr[y][x+1]<<24);
                        break;
                    case 9:
                        Rastr[y][x]=((Rastr[y][x]>>9)&MOVE_L_23)|(Rastr[y][x+1]<<23);
                        break;
                    case 10:
                        Rastr[y][x]=((Rastr[y][x]>>10)&MOVE_L_22)|(Rastr[y][x+1]<<22);
                        break;
                    case 11:
                        Rastr[y][x]=((Rastr[y][x]>>11)&MOVE_L_21)|(Rastr[y][x+1]<<21);
                        break;
                    case 12:
                        Rastr[y][x]=((Rastr[y][x]>>12)&MOVE_L_20)|(Rastr[y][x+1]<<20);
                        break;
                    case 13:
                        Rastr[y][x]=((Rastr[y][x]>>13)&MOVE_L_19)|(Rastr[y][x+1]<<19);
                        break;
                    case 14:
                        Rastr[y][x]=((Rastr[y][x]>>14)&MOVE_L_18)|(Rastr[y][x+1]<<18);
                        break;
                    case 15:
                        Rastr[y][x]=((Rastr[y][x]>>15)&MOVE_L_17)|(Rastr[y][x+1]<<17);
                        break;
                    case 16:
                        Rastr[y][x]=((Rastr[y][x]>>16)&MOVE_L_16)|(Rastr[y][x+1]<<16);
                        break;
                    case 17:
                        Rastr[y][x]=((Rastr[y][x]>>17)&MOVE_L_15)|(Rastr[y][x+1]<<15);
                        break;
                    case 18:
                        Rastr[y][x]=((Rastr[y][x]>>18)&MOVE_L_14)|(Rastr[y][x+1]<<14);
                        break;
                    case 19:
                        Rastr[y][x]=((Rastr[y][x]>>19)&MOVE_L_13)|(Rastr[y][x+1]<<13);
                        break;
                    case 20:
                        Rastr[y][x]=((Rastr[y][x]>>20)&MOVE_L_12)|(Rastr[y][x+1]<<12);
                        break;
                    case 21:
                        Rastr[y][x]=((Rastr[y][x]>>21)&MOVE_L_11)|(Rastr[y][x+1]<<11);
                        break;
                    case 22:
                        Rastr[y][x]=((Rastr[y][x]>>22)&MOVE_L_10)|(Rastr[y][x+1]<<10);
                        break;
                    case 23:
                        Rastr[y][x]=((Rastr[y][x]>>23)&MOVE_L_9)|(Rastr[y][x+1]<<9);
                        break;
                    case 24:
                        Rastr[y][x]=((Rastr[y][x]>>24)&MOVE_L_8)|(Rastr[y][x+1]<<8);
                        break;
                    case 25:
                        Rastr[y][x]=((Rastr[y][x]>>25)&MOVE_L_7)|(Rastr[y][x+1]<<7);
                        break;
                    case 26:
                        Rastr[y][x]=((Rastr[y][x]>>26)&MOVE_L_6)|(Rastr[y][x+1]<<6);
                        break;
                    case 27:
                        Rastr[y][x]=((Rastr[y][x]>>27)&MOVE_L_5)|(Rastr[y][x+1]<<5);
                        break;
                    case 28:
                        Rastr[y][x]=((Rastr[y][x]>>28)&MOVE_L_4)|(Rastr[y][x+1]<<4);
                        break;
                    case 29:
                        Rastr[y][x]=((Rastr[y][x]>>29)&MOVE_L_3)|(Rastr[y][x+1]<<3);
                        break;
                    case 30:
                        Rastr[y][x]=((Rastr[y][x]>>30)&MOVE_L_2)|(Rastr[y][x+1]<<2);
                        break;
                    case 31:
                        Rastr[y][x]=((Rastr[y][x]>>31)&MOVE_L_1)|(Rastr[y][x+1]<<1);
                        break;
                    }
                }
                else
                {
                    switch(count)
                    {
                    case 1:
                        Rastr[y][x]=(Rastr[y][x]>>1)&MOVE_L_31;
                        break;
                    case 2:
                        Rastr[y][x]=(Rastr[y][x]>>2)&MOVE_L_30;
                        break;
                    case 3:
                        Rastr[y][x]=(Rastr[y][x]>>3)&MOVE_L_29;
                        break;
                    case 4:
                        Rastr[y][x]=(Rastr[y][x]>>4)&MOVE_L_28;
                        break;
                    case 5:
                        Rastr[y][x]=(Rastr[y][x]>>5)&MOVE_L_27;
                        break;
                    case 6:
                        Rastr[y][x]=(Rastr[y][x]>>6)&MOVE_L_26;
                        break;
                    case 7:
                        Rastr[y][x]=(Rastr[y][x]>>7)&MOVE_L_25;
                        break;
                    case 8:
                        Rastr[y][x]=(Rastr[y][x]>>8)&MOVE_L_24;
                        break;
                    case 9:
                        Rastr[y][x]=(Rastr[y][x]>>9)&MOVE_L_23;
                        break;
                    case 10:
                        Rastr[y][x]=(Rastr[y][x]>>10)&MOVE_L_22;
                        break;
                    case 11:
                        Rastr[y][x]=(Rastr[y][x]>>11)&MOVE_L_21;
                        break;
                    case 12:
                        Rastr[y][x]=(Rastr[y][x]>>12)&MOVE_L_20;
                        break;
                    case 13:
                        Rastr[y][x]=(Rastr[y][x]>>13)&MOVE_L_19;
                        break;
                    case 14:
                        Rastr[y][x]=(Rastr[y][x]>>14)&MOVE_L_18;
                        break;
                    case 15:
                        Rastr[y][x]=(Rastr[y][x]>>15)&MOVE_L_17;
                        break;
                    case 16:
                        Rastr[y][x]=(Rastr[y][x]>>16)&MOVE_L_16;
                        break;
                    case 17:
                        Rastr[y][x]=(Rastr[y][x]>>17)&MOVE_L_15;
                        break;
                    case 18:
                        Rastr[y][x]=(Rastr[y][x]>>18)&MOVE_L_14;
                        break;
                    case 19:
                        Rastr[y][x]=(Rastr[y][x]>>19)&MOVE_L_13;
                        break;
                    case 20:
                        Rastr[y][x]=(Rastr[y][x]>>20)&MOVE_L_12;
                        break;
                    case 21:
                        Rastr[y][x]=(Rastr[y][x]>>21)&MOVE_L_11;
                        break;
                    case 22:
                        Rastr[y][x]=(Rastr[y][x]>>22)&MOVE_L_10;
                        break;
                    case 23:
                        Rastr[y][x]=(Rastr[y][x]>>23)&MOVE_L_9;
                        break;
                    case 24:
                        Rastr[y][x]=(Rastr[y][x]>>24)&MOVE_L_8;
                        break;
                    case 25:
                        Rastr[y][x]=(Rastr[y][x]>>25)&MOVE_L_7;
                        break;
                    case 26:
                        Rastr[y][x]=(Rastr[y][x]>>26)&MOVE_L_6;
                        break;
                    case 27:
                        Rastr[y][x]=(Rastr[y][x]>>27)&MOVE_L_5;
                        break;
                    case 28:
                        Rastr[y][x]=(Rastr[y][x]>>28)&MOVE_L_4;
                        break;
                    case 29:
                        Rastr[y][x]=(Rastr[y][x]>>29)&MOVE_L_3;
                        break;
                    case 30:
                        Rastr[y][x]=(Rastr[y][x]>>30)&MOVE_L_2;
                        break;
                    case 31:
                        Rastr[y][x]=(Rastr[y][x]>>31)&MOVE_L_1;
                        break;
                    }
                }
            }
    }
}

void CnclRastr::new_Move(long count)
{
    if(Moving+count>31 || Moving+count<0 || count==0) return;
    if(count>0)
    {
        //        for(unsigned long y=0;y<height;y++)
        for(unsigned long x=width_bite;x>0;x--)
        {
            if(x>1)
            {
                switch(count)
                {
                case 1:
                    SinglLine[x-1]=(SinglLine[x-1]<<1)|((SinglLine[x-2]>>31)&MOVE_L_1);
                    break;
                case 2:
                    SinglLine[x-1]=(SinglLine[x-1]<<2)|((SinglLine[x-2]>>30)&MOVE_L_2);
                    break;
                case 3:
                    SinglLine[x-1]=(SinglLine[x-1]<<3)|((SinglLine[x-2]>>28)&MOVE_L_3);
                    break;
                case 4:
                    SinglLine[x-1]=(SinglLine[x-1]<<4)|((SinglLine[x-2]>>28)&MOVE_L_4);
                    break;
                case 5:
                    SinglLine[x-1]=(SinglLine[x-1]<<5)|((SinglLine[x-2]>>27)&MOVE_L_5);
                    break;
                case 6:
                    SinglLine[x-1]=(SinglLine[x-1]<<6)|((SinglLine[x-2]>>26)&MOVE_L_6);
                    break;
                case 7:
                    SinglLine[x-1]=(SinglLine[x-1]<<7)|((SinglLine[x-2]>>25)&MOVE_L_7);
                    break;
                case 8:
                    SinglLine[x-1]=(SinglLine[x-1]<<8)|((SinglLine[x-2]>>24)&MOVE_L_8);
                    break;
                case 9:
                    SinglLine[x-1]=(SinglLine[x-1]<<9)|((SinglLine[x-2]>>23)&MOVE_L_9);
                    break;
                case 10:
                    SinglLine[x-1]=(SinglLine[x-1]<<10)|((SinglLine[x-2]>>22)&MOVE_L_10);
                    break;
                case 11:
                    SinglLine[x-1]=(SinglLine[x-1]<<11)|((SinglLine[x-2]>>21)&MOVE_L_11);
                    break;
                case 12:
                    SinglLine[x-1]=(SinglLine[x-1]<<12)|((SinglLine[x-2]>>20)&MOVE_L_12);
                    break;
                case 13:
                    SinglLine[x-1]=(SinglLine[x-1]<<13)|((SinglLine[x-2]>>19)&MOVE_L_13);
                    break;
                case 14:
                    SinglLine[x-1]=(SinglLine[x-1]<<14)|((SinglLine[x-2]>>18)&MOVE_L_14);
                    break;
                case 15:
                    SinglLine[x-1]=(SinglLine[x-1]<<15)|((SinglLine[x-2]>>17)&MOVE_L_15);
                    break;
                case 16:
                    SinglLine[x-1]=(SinglLine[x-1]<<16)|((SinglLine[x-2]>>16)&MOVE_L_16);
                    break;
                case 17:
                    SinglLine[x-1]=(SinglLine[x-1]<<17)|((SinglLine[x-2]>>15)&MOVE_L_17);
                    break;
                case 18:
                    SinglLine[x-1]=(SinglLine[x-1]<<18)|((SinglLine[x-2]>>14)&MOVE_L_18);
                    break;
                case 19:
                    SinglLine[x-1]=(SinglLine[x-1]<<19)|((SinglLine[x-2]>>13)&MOVE_L_19);
                    break;
                case 20:
                    SinglLine[x-1]=(SinglLine[x-1]<<20)|((SinglLine[x-2]>>12)&MOVE_L_20);
                    break;
                case 21:
                    SinglLine[x-1]=(SinglLine[x-1]<<21)|((SinglLine[x-2]>>11)&MOVE_L_21);
                    break;
                case 22:
                    SinglLine[x-1]=(SinglLine[x-1]<<22)|((SinglLine[x-2]>>10)&MOVE_L_22);
                    break;
                case 23:
                    SinglLine[x-1]=(SinglLine[x-1]<<23)|((SinglLine[x-2]>>9)&MOVE_L_23);
                    break;
                case 24:
                    SinglLine[x-1]=(SinglLine[x-1]<<24)|((SinglLine[x-2]>>8)&MOVE_L_24);
                    break;
                case 25:
                    SinglLine[x-1]=(SinglLine[x-1]<<25)|((SinglLine[x-2]>>7)&MOVE_L_25);
                    break;
                case 26:
                    SinglLine[x-1]=(SinglLine[x-1]<<26)|((SinglLine[x-2]>>6)&MOVE_L_26);
                    break;
                case 27:
                    SinglLine[x-1]=(SinglLine[x-1]<<27)|((SinglLine[x-2]>>5)&MOVE_L_27);
                    break;
                case 28:
                    SinglLine[x-1]=(SinglLine[x-1]<<28)|((SinglLine[x-2]>>4)&MOVE_L_28);
                    break;
                case 29:
                    SinglLine[x-1]=(SinglLine[x-1]<<29)|((SinglLine[x-2]>>3)&MOVE_L_29);
                    break;
                case 30:
                    SinglLine[x-1]=(SinglLine[x-1]<<30)|((SinglLine[x-2]>>2)&MOVE_L_30);
                    break;
                case 31:
                    SinglLine[x-1]=(SinglLine[x-1]<<31)|((SinglLine[x-2]>>1)&MOVE_L_31);
                    break;
                }
            }
            else
            {
                SinglLine[x-1]=SinglLine[x-1]<<count;
            }
        }
    }
    else
    {
        count=-count;
        for(unsigned long y=0;y<height;y++)
            for(unsigned long x=0;x<width_bite;x++)
            {
                if(x<(width_bite-1))
                {
                    switch(count)
                    {
                    case 1:
                        SinglLine[x]=((SinglLine[x]>>1)&MOVE_L_31)|(SinglLine[x+1]<<31);
                        break;
                    case 2:
                        SinglLine[x]=((SinglLine[x]>>2)&MOVE_L_30)|(SinglLine[x+1]<<30);
                        break;
                    case 3:
                        SinglLine[x]=((SinglLine[x]>>3)&MOVE_L_29)|(SinglLine[x+1]<<29);
                        break;
                    case 4:
                        SinglLine[x]=((SinglLine[x]>>4)&MOVE_L_28)|(SinglLine[x+1]<<28);
                        break;
                    case 5:
                        SinglLine[x]=((SinglLine[x]>>5)&MOVE_L_27)|(SinglLine[x+1]<<27);
                        break;
                    case 6:
                        SinglLine[x]=((SinglLine[x]>>6)&MOVE_L_26)|(SinglLine[x+1]<<26);
                        break;
                    case 7:
                        SinglLine[x]=((SinglLine[x]>>7)&MOVE_L_25)|(SinglLine[x+1]<<25);
                        break;
                    case 8:
                        SinglLine[x]=((SinglLine[x]>>8)&MOVE_L_24)|(SinglLine[x+1]<<24);
                        break;
                    case 9:
                        SinglLine[x]=((SinglLine[x]>>9)&MOVE_L_23)|(SinglLine[x+1]<<23);
                        break;
                    case 10:
                        SinglLine[x]=((SinglLine[x]>>10)&MOVE_L_22)|(SinglLine[x+1]<<22);
                        break;
                    case 11:
                        SinglLine[x]=((SinglLine[x]>>11)&MOVE_L_21)|(SinglLine[x+1]<<21);
                        break;
                    case 12:
                        SinglLine[x]=((SinglLine[x]>>12)&MOVE_L_20)|(SinglLine[x+1]<<20);
                        break;
                    case 13:
                        SinglLine[x]=((SinglLine[x]>>13)&MOVE_L_19)|(SinglLine[x+1]<<19);
                        break;
                    case 14:
                        SinglLine[x]=((SinglLine[x]>>14)&MOVE_L_18)|(SinglLine[x+1]<<18);
                        break;
                    case 15:
                        SinglLine[x]=((SinglLine[x]>>15)&MOVE_L_17)|(SinglLine[x+1]<<17);
                        break;
                    case 16:
                        SinglLine[x]=((SinglLine[x]>>16)&MOVE_L_16)|(SinglLine[x+1]<<16);
                        break;
                    case 17:
                        SinglLine[x]=((SinglLine[x]>>17)&MOVE_L_15)|(SinglLine[x+1]<<15);
                        break;
                    case 18:
                        SinglLine[x]=((SinglLine[x]>>18)&MOVE_L_14)|(SinglLine[x+1]<<14);
                        break;
                    case 19:
                        SinglLine[x]=((SinglLine[x]>>19)&MOVE_L_13)|(SinglLine[x+1]<<13);
                        break;
                    case 20:
                        SinglLine[x]=((SinglLine[x]>>20)&MOVE_L_12)|(SinglLine[x+1]<<12);
                        break;
                    case 21:
                        SinglLine[x]=((SinglLine[x]>>21)&MOVE_L_11)|(SinglLine[x+1]<<11);
                        break;
                    case 22:
                        SinglLine[x]=((SinglLine[x]>>22)&MOVE_L_10)|(SinglLine[x+1]<<10);
                        break;
                    case 23:
                        SinglLine[x]=((SinglLine[x]>>23)&MOVE_L_9)|(SinglLine[x+1]<<9);
                        break;
                    case 24:
                        SinglLine[x]=((SinglLine[x]>>24)&MOVE_L_8)|(SinglLine[x+1]<<8);
                        break;
                    case 25:
                        SinglLine[x]=((SinglLine[x]>>25)&MOVE_L_7)|(SinglLine[x+1]<<7);
                        break;
                    case 26:
                        SinglLine[x]=((SinglLine[x]>>26)&MOVE_L_6)|(SinglLine[x+1]<<6);
                        break;
                    case 27:
                        SinglLine[x]=((SinglLine[x]>>27)&MOVE_L_5)|(SinglLine[x+1]<<5);
                        break;
                    case 28:
                        SinglLine[x]=((SinglLine[x]>>28)&MOVE_L_4)|(SinglLine[x+1]<<4);
                        break;
                    case 29:
                        SinglLine[x]=((SinglLine[x]>>29)&MOVE_L_3)|(SinglLine[x+1]<<3);
                        break;
                    case 30:
                        SinglLine[x]=((SinglLine[x]>>30)&MOVE_L_2)|(SinglLine[x+1]<<2);
                        break;
                    case 31:
                        SinglLine[x]=((SinglLine[x]>>31)&MOVE_L_1)|(SinglLine[x+1]<<1);
                        break;
                    }
                }
                else
                {
                    switch(count)
                    {
                    case 1:
                        SinglLine[x]=(SinglLine[x]>>1)&MOVE_L_31;
                        break;
                    case 2:
                        SinglLine[x]=(SinglLine[x]>>2)&MOVE_L_30;
                        break;
                    case 3:
                        SinglLine[x]=(SinglLine[x]>>3)&MOVE_L_29;
                        break;
                    case 4:
                        SinglLine[x]=(SinglLine[x]>>4)&MOVE_L_28;
                        break;
                    case 5:
                        SinglLine[x]=(SinglLine[x]>>5)&MOVE_L_27;
                        break;
                    case 6:
                        SinglLine[x]=(SinglLine[x]>>6)&MOVE_L_26;
                        break;
                    case 7:
                        SinglLine[x]=(SinglLine[x]>>7)&MOVE_L_25;
                        break;
                    case 8:
                        SinglLine[x]=(SinglLine[x]>>8)&MOVE_L_24;
                        break;
                    case 9:
                        SinglLine[x]=(SinglLine[x]>>9)&MOVE_L_23;
                        break;
                    case 10:
                        SinglLine[x]=(SinglLine[x]>>10)&MOVE_L_22;
                        break;
                    case 11:
                        SinglLine[x]=(SinglLine[x]>>11)&MOVE_L_21;
                        break;
                    case 12:
                        SinglLine[x]=(SinglLine[x]>>12)&MOVE_L_20;
                        break;
                    case 13:
                        SinglLine[x]=(SinglLine[x]>>13)&MOVE_L_19;
                        break;
                    case 14:
                        SinglLine[x]=(SinglLine[x]>>14)&MOVE_L_18;
                        break;
                    case 15:
                        SinglLine[x]=(SinglLine[x]>>15)&MOVE_L_17;
                        break;
                    case 16:
                        SinglLine[x]=(SinglLine[x]>>16)&MOVE_L_16;
                        break;
                    case 17:
                        SinglLine[x]=(SinglLine[x]>>17)&MOVE_L_15;
                        break;
                    case 18:
                        SinglLine[x]=(SinglLine[x]>>18)&MOVE_L_14;
                        break;
                    case 19:
                        SinglLine[x]=(SinglLine[x]>>19)&MOVE_L_13;
                        break;
                    case 20:
                        SinglLine[x]=(SinglLine[x]>>20)&MOVE_L_12;
                        break;
                    case 21:
                        SinglLine[x]=(SinglLine[x]>>21)&MOVE_L_11;
                        break;
                    case 22:
                        SinglLine[x]=(SinglLine[x]>>22)&MOVE_L_10;
                        break;
                    case 23:
                        SinglLine[x]=(SinglLine[x]>>23)&MOVE_L_9;
                        break;
                    case 24:
                        SinglLine[x]=(SinglLine[x]>>24)&MOVE_L_8;
                        break;
                    case 25:
                        SinglLine[x]=(SinglLine[x]>>25)&MOVE_L_7;
                        break;
                    case 26:
                        SinglLine[x]=(SinglLine[x]>>26)&MOVE_L_6;
                        break;
                    case 27:
                        SinglLine[x]=(SinglLine[x]>>27)&MOVE_L_5;
                        break;
                    case 28:
                        SinglLine[x]=(SinglLine[x]>>28)&MOVE_L_4;
                        break;
                    case 29:
                        SinglLine[x]=(SinglLine[x]>>29)&MOVE_L_3;
                        break;
                    case 30:
                        SinglLine[x]=(SinglLine[x]>>30)&MOVE_L_2;
                        break;
                    case 31:
                        SinglLine[x]=(SinglLine[x]>>31)&MOVE_L_1;
                        break;
                    }
                }
            }
    }
}

//возвращает TRUE если Rastr перекрывает
//текущий растр хотя бы в одной точке
//или Rastr выступает за пределы текущего растра
//в противном случае - FALSE
//x - смещение Rastr относительно текущего растра по оси X
//y - смещение Rastr относительно текущего растра по оси Y
/*
BOOL CnclRastr::IsCross(unsigned long x_move, unsigned long y_move,CnclRastr &newRastr)
{
    if((y_move+newRastr.height)>height)
        return TRUE;
    //вычисление длины сдвига в байтах
    unsigned long x_move_bite=x_move/BIT_LEN;
    if((x_move_bite+newRastr.width_bite)>width_bite)
        return TRUE;
    //вычисление величины сдвига в битах
    unsigned long x_move_bit=x_move-x_move_bite*BIT_LEN;
    newRastr.Move(x_move_bit-newRastr.Moving);

    for(unsigned long y=0;y<newRastr.height;y++)
        for(unsigned long x=0;x<newRastr.width_bite;x++)
        {
//            if((newRastr.Rastr[y][x]&Rastr[y+y_move][x+x_move_bite])!=0)
                return TRUE;
        }

    return FALSE;
}
*/

int CnclRastr::IsCross(unsigned long x_move, unsigned long y_move,CnclRastr &newRastr)
{
    if((y_move+newRastr.height)>height)
        return -1;
    //вычисление длины сдвига в байтах
    unsigned long x_move_bite=x_move/BIT_LEN;
    if((x_move_bite+newRastr.width_bite)>width_bite)
        return -1;
    //вычисление величины сдвига в битах
    unsigned long x_move_bit=x_move-x_move_bite*BIT_LEN;
    //	int max_cross=0;
    int cross;

    for(unsigned long y=0;y<newRastr.height;y++)
    {
        memcpy(newRastr.SinglLine,newRastr.Rastr[y],(sizeof(long)*newRastr.width_bite));
        newRastr.new_Move(x_move_bit-newRastr.Moving);
        for(unsigned long x=0;x<newRastr.width_bite;x++)
        {
            if((newRastr.SinglLine[x]&Rastr[y+y_move][x+x_move_bite])!=0)
            {
                cross=0;
                do
                {
                    cross++;
                    y++;
                    if(y==newRastr.height)
                        break;
                }while((newRastr.SinglLine[x]&Rastr[y+y_move][x+x_move_bite])>0);
                return cross;
                //				if(cross>max_cross)
                //				{
                //					max_cross=cross;
                //				    if((y_move+newRastr.height+max_cross)>height)
                //						return -1;
                //				}
            }
            //			if(y==newRastr.height)
            //				break;
        }
    }
    //	return max_cross;
    /*
    for(unsigned long y=newRastr.height/2;y>=0;y--)
    {
        memcpy(newRastr.SinglLine,newRastr.Rastr[y],(sizeof(long)*newRastr.width_bite));
        newRastr.new_Move(x_move_bit-newRastr.Moving);
        for(unsigned long x=0;x<newRastr.width_bite;x++)
        {
            if((newRastr.SinglLine[x]&Rastr[y+y_move][x+x_move_bite])!=0)
                return TRUE;
        }
        if(y==0)
            break;
    }
    for(y=newRastr.height/2+1;y<newRastr.height;y++)
    {
        memcpy(newRastr.SinglLine,newRastr.Rastr[y],(sizeof(long)*newRastr.width_bite));
        newRastr.new_Move(x_move_bit-newRastr.Moving);
        for(unsigned long x=0;x<newRastr.width_bite;x++)
        {
            if((newRastr.SinglLine[x]&Rastr[y+y_move][x+x_move_bite])!=0)
                return TRUE;
        }
    }
    */
    return 0;
}

//насладывет растр Rastr на текущий растр
//если наложение невозможно, то возвращает FALSE
//иначе TRUE
BOOL CnclRastr::Add(unsigned long x_move, unsigned long y_move,CnclRastr &newRastr)
{
    if(y_move+newRastr.height>height)
        return FALSE;
    unsigned long x_move_bite=x_move/BIT_LEN;
    if((x_move_bite+newRastr.width_bite)>width_bite)
        return FALSE;
    //вычисление величины сдвига в битах
    unsigned long x_move_bit=x_move-x_move_bite*BIT_LEN;
    newRastr.Move(x_move_bit-newRastr.Moving);

    for(unsigned long y=0;y<newRastr.height;y++)
        for(unsigned long x=0;x<newRastr.width_bite;x++)
            Rastr[y+y_move][x+x_move_bite]|=newRastr.Rastr[y][x];
    return TRUE;
}

//интертировать изображение растра
void CnclRastr::Invert(const CnclDetail *Detail)
{
    for(unsigned long y=0;y<height;y++)
        for(unsigned long x=0;x<width_bite;x++)
            Rastr[y][x]=(Rastr[y][x]^0xFFFFFFFF);
    PaintDetail(Detail,FALSE,FALSE);
}


//Direction - направление рисования линии к которой принадлежит точка
//Direction=  0 - параллельно оси X
//Direction=  1 - вверх по оси Y
//Direction= -1 - вниз по оси Y
//если параметр NeedFill==FALSE, то Direction игнорируется
void CnclRastr::Fill_SetPixel(const CnclPixelCoord &Point,int Direction)
{
    if(Point.y==LastPixelCoord.y && Serial!=SERIAL_LAST)
    {
        LastPixelCoord=Point;
        return;
    }
    if(long(Point.x+Moving)<0 || long(Point.y)<0 || width==0 || height==0 || long(Point.x+Moving)>(long)(width-1) || long(Point.y)>(long)(height-1))
    {
        LastPixelCoord=Point;
        return;
    }
    long x=(Point.x+Moving)/BIT_LEN;
    long y=Point.y;
    unsigned long *Bit=&Rastr[y][x];

    if(Serial!=SERIAL_FIRST &&
            ((Direction!=0 && LastPixelCoord.y!=Point.y)||
             Serial==SERIAL_LAST))
    {

        if(last_nonzerro_dir*Direction==-1)
        {
            int s=Serial;
            Serial=SERIAL_LAST;
            Fill_SetPixel(LastPixelCoord,0);
            Serial=s;
        }
        for(long i=0;i<x;i++)
            Rastr[y][i]^=MOVE_L_32;
        switch(long(Point.x+Moving)-x*BIT_LEN)
        {
        case 0:  Rastr[y][x]^=MOVE_L_1;   break;
        case 1:  Rastr[y][x]^=MOVE_L_2;   break;
        case 2:  Rastr[y][x]^=MOVE_L_3;   break;
        case 3:  Rastr[y][x]^=MOVE_L_4;   break;
        case 4:  Rastr[y][x]^=MOVE_L_5;   break;
        case 5:  Rastr[y][x]^=MOVE_L_6;   break;
        case 6:  Rastr[y][x]^=MOVE_L_7;   break;
        case 7:  Rastr[y][x]^=MOVE_L_8;   break;
        case 8:  Rastr[y][x]^=MOVE_L_9;   break;
        case 9:  Rastr[y][x]^=MOVE_L_10;  break;
        case 10: Rastr[y][x]^=MOVE_L_11;  break;
        case 11: Rastr[y][x]^=MOVE_L_12;  break;
        case 12: Rastr[y][x]^=MOVE_L_13;  break;
        case 13: Rastr[y][x]^=MOVE_L_14;  break;
        case 14: Rastr[y][x]^=MOVE_L_15;  break;
        case 15: Rastr[y][x]^=MOVE_L_16;  break;
        case 16: Rastr[y][x]^=MOVE_L_17;  break;
        case 17: Rastr[y][x]^=MOVE_L_18;  break;
        case 18: Rastr[y][x]^=MOVE_L_19;  break;
        case 19: Rastr[y][x]^=MOVE_L_20;  break;
        case 20: Rastr[y][x]^=MOVE_L_21;  break;
        case 21: Rastr[y][x]^=MOVE_L_22;  break;
        case 22: Rastr[y][x]^=MOVE_L_23;  break;
        case 23: Rastr[y][x]^=MOVE_L_24;  break;
        case 24: Rastr[y][x]^=MOVE_L_25;  break;
        case 25: Rastr[y][x]^=MOVE_L_26;  break;
        case 26: Rastr[y][x]^=MOVE_L_27;  break;
        case 27: Rastr[y][x]^=MOVE_L_28;  break;
        case 28: Rastr[y][x]^=MOVE_L_29;  break;
        case 29: Rastr[y][x]^=MOVE_L_30;  break;
        case 30: Rastr[y][x]^=MOVE_L_31;  break;
        case 31: Rastr[y][x]^=MOVE_L_32;  break;
        }
    }
    else
    {
        switch((unsigned long)(Point.x+Moving)-x*BIT_LEN)
        {
        case 0:  Rastr[y][x]|=BIT_01;  break;
        case 1:  Rastr[y][x]|=BIT_02;  break;
        case 2:  Rastr[y][x]|=BIT_03;  break;
        case 3:  Rastr[y][x]|=BIT_04;  break;
        case 4:  Rastr[y][x]|=BIT_05;  break;
        case 5:  Rastr[y][x]|=BIT_06;  break;
        case 6:  Rastr[y][x]|=BIT_07;  break;
        case 7:  Rastr[y][x]|=BIT_08;  break;
        case 8:  Rastr[y][x]|=BIT_09;  break;
        case 9:  Rastr[y][x]|=BIT_10;  break;
        case 10:  Rastr[y][x]|=BIT_11;  break;
        case 11:  Rastr[y][x]|=BIT_12;  break;
        case 12:  Rastr[y][x]|=BIT_13;  break;
        case 13:  Rastr[y][x]|=BIT_14;  break;
        case 14:  Rastr[y][x]|=BIT_15;  break;
        case 15:  Rastr[y][x]|=BIT_16;  break;
        case 16:  Rastr[y][x]|=BIT_17;  break;
        case 17:  Rastr[y][x]|=BIT_18;  break;
        case 18:  Rastr[y][x]|=BIT_19;  break;
        case 19:  Rastr[y][x]|=BIT_20;  break;
        case 20:  Rastr[y][x]|=BIT_21;  break;
        case 21:  Rastr[y][x]|=BIT_22;  break;
        case 22:  Rastr[y][x]|=BIT_23;  break;
        case 23:  Rastr[y][x]|=BIT_24;  break;
        case 24:  Rastr[y][x]|=BIT_25;  break;
        case 25:  Rastr[y][x]|=BIT_26;  break;
        case 26:  Rastr[y][x]|=BIT_27;  break;
        case 27:  Rastr[y][x]|=BIT_28;  break;
        case 28:  Rastr[y][x]|=BIT_29;  break;
        case 29:  Rastr[y][x]|=BIT_30;  break;
        case 30:  Rastr[y][x]|=BIT_31;  break;
        case 31:  Rastr[y][x]|=BIT_32;  break;
        }
    }
    LastPixelCoord=Point;
    if(Direction!=last_pixel_dir)
        dir_change++;
    if(Direction!=0)
        last_nonzerro_dir=Direction;
    last_pixel_dir=Direction;
}

void CnclRastr::NoFill_SetPixel(const CnclPoint &Point,BOOL Move)
{
    if(long(Point.x+Moving)<0 || long(Point.y)<0 || width==0 || height==0 || long(Point.x+Moving)>(long)(width-1) || long(Point.y)>(long)(height-1))
    {
        if(Move)
            LastPointCoord=Point;
        return;
    }
    long x=long(Point.x+Moving)/BIT_LEN;
    long y=long(Point.y);
    switch((unsigned long)(Point.x+Moving)-x*BIT_LEN)
    {
    case 0:   Rastr[y][x]|=BIT_01;  break;
    case 1:   Rastr[y][x]|=BIT_02;  break;
    case 2:   Rastr[y][x]|=BIT_03;  break;
    case 3:   Rastr[y][x]|=BIT_04;  break;
    case 4:   Rastr[y][x]|=BIT_05;  break;
    case 5:   Rastr[y][x]|=BIT_06;  break;
    case 6:   Rastr[y][x]|=BIT_07;  break;
    case 7:   Rastr[y][x]|=BIT_08;  break;
    case 8:   Rastr[y][x]|=BIT_09;  break;
    case 9:   Rastr[y][x]|=BIT_10;  break;
    case 10:  Rastr[y][x]|=BIT_11;  break;
    case 11:  Rastr[y][x]|=BIT_12;  break;
    case 12:  Rastr[y][x]|=BIT_13;  break;
    case 13:  Rastr[y][x]|=BIT_14;  break;
    case 14:  Rastr[y][x]|=BIT_15;  break;
    case 15:  Rastr[y][x]|=BIT_16;  break;
    case 16:  Rastr[y][x]|=BIT_17;  break;
    case 17:  Rastr[y][x]|=BIT_18;  break;
    case 18:  Rastr[y][x]|=BIT_19;  break;
    case 19:  Rastr[y][x]|=BIT_20;  break;
    case 20:  Rastr[y][x]|=BIT_21;  break;
    case 21:  Rastr[y][x]|=BIT_22;  break;
    case 22:  Rastr[y][x]|=BIT_23;  break;
    case 23:  Rastr[y][x]|=BIT_24;  break;
    case 24:  Rastr[y][x]|=BIT_25;  break;
    case 25:  Rastr[y][x]|=BIT_26;  break;
    case 26:  Rastr[y][x]|=BIT_27;  break;
    case 27:  Rastr[y][x]|=BIT_28;  break;
    case 28:  Rastr[y][x]|=BIT_29;  break;
    case 29:  Rastr[y][x]|=BIT_30;  break;
    case 30:  Rastr[y][x]|=BIT_31;  break;
    case 31:  Rastr[y][x]|=BIT_32;  break;
    }
    if(Move)
    {
        NoFill_SetPixel(CnclPoint(Point.x+1,Point.y  ),FALSE);
        NoFill_SetPixel(CnclPoint(Point.x,  Point.y+1),FALSE);
        NoFill_SetPixel(CnclPoint(Point.x-1,Point.y  ),FALSE);
        NoFill_SetPixel(CnclPoint(Point.x,  Point.y-1),FALSE);
        LastPointCoord=Point;
    }
}

void CnclRastr::Fill_PaintSegment(CnclSegment *Segment)
{
    CnclPoint newBegin=Segment->GetNewCoord(Segment->Begin,FALSE,TransformView);
    CnclPoint newEnd=Segment->GetNewCoord(Segment->End,FALSE,TransformView);
    if(Segment->z==0)
    {
        if(Serial==SERIAL_FIRST)
            Fill_Line(CnclPixelCoord(CnclPoint(-left,-bottom)+newBegin),CnclPixelCoord(CnclPoint(-left,-bottom)+newEnd));
        else
            Fill_LineTo(CnclPixelCoord(CnclPoint(-left,-bottom)+newEnd));
    }
    else
    {
        CnclPoint Center=Segment->CenterArc(TRUE,FALSE,TransformView);
        double Radius=Segment->RadiusArc(Center,TRUE,TransformView);
        if(Radius<=0)
        {
            if(Serial==SERIAL_FIRST)
                Fill_Line(CnclPixelCoord(CnclPoint(-left,-bottom)+newBegin),CnclPixelCoord(CnclPoint(-left,-bottom)+newEnd));
            else
                Fill_LineTo(CnclPixelCoord(CnclPoint(-left,-bottom)+newEnd));
        }
        else
        {
            double angl_begin,angl;
            CnclVector Vec_begin,Vec_end;
            Vec_begin.Set(Center,newBegin);
            Vec_end.Set(Center,newEnd);
            angl_begin=Vec_begin.Angle();
            double z=Segment->GetNewZ();
            if(z<0)
                angl=Vec_end.Angle(Vec_begin);
            else
                angl=Vec_begin.Angle(Vec_end);

            //точность рисования кривой
            //из скольки сегментов будет составления кривая
            //если сегментов слишком мало, то возможно наложение деталей
            int PointNum=int(PI*sqrt(Radius));
            if(PointNum<=2)
            {
                if(Serial==SERIAL_FIRST)
                    Fill_Line(CnclPixelCoord(CnclPoint(-left,-bottom)+newBegin),CnclPixelCoord(CnclPoint(-left,-bottom)+newEnd));
                else
                    Fill_LineTo(CnclPixelCoord(CnclPoint(-left,-bottom)+newEnd));
            }
            else
            {
                double step=angl/double(PointNum-1);
                MoveTo(CnclPoint(-left,-bottom)+newBegin);
                if(z<0)
                {
                    for(double i=angl_begin;i>angl_begin-angl;i-=step)
                        Fill_LineTo(CnclPixelCoord(long(cos(i)*Radius+Center.x-left),long(sin(i)*Radius+Center.y-bottom)));
                }
                else
                {
                    for(double i=angl_begin;i<angl_begin+angl;i+=step)
                        Fill_LineTo(CnclPixelCoord(long(cos(i)*Radius+Center.x-left),long(sin(i)*Radius+Center.y-bottom)));
                }
                Fill_LineTo(CnclPixelCoord(CnclPoint(-left,-bottom)+newEnd));
            }
        }
    }
}

void CnclRastr::NoFill_PaintSegment(CnclSegment *Segment)
{
    CnclPoint newBegin=Segment->GetNewCoord(Segment->Begin,FALSE,TransformView);
    CnclPoint newEnd=Segment->GetNewCoord(Segment->End,FALSE,TransformView);
    if(Segment->z==0)
    {
        NoFill_Line(CnclPoint(-left,-bottom)+newBegin,CnclPoint(-left,-bottom)+newEnd);
    }
    else
    {
        CnclPoint Center=Segment->CenterArc(TRUE,FALSE,TransformView);
        double Radius=Segment->RadiusArc(Center,TRUE,TransformView);
        if(Radius<=0)
        {
            NoFill_Line(CnclPoint(-left,-bottom)+newBegin,CnclPoint(-left,-bottom)+newEnd);
        }
        else
        {
            double angl_begin,angl;
            CnclVector Vec_begin,Vec_end;
            Vec_begin.Set(Center,newBegin);
            Vec_end.Set(Center,newEnd);
            angl_begin=Vec_begin.Angle();
            double z=Segment->GetNewZ();
            if(z<0)
                angl=Vec_end.Angle(Vec_begin);
            else
                angl=Vec_begin.Angle(Vec_end);

            //точность рисования кривой
            //из скольки сегментов будет составления кривая
            //если сегментов слишком мало, то возможно наложение деталей
            int PointNum=int(PI*sqrt(Radius))*4;
            if(PointNum<=2)
            {
                NoFill_Line(CnclPoint(-left,-bottom)+newBegin,CnclPoint(-left,-bottom)+newEnd);
            }
            else
            {
                double step=angl/double(PointNum-1);
                MoveTo(CnclPoint(-left,-bottom)+newBegin);
                if(z<0)
                {
                    for(double i=angl_begin;i>angl_begin-angl;i-=step)
                        NoFill_LineTo(CnclPoint(cos(i)*Radius+Center.x-left,sin(i)*Radius+Center.y-bottom));
                }
                else
                {
                    for(double i=angl_begin;i<angl_begin+angl;i+=step)
                        NoFill_LineTo(CnclPoint(cos(i)*Radius+Center.x-left,sin(i)*Radius+Center.y-bottom));
                }
                NoFill_LineTo(CnclPoint(-left,-bottom)+newEnd);
            }
        }
    }
}

void CnclRastr::PaintSpline(CnclSpline *Spline, BOOL NeedFill)
{
    //если контур не замнкнут, то нет необходимости его рисовать
    if(!Spline->IsClose())
        return;
    if(NeedFill)
    {
        last_nonzerro_dir=-2;
        CnclSpline::SnclSegment *Current=Spline->First;
        if(!Current)
            return;
        CnclSpline::SnclSegment *Last=Current->Next;
        Serial=SERIAL_FIRST;
        dir_change=0;
        while(Current)
        {
            Fill_PaintSegment(Current);
            Current=Current->Next;
            if(dir_change>2)
            {
                Serial=SERIAL_CURRENT;
            }
            else
            {
                if(Last)
                    Last=Last->Next;
            }
        }
        Serial=SERIAL_CURRENT;
        Current=Spline->First;
        do
        {
            Fill_PaintSegment(Current);
            Current=Current->Next;
        }while(Current!=Last);
    }
    else
    {
        CnclSpline::SnclSegment *Current=Spline->First;
        while(Current)
        {
            NoFill_PaintSegment(Current);
            Current=Current->Next;
        }
    }
}

//вывод детли в битовый массив
void CnclRastr::PaintDetail(const CnclDetail *Detail, BOOL NeedFill, BOOL Inverse)
{
    CnclDetail::SnclSpline *Current=Detail->First;
    while(Current)
    {
        if(Current->IsClose())
            PaintSpline(Current,NeedFill);
        Current=Current->Next;
    }
    if(NeedFill)
    {
        Current=Detail->First;
        while(Current)
        {
            if(Current->IsClose())
                PaintSpline(Current,FALSE);
            Current=Current->Next;
        }
    }
    if(Inverse)
        Invert(Detail);
}

//рисование линии в битовом массиве
//от текущей точки до Point
void CnclRastr::Fill_LineTo(const CnclPixelCoord &Point)
{
    int direction;
    if(Point.y==LastPixelCoord.y)
        direction=0;
    else
        direction=SIGN(Point.y-LastPixelCoord.y);
    CnclPixelCoord Last=LastPixelCoord;
    long x_size=Point.x-Last.x;
    long y_size=Point.y-Last.y;
    if(ABS(x_size)>=ABS(y_size))
    {
        if(x_size==0)
            return;
        if(x_size>0)
        {
            for(long i=0;i<=x_size;i++)
                Fill_SetPixel(CnclPixelCoord(i+Last.x,i*y_size/x_size+Last.y),direction);
        }
        else
        {
            for(long i=0;i>=x_size;i--)
                Fill_SetPixel(CnclPixelCoord(i+Last.x,i*y_size/x_size+Last.y),direction);
        }
    }
    else
    {
        if(y_size>0)
        {
            for(long i=0;i<=y_size;i++)
                Fill_SetPixel(CnclPixelCoord(i*x_size/y_size+Last.x,i+Last.y),direction);
        }
        else
        {
            for(long i=0;i>=y_size;i--)
                Fill_SetPixel(CnclPixelCoord(i*x_size/y_size+Last.x,i+Last.y),direction);
        }
    }
}

void CnclRastr::NoFill_LineTo(const CnclPoint &Point)
{
    CnclPoint Last=LastPointCoord;
    double x_size=Point.x-Last.x;
    double y_size=Point.y-Last.y;
    double delta;
    if(ABS(x_size)>=ABS(y_size))
    {
        if(x_size==0)
            return;
        delta=y_size/x_size;
        if(x_size>0)
        {
            for(double i=0;i<=x_size;i++)
                NoFill_SetPixel(CnclPoint(i+Last.x,i*delta+Last.y),TRUE);
        }
        else
        {
            for(double i=0;i>=x_size;i--)
                NoFill_SetPixel(CnclPoint(i+Last.x,i*delta+Last.y),TRUE);
        }
    }
    else
    {
        delta=x_size/y_size;
        if(y_size>0)
        {
            for(double i=0;i<=y_size;i++)
                NoFill_SetPixel(CnclPoint(i*delta+Last.x,i+Last.y),TRUE);
        }
        else
        {
            for(double i=0;i>=y_size;i--)
                NoFill_SetPixel(CnclPoint(i*delta+Last.x,i+Last.y),TRUE);
        }
    }
    NoFill_SetPixel(Point,TRUE);
}

//рисование линии в битовом массиве
//от точки Begin до точки End
void CnclRastr::Fill_Line(const CnclPixelCoord &Begin,const CnclPixelCoord &End)
{
    MoveTo(CnclPoint(Begin.x,Begin.y));
    Fill_LineTo(End);
}

void CnclRastr::NoFill_Line(const CnclPoint &Begin,const CnclPoint &End)
{
    NoFill_SetPixel(Begin,TRUE);
    MoveTo(Begin);
    NoFill_LineTo(End);
}

//функция округления в зависимости от параметра flag
//INTEGER_MAX - округление в большую сторону
//INTEGER_MIN - отбрасывается дробная часть
long CnclRastr::Integer(double number,long flag)
{
    if(flag==INTEGER_MAX)
        return (long(number)<number ? long(number)+1 : long(number));
    return long(number);
}

/*
void CnclRastr::Rectangle(long x1, long y1, long x2, long y2)
{
    glBegin(GL_LINE_STRIP);
    glVertex2i(x1,y1);
    glVertex2i(x1,y2);
    glVertex2i(x2,y2);
    glVertex2i(x2,y1);
    glVertex2i(x1,y1);
    glEnd();
}
*/
