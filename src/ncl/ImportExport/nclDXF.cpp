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

#include "common.h"
#include "ncl/define.h"
#include "nclDXF.h"
#include "ncl/nclFont.h"

CnclDXF::CnclDXF():CnclCommonImport()
{
    code[0]='\0';
    val[0]='\0';
}

CnclDXF::~CnclDXF()
{

}

void CnclDXF::Read(const WCHAR *FileName,BOOL Build,BOOL nest)
{
    Clear();
    CnclFile DXF(FileName,GENERIC_READ,OPEN_EXISTING);
    //File DXF(FileName,GENERIC_READ,OPEN_EXISTING);
    BOOL ReadGeom=FALSE;
    while(TRUE)
    {
        Trim(code);
        Trim(val);
        if(lstrcmpi(val,L"INSERT")==0)
        {
            ReadBlock(&DXF);
        }
        if(lstrcmpi(val,L"ENTITIES")==0)
        {
            ReadGeom=TRUE;
        }
        else
        {
            if(lstrcmpi(val,L"ENDSEC")==0)
            {
                ReadGeom=FALSE;
            }
            else
            {
                if(lstrcmpi(val,L"EOF")==0)
                    break;
            }
        }
        if(ReadGeom)
        {
            if(lstrcmpi(val,L"CIRCLE")==0)
                ReadCircle(&DXF);
            else if(lstrcmpi(val,L"ARC")==0)
                ReadArc(&DXF);
            else if(lstrcmpi(val,L"LINE")==0)
                ReadLine(&DXF);
            else if(lstrcmpi(val,L"POLYLINE")==0)
                ReadPolyline(&DXF);
            else if(lstrcmpi(val,L"LWPOLYLINE")==0)
                ReadLwPolyline(&DXF);
            else if(lstrcmpi(val,L"TEXT")==0)
                ReadText(&DXF);
            else
            {
                if(!GetNextLine(&DXF))
                    break;
            }
        }
        else
            if(!GetNextLine(&DXF))
                break;
    }
    DXF.close();
    WCHAR Name[MAX_PATH];
    if(wcsrchr(FileName,'\\'))
        lstrcpy(Name,&(wcsrchr(FileName,'\\')[1]));
    else
        lstrcpy(Name,FileName);
    if(wcsrchr(Name,'.'))
    {
        wcsrchr(Name,'.')[0]=L'\0';
    }
    BuildDetail(Name,nest);
}

void CnclDXF::ReadBlock(CnclFile* DXF)
{
    WCHAR block_name[1024]=L"\0";
    DWORD current_pos=DXF->tellg();
    while(TRUE)
    {
        if(!GetNextLine(DXF))
            break;
        Trim(code);
        Trim(val);
        if(lstrcmpi(code,L"0")==0)
            break;
        else if(lstrcmpi(code,L"2")==0)
        {
            lstrcpy(block_name,val);
        }
    }
    if(lstrlen(block_name)>0)
    {
        DXF->seekg(0);
        BOOL find=FALSE;
        while(TRUE)
        {
            Trim(code);
            Trim(val);
            if(lstrcmpi(val,L"BLOCK")==0)
            {
                while(TRUE)
                {
                    if(!GetNextLine(DXF))
                        break;
                    Trim(code);
                    Trim(val);
                    if(lstrcmpi(code,L"0")==0)
                        break;
                    if(lstrcmpi(code,L"2")==0)
                    {
                        if(lstrcmpi(block_name,val)==0)
                        {
                            find=TRUE;
                            break;
                        }
                    }
                }
            }
            if(find)
            {
                if(lstrcmpi(val,L"CIRCLE")==0)
                    ReadCircle(DXF);
                else if(lstrcmpi(val,L"ARC")==0)
                    ReadArc(DXF);
                else if(lstrcmpi(val,L"LINE")==0)
                    ReadLine(DXF);
                else if(lstrcmpi(val,L"POLYLINE")==0)
                    ReadPolyline(DXF);
                else if(lstrcmpi(val,L"LWPOLYLINE")==0)
                    ReadLwPolyline(DXF);
                else if(lstrcmpi(val,L"ENDBLK")==0)
                    break;
                else
                {
                    if(!GetNextLine(DXF))
                        break;
                }
            }
            else
            {
                if(!GetNextLine(DXF))
                    break;
            }
        }

    }
    DXF->seekg(current_pos);
}

void CnclDXF::ReadPolyline(CnclFile* DXF)
{
    double x,y,z;
    double x_old=0,y_old=0,z_old=0;
    long count=0;
    long Type_70=0;
    CnclPoint First;
    while(TRUE)
    {
        Trim(code);
        Trim(val);
        if(lstrcmpi(val,L"VERTEX")==0)
        {
            count++;
            if(count>1)
            {
                x_old=x;
                y_old=y;
                z_old=z;
            }
            GetVertex(DXF,x,y,z);
            if(count==1)
                First.Set(x,y);
        }
        else if(lstrcmpi(val,L"SEQEND")==0)
            break;
        else
        {
            if(lstrcmpi(code,L"70")==0)
                Type_70=_wtol(val);
            if(!GetNextLine(DXF))
                break;
        }
        if(count>1)
            Spline.Add(CnclPoint(x_old,y_old),CnclPoint(x,y),z_old,FALSE);
    }
    if(Spline.First)
    {
        if(Type_70==1)
        {
            if(!Spline.IsClose())
                Spline.Add(Spline.Last->RealEnd,First,z,FALSE);
        }
    }
}

void CnclDXF::ReadLwPolyline(CnclFile* DXF)
{
    double x=0,y=0,z=0;
    double x_old=0,y_old=0,z_old=0;
    long count=0;
    long Type_70=0;
    CnclPoint First;
    bool findPoint=false;
    while(TRUE)
    {
        if(!GetNextLine(DXF))
            break;
        Trim(code);
        Trim(val);
        if(lstrcmpi(code,L"0")==0)
            break;
        if(lstrcmpi(code,L"10")==0)
        {
            findPoint=false;
            x=StrToDouble(val);
        }
        if(lstrcmpi(code,L"20")==0)
        {
            findPoint=true;
            y=StrToDouble(val);
        }
        if(findPoint)
        {
            count++;
            if(count==1)
                First.Set(x,y);
            if(count>1)
            {
                Spline.Add(CnclPoint(x_old,y_old),CnclPoint(x,y),z_old,FALSE);
            }
            x_old=x;
            y_old=y;
        }
        else if(lstrcmpi(val,L"SEQEND")==0)
            break;
        else
        {
            if(lstrcmpi(code,L"70")==0)
                Type_70=_wtol(val);
        }
    }
    if(Spline.First)
    {
        if(Type_70==1)
        {
            if(!Spline.IsClose())
                Spline.Add(Spline.Last->RealEnd,First,z,FALSE);
        }
    }
}

void CnclDXF::ReadCircle(CnclFile* DXF)
{
    double x=0,y=0,r=0;
    while(TRUE)
    {
        if(!GetNextLine(DXF))
            break;
        Trim(code);
        Trim(val);
        if(lstrcmpi(code,L"0")==0)
            break;
        else if(lstrcmpi(code,L"10")==0)
        {
            x=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"20")==0)
        {
            y=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"40")==0)
        {
            r=StrToDouble(val);
        }
    }
    if(r>0)
    {
        Spline.Add(CnclPoint(x-r,y),CnclPoint(x+r,y),1,FALSE);
        Spline.Add(CnclPoint(x+r,y),CnclPoint(x-r,y),1,FALSE);
    }
}

void CnclDXF::ReadArc(CnclFile* DXF)
{
    double x=0,y=0;
    double r=0;
    double start_angle=0;	//начальный угол дуги в градусах
    double end_angle=0;		//конечный угол дуги в градусах
    double zExtrusion=1;
    while(DXF->good())
    {
        if(!GetNextLine(DXF))
            break;
        Trim(code);
        Trim(val);
        if(lstrcmpi(code,L"0")==0)
            break;
        else if(lstrcmpi(code,L"10")==0)
        {
            x=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"20")==0)
        {
            y=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"40")==0)
        {
            r=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"50")==0)
        {
            start_angle=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"51")==0)
        {
            end_angle=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"230")==0)
        {
            zExtrusion=StrToDouble(val);
        }
    }
    if(zExtrusion<0)
    {
        x=-x;
        start_angle=180-start_angle;
        end_angle=180-end_angle;
        double a=start_angle;
        start_angle=end_angle;
        end_angle=a;
    }
    if(r>0)
    {
        CnclPoint Start((r*cos(start_angle*PI/180)+x),r*sin(start_angle*PI/180)+y);
        CnclPoint End((r*cos(end_angle*PI/180)+x),r*sin(end_angle*PI/180)+y);
        double z=(end_angle-start_angle);
        if(z<0)
            z+=360;
        if(ABS(z)<360)
        {
            z=tan(z*PI/180/4);
            Spline.Add(Start,End,z,FALSE);
        }
    }
}

void CnclDXF::ReadLine(CnclFile* DXF)
{
    double x1=0,y1=0;
    double x2=0,y2=0;
    while(DXF->good())
    {
        if(!GetNextLine(DXF))
            break;
        Trim(code);
        Trim(val);
        if(lstrcmpi(code,L"0")==0)
            break;
        else if(lstrcmpi(code,L"10")==0)
        {
            x1=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"20")==0)
        {
            y1=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"11")==0)
        {
            x2=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"21")==0)
        {
            y2=StrToDouble(val);
        }
    }
    Spline.Add(CnclPoint(x1,y1),CnclPoint(x2,y2),0,FALSE);
}

void CnclDXF::ReadText(CnclFile* DXF)
{
    double x=0,y=0;
    double size=0;
    double angle=0;
    double scale=1;
    WCHAR text[2014];
    while(DXF->good())
    {
        if(!GetNextLine(DXF))
            break;
        Trim(code);
        Trim(val);
        if(lstrcmpi(code,L"0")==0)
            break;
        else if(lstrcmpi(code,L"10")==0)
        {
            x=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"20")==0)
        {
            y=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"40")==0)
        {
            size=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"50")==0)
        {
            angle=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"41")==0) //закоментировано так как из дуг получаются дуги эллипса
        {
            //		scale=Application->StrToDouble(val);
        }
        else if(lstrcmpi(code,L"1")==0)
        {
            lstrcpy(text,val);
        }
    }
    CnclSpline spline;
    WCHAR Name[1024];
    swprintf_s(Name,L"TEXT%d",CountDetail());
    CnclFont font;
    font.PrintText(&spline,text,x,y,size,angle,scale);
    CnclSpline::SnclSegment *current=spline.First;
    while(current)
    {
        Spline.Add(current->RealBegin,current->RealEnd,current->z,false);
        current=current->Next;
    }
}

void CnclDXF::GetVertex(CnclFile* DXF,double &x, double &y, double &z)
{
    x=0;
    y=0;
    z=0;
    while(DXF->good())
    {
        if(!GetNextLine(DXF))
            break;
        Trim(code);
        Trim(val);
        if(lstrcmpi(code,L"0")==0)
            break;
        else if(lstrcmpi(code,L"10")==0)
        {
            x=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"20")==0)
        {
            y=StrToDouble(val);
        }
        else if(lstrcmpi(code,L"42")==0)
        {
            z=StrToDouble(val);
        }
    }
}

BOOL CnclDXF::GetNextLine(CnclFile* DXF)
{
    DXF->getline(code,sizeof(code));
    if(!DXF->good())
        return FALSE;
    DXF->getline(val,sizeof(val));
    if(!DXF->good())
        return FALSE;
    return TRUE;
}



void CnclDXF::Save(CnclNest *Nest,WCHAR *FileName)
{
    lstrcpy(&FileName[lstrlen(FileName)-3],L"dxf");
    CnclFile OutFile(FileName,GENERIC_WRITE,CREATE_ALWAYS);
    CnclNest::SnclDetail *Current=NULL;
    CnclDetail *Detail;
    CnclPoint newBegin;
    CnclPoint newEnd;
    double z;

    OutFile<<L"  0"<<ENDL;
    OutFile<<L"SECTION"<<ENDL;
    OutFile<<L"  2"<<ENDL;
    OutFile<<L"ENTITIES"<<ENDL;
    do
    {
        if(!Current)
            Detail=&Nest->Sheet;
        else
            Detail=Current;

        //Выгрузка детали в DXF
        CnclDetail::SnclSpline *CurSpline=Detail->First;
        while(CurSpline)
        {
            CnclSpline::SnclSegment *Segment=CurSpline->First;
            while(Segment)
            {
                newBegin=Segment->GetNewCoord(Segment->Begin,FALSE,NULL);
                newEnd=Segment->GetNewCoord(Segment->End,FALSE,NULL);
                z=Segment->GetNewZ();

                if(Segment==CurSpline->First)
                {
                    OutFile<<L"  0"<<ENDL;
                    OutFile<<L"POLYLINE"<<ENDL;
                    OutFile<<L"  8"<<ENDL;
                    OutFile<<L"0"<<ENDL;
                    OutFile<<L" 10"<<ENDL;
                    OutFile<<L"0"<<ENDL;
                    OutFile<<L" 20"<<ENDL;
                    OutFile<<L"0"<<ENDL;
                    OutFile<<L" 39"<<ENDL;
                    OutFile<<L"0"<<ENDL;
                    OutFile<<L" 70"<<ENDL;
                    if(CurSpline->IsClose())
                        OutFile<<L"1"<<ENDL;
                    else
                        OutFile<<L"0"<<ENDL;
                    OutFile<<L" 66"<<ENDL;
                    OutFile<<L"1"<<ENDL;
                }
                OutFile<<L"  0"<<ENDL;
                OutFile<<L"VERTEX"<<ENDL;
                OutFile<<L"  8"<<ENDL;
                OutFile<<L"0"<<ENDL;
                OutFile<<L" 10"<<ENDL;
                OutFile<<newBegin.x<<ENDL;
                OutFile<<L" 20"<<ENDL;
                OutFile<<newBegin.y<<ENDL;
                OutFile<<L" 42"<<ENDL;
                OutFile<<z<<ENDL;
                if(Segment==CurSpline->Last && !CurSpline->IsClose())
                {
                    OutFile<<L"  0"<<ENDL;
                    OutFile<<L"VERTEX"<<ENDL;
                    OutFile<<L"  8"<<ENDL;
                    OutFile<<L"0"<<ENDL;
                    OutFile<<L" 10"<<ENDL;
                    OutFile<<newEnd.x<<ENDL;
                    OutFile<<L" 20"<<ENDL;
                    OutFile<<newEnd.y<<ENDL;
                    OutFile<<L" 42"<<ENDL;
                    OutFile<<L"0"<<ENDL;
                }
                Segment=Segment->Next;
            }
            OutFile<<L"  0"<<ENDL;
            OutFile<<L"SEQEND"<<ENDL;
            CurSpline=CurSpline->Next;
        }

        if(!Current)
            Current=Nest->First;
        else
            Current=Current->Next;
    }while(Current);

    OutFile<<L"  0"<<ENDL;
    OutFile<<L"ENDSEC"<<ENDL;
    OutFile<<L"  0"<<ENDL;
    OutFile<<L"EOF"<<ENDL;
    OutFile.close();
}


