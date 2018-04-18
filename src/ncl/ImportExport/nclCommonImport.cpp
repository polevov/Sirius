// nclCommonImport.cpp: implementation of the CnclCommonImport class.
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ncl/define.h"
#include "nclCommonImport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool IsFile(WCHAR* FileName)
{
    WIN32_FIND_DATA Data;
    bool result=true;
    HANDLE hFind=FindFirstFile(FileName,&Data);
    if(INVALID_HANDLE_VALUE==hFind)
        result=FALSE;
    FindClose(hFind);
    return result;

}

double Round(double num, long pr)
{
    double sign=num>0?1:-1;
    num=ABS(num);
    return (double)((__int64)(num*pow(10.0,pr)+0.5)/pow(10.0,pr))*sign;
}

long dummy;

//инициализация генератора случайных чисел
void nclRand(long dum)
{
    dummy=dum;
}

//генератор случайных чисел
//от 0 до 0.99999999999999
float nclRand()
{
#define IM1 2147483563
#define IM2 2147483399
#define AM (1.f/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define NWUP 8
#define EPS 1.2e-7f
#define RNMX (1.0f-EPS)

    short int j;
    long k;
    static long dummy2=123456789;
    static long iy=0;
    static long iv[NTAB];
    float temp;
    /* initialize the random sequence (first set of coefficients, the
        routine close to that in the function above */
    if(dummy<=0 || !iy)
    {
        /* avoid negative or zero seed */
        if(dummy<0)
            dummy=-dummy;
        else if(dummy==0)
            dummy=1;
        dummy2=dummy;
        /* after NWUP warmups, initialize shuffle table */
        for(j=NTAB+NWUP-1;j>=0;j--)
        {
            k=dummy/IQ1;
            if((dummy=IA1*(dummy-k*IQ1)-IR1*k)<0)
                dummy+=IM1;
            if(j<NTAB)
                iv[j]=dummy;
        }
        /* first specimen from the table */
        iy=iv[0];
    }
    /* regular work: generate 2 sequences. */
    k=dummy/IQ1;
    if((dummy=IA1*(dummy-k*IQ1)-IR1*k)<0)
        dummy+=IM1;
    k=dummy2/IQ2;
    if((dummy2=IA2*(dummy2-k*IQ2)-IR2*k)<0)
        dummy2+=IM2;
    /* shuffle output combining 2 sequences */
    iy=iv[j=short(iy/NDIV)]-dummy2;
            iv[j]=dummy;
            if(iy<1)
            iy+=IMM1;
            /* return the result, as in the previous function */
            if((temp=AM*iy)>RNMX)
            return(RNMX);
            else
            return(temp);

}

            //удаление непечатных символов и пробелов из строки
            WCHAR* Trim(WCHAR *str)
    {
        long len=lstrlen(str);
        long i;
        for(i=0;i<len;i++)
        {
            if((str[i])>WCHAR(' '))
                break;
        }
        lstrcpy(str,&str[i]);
        for(i=len-1;i>=0;i--)
        {
            if(WCHAR(str[i])>WCHAR(' '))
                break;
            else
                str[i]='\0';
        }
        return str;
    }

    //перевод строки в число
    double StrToDouble(WCHAR *str)
    {
    _wsetlocale( LC_ALL, L"en-US" );
    WCHAR buf[128];
    lstrcpy(buf,str);
    Trim(buf);
    int len;
    len=lstrlen(buf);
    for(int i=0;i<len;i++)
    {
        if(buf[i]==L',')
            buf[i]=L'.';
    }
    return _wtof(buf);
}

//перевод числа в строку
WCHAR* DoubleToStr(double num,WCHAR *buf)
{
    WCHAR buf_t[64];
    swprintf(buf_t,L"%.20g",num);
    lstrcpy(buf,buf_t);
    return buf;

}

CnclCommonImport::CnclCommonImport()
{
    First=NULL;
    Last=NULL;
    Count=0;
    Flag=0;
    angle_step=0;
    Name[0]='\0';
}

CnclCommonImport::CnclCommonImport(WCHAR *_Name,long _Count,long _Flag,long _angle_step)
{
    First=NULL;
    Last=NULL;
    Count=_Count;
    Flag=_Flag;
    angle_step=_angle_step;
    lstrcpy(Name,_Name);
}

CnclCommonImport::~CnclCommonImport()
{
    Clear();
}

void CnclCommonImport::BuildDetail(WCHAR *_Name,BOOL nest)
{
    if(!Add())
        return;
    if(_Name)
        lstrcpy(Name,_Name);
    lstrcpy(Last->Name,Name);
    CnclSpline::SnclSegment *Current;
    CnclSpline newSpline;
    BOOL Find;
    long loop=0;
    while(Spline.First && loop<100000)
    {
        loop++;
        if(newSpline.First)
        {
            if(newSpline.First!=newSpline.Last)
            {
                Spline.Add(newSpline.Last->RealBegin,newSpline.Last->RealEnd,newSpline.Last->z,TRUE);
                Current=Spline.First->Next;
            }
            else
                Current=Spline.First;
            newSpline.Delete(newSpline.Last);
        }
        else
        {
            Current=Spline.First;
        }
        while(Current)
        {
            Find=FALSE;
            if(newSpline.First)
            {
                if(newSpline.Last->RealEnd.Distance(Current->RealBegin)<=max_dist)
                {
                    Find=TRUE;
                }
                else if(newSpline.Last->RealEnd.Distance(Current->RealEnd)<=max_dist)
                {
                    CnclPoint buf=Current->RealEnd;
                    Current->RealEnd=Current->RealBegin;
                    Current->RealBegin=buf;
                    Current->z=-Current->z;
                    Find=TRUE;
                }
            }
            if(Find || !newSpline.First)
            {
                newSpline.Add(Current->RealBegin,Current->RealEnd,Current->z,FALSE);
                Spline.Delete(Current);
                if(newSpline.First->RealBegin.Distance(newSpline.Last->RealEnd)<=max_dist)
                {
                    Last->Add(&newSpline);
                    loop=0;
                    newSpline.Clear();
                }
                Current=Spline.First;
                continue;
            }
            else
            {
                Current=Current->Next;
            }
        }
        if(newSpline.Count()>0)
        {
            Last->Add(&newSpline);
            newSpline.Clear();
        }

    }
    if(Last->First)
    {
        Last->Check();
    }
    else
    {
        SnclDetail *del=Last;
        if(Last->Prev)
        {
            Last=Last->Prev;
            Last->Next=NULL;
            delete del;
        }
        else
            Clear();
    }
    if(nest)
        BuildNest();
}

BOOL CnclCommonImport::Add()
{
    SnclDetail *Detail=new SnclDetail;
    if(!Detail)
        return FALSE;
    Detail->Prev=Last;
    if(!First)
        First=Detail;
    if(Last)
        Last->Next=Detail;
    Last=Detail;
    Last->Text=FALSE;
    return TRUE;
}

void CnclCommonImport::Clear()
{
    SnclDetail *Current=First;
    while(Current)
    {
        First=First->Next;
        delete Current;
        Current=First;
    }
    First=NULL;
    Last=NULL;
    Spline.Clear();
}

CnclCommonImport::SnclDetail::SnclDetail():CnclDetail()
{
    Next=NULL;
    Prev=NULL;
}

CnclCommonImport::SnclDetail::~SnclDetail()
{
}

long CnclCommonImport::CountDetail()
{
    SnclDetail *Current=First;
    long count_det=0;
    while(Current)
    {
        count_det++;
        Current=Current->Next;
    }
    return count_det;
}

void CnclCommonImport::BuildNest()
{
    if(!First)
        return;
    SnclDetail *Current=First;
    SnclDetail *all_splines=NULL;
    while(Current)
    {
        if(Current->Text)
        {
            nest.Add(Current);
        }
        else
        {
            all_splines=Current;
        }
        Current=Current->Next;
    }
    if(!all_splines)
        return;

    CnclDetail::SnclSpline *CurSpline=all_splines->First;
    CnclDetail::SnclSpline *maxSpline=NULL;
    double maxArea=0;
    while(CurSpline)
    {
        if(!maxSpline)
        {
            maxSpline=CurSpline;
        }
        else
        {
            double curArea=CurSpline->Area();
            if(ABS(maxArea)<ABS(curArea))
            {
                maxArea=curArea;
                maxSpline=CurSpline;
            }
        }
        CurSpline=CurSpline->Next;
    }
    if(maxSpline)
    {
        CnclDetail det;
        lstrcpy(det.Name,L"Sheet");
        det.ID=0;
        det.Add(maxSpline);
        nest.SetSheet(&det);
    }
}
