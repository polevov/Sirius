#include "define.h"
#include "nclFile.h"

#include "nclFont.h"


CnclFont::CnclFont()
{
    First=NULL;
    Last=NULL;
    steck_full=0;
    scale=1;
}


CnclFont::~CnclFont()
{
    Clear();
}

void CnclFont::Clear()
{
    CnclSymbol *Current=First;
    while(Current)
    {
        First=First->Next;
        delete Current;
        Current=First;
    }
    First=NULL;
    Last=NULL;

}

BOOL CnclFont::Add(CnclSymbol *Symbol)
{
    if(Last)Last->Next=Symbol;
    Last=Symbol;
    Last->Next=NULL;
    if(!First)First=Symbol;
    return TRUE;
}

void CnclFont::Load(WCHAR *FileName)
{
    CnclFile file;
    file.open(FileName,GENERIC_READ,OPEN_EXISTING);
    WCHAR str[2024]=L"\0";
    while(file.good())
    {
        file.getline(str,sizeof(str));
        if(str[0]!=L'*')
            continue;
        if(wcsncmp(&str[1],L"UNIFONT",7)==0)
        {
            file.getline(str,sizeof(str));
            int num_count=ParseString(str,FALSE);
            if(num_count>2)
            {
                H=numbers[0];
                U=numbers[1];
            }
        }
        else
        {
            int num_count=ParseString(str,FALSE);
            WCHAR smb=0;
            if(num_count>0)
                smb=WCHAR(numbers[0]);
            WCHAR out[]={0,0};
            out[0]=smb;
            file.getline(str,sizeof(str));

            WCHAR buf[2024]=L"\0";
            DWORD pos=0;
            do
            {
                pos=file.tellg();
                file.getline(buf,sizeof(buf));
                if(buf[0]==L'*')
                {
                    file.seekg(pos);
                    break;
                }
                lstrcat(str,buf);
            }while(file.good());

            num_count=ParseString(str,TRUE);
            if(num_count==0)
                continue;
            CnclSymbol *symbol=new CnclSymbol;
            Add(symbol);
            symbol->symbol=smb;
            BOOL Error=FALSE;
            BOOL Draw=TRUE;
            for(int i=0;i<num_count && !Error;i++)
            {
                if(numbers[i]>1000)
                {
                    numbers[i]-=2000;
                    int len=numbers[i]/16;
                    int direction=numbers[i]%16;
                    int dx=(Round(cos(direction*22.5/360*(PI*2)),2)==0?0:(cos(direction*22.5/360*(PI*2))>0?len:-len));
                    int dy=(Round(sin(direction*22.5/360*(PI*2)),2)==0?0:(sin(direction*22.5/360*(PI*2))>0?len:-len));
                    CnclPoint end(symbol->end.x+dx*scale,symbol->end.y+dy*scale);
                    if(Draw)
                        symbol->Add(symbol->end,end,0,false);
                    symbol->end=end;
                    continue;
                }
                switch(numbers[i])
                {
                case 0:
                    Error=TRUE;
                    break;
                case 1:
                    Draw=TRUE;
                    break;
                case 2:
                    Draw=FALSE;
                    break;
                case 3:
                    if(numbers[i+1]>1000)
                        numbers[i+1]-=2000;
                    scale=scale/numbers[i+1];
                    i++;
                    break;
                case 4:
                    if(numbers[i+1]>1000)
                        numbers[i+1]-=2000;
                    scale=scale*numbers[i+1];
                    i++;
                    break;
                case 5:
                    steck[steck_full]=symbol->end;
                    steck_full++;
                    break;
                case 6:
                    steck_full--;
                    symbol->end=steck[steck_full];
                    break;
                case 7:
                {
                    CnclSymbol *current=First;
                    if(numbers[i+1]>1000)
                        numbers[i+1]-=2000;
                    while(current)
                    {
                        if(current->symbol==numbers[i+1])
                        {
                            CnclSpline::SnclSegment* cur=current->First;
                            CnclPoint start=symbol->end;
                            while(cur)
                            {
                                CnclPoint begin=cur->RealBegin;
                                CnclPoint end=cur->RealEnd;
                                begin.x=begin.x*scale+start.x;
                                begin.y=begin.y*scale+start.y;
                                end.x=end.x*scale+start.x;
                                end.y=end.y*scale+start.y;
                                if(Draw)
                                    symbol->Add(begin,end,cur->z,FALSE);
                                symbol->end=end;
                                cur=cur->Next;
                            }
                            break;
                        }
                        current=current->Next;
                    }
                    i++;
                    break;
                }
                case 8:
                case 9:
                {
                    BOOL loop=(numbers[i]==9);
                    do
                    {
                        if(numbers[i+1]>1000)
                            numbers[i+1]-=2000;
                        if(numbers[i+2]>1000)
                            numbers[i+2]-=2000;
                        if(numbers[i+1]!=0 || numbers[i+2]!=0)
                        {
                            CnclPoint end(symbol->end.x+numbers[i+1]*scale,symbol->end.y+numbers[i+2]*scale);
                            if(Draw)
                            {
                                symbol->Add(symbol->end,end,0,FALSE);
                            }
                            symbol->end=end;
                        }
                        else
                        {
                            loop=FALSE;
                        }
                        i+=2;
                    }while(loop);
                    break;
                }
                case 10:
                {
                    if(numbers[i+1]>1000)
                        numbers[i+1]-=2000;
                    if(numbers[i+2]>1000)
                        numbers[i+2]-=2000;
                    int r=numbers[i+1];
                    double start=ABS(int(numbers[i+2]/16));
                    double end=start+numbers[i+2]%16;
                    if(start!=end)
                    {
                        int dx=(Round(cos(end*45/360*(PI*2)),2)==0?0:(cos(end*45/360*(PI*2))>0?r:-r))-(Round(cos(start*45/360*(PI*2)),2)==0?0:(cos(start*45/360*(PI*2))>0?r:-r));
                        int dy=(Round(sin(end*45/360*(PI*2)),2)==0?0:(sin(end*45/360*(PI*2))>0?r:-r))-(Round(sin(start*45/360*(PI*2)),2)==0?0:(sin(start*45/360*(PI*2))>0?r:-r));
                        //if(numbers[i+2]<0)
                        //{
                        //dx=-dx;
                        //dy=-dy;
                        //}
                        CnclPoint end(symbol->end.x+dx*scale,symbol->end.y+dy*scale);
                        double arc=tan(double(numbers[i+2]%16)*45/360*(2*PI)/4);//*(numbers[i+2]>0?1:-1);
                        if(Draw)
                            symbol->Add(symbol->end,end,arc,false);
                        symbol->end=end;
                    }
                    else
                    {
                        end=start+4;
                        int dx=(Round(cos(end*45/360*(PI*2)),2)==0?0:(cos(end*45/360*(PI*2))>0?r:-r))-(Round(cos(start*45/360*(PI*2)),2)==0?0:(cos(start*45/360*(PI*2))>0?r:-r));
                        int dy=(Round(sin(end*45/360*(PI*2)),2)==0?0:(sin(end*45/360*(PI*2))>0?r:-r))-(Round(sin(start*45/360*(PI*2)),2)==0?0:(sin(start*45/360*(PI*2))>0?r:-r));

                        CnclPoint end(symbol->end.x+dx*scale,symbol->end.y+dy*scale);
                        double arc=tan(double(PI/4))*(numbers[i+2]>0?1:-1);
                        if(Draw)
                        {
                            symbol->Add(symbol->end,end,arc,false);
                            symbol->Add(end,symbol->end,arc,false);
                        }

                    }
                    i+=2;
                    break;
                }
                case 12:
                case 13:
                {
                    BOOL loop=(numbers[i]==13);
                    do
                    {
                        if(numbers[i+1]>1000)
                            numbers[i+1]-=2000;
                        if(numbers[i+2]>1000)
                            numbers[i+2]-=2000;
                        if(numbers[i+3]>1000)
                            numbers[i+3]-=2000;
                        if(numbers[i+1]!=0 || numbers[i+2]!=0)
                        {
                            double arc=numbers[i+3];
                            if(arc!=0)
                                arc=(arc/127);

                            //arc=0;
                            CnclPoint end(symbol->end.x+double(numbers[i+1])*scale,symbol->end.y+double(numbers[i+2])*scale);
                            if(Draw)
                                symbol->Add(symbol->end,end,arc,false);
                            symbol->end=end;
                            i+=3;
                        }
                        else
                        {
                            loop=FALSE;
                            i+=2;
                        }
                    }while(loop);
                    break;
                }
                default:
                    Error=TRUE;
                    break;
                }
            }
            symbol->Recalc();
            scale=1;
        }
    }
    file.close();
}

int CnclFont::ParseString(WCHAR *str,BOOL hex2000)
{
    WCHAR parse[2024];
    lstrcpy(parse,str);
    int len=lstrlen(parse);
    for(int i=0;i<len;i++)
    {
        if(parse[i]>=L'0' && parse[i]<=L'9')
            continue;
        if(parse[i]>=L'A' && parse[i]<=L'F')
            continue;
        if(parse[i]>=L'a' && parse[i]<=L'f')
        {
            parse[i]=(parse[i]+(L'A'-L'a'));
            continue;
        }
        if(parse[i]==L'-')
            continue;
        parse[i]='\0';
    }
    int count=0;
    for(int i=0;i<len;i++)
    {
        int num_len=lstrlen(&parse[i]);
        if(num_len>0)
        {
            BOOL hex=FALSE;
            if(parse[i]==L'0' && num_len>1)
                hex=TRUE;
            if(num_len>1 && parse[i]==L'-')
            {
                if(parse[i+1]==L'0')
                    hex=TRUE;
            }
            if(hex)
            {
                numbers[count]=wcstol(&parse[i],NULL,16)+(hex2000?2000:0);
            }
            else
            {
                numbers[count]=wcstol(&parse[i],NULL,10);
            }
            count++;
            i+=num_len;
        }
    }
    return count;
}

void CnclFont::PrintText(CnclSpline* spline,WCHAR *str,double x, double y,double height,double angle,double w_scale)
{
    double x_c=x;
    double y_c=y;
    int len=lstrlen(str);
    spline->Clear();
    double scale=H/height;
    for(int i=0;i<len;i++)
    {
        CnclSymbol *current=First;
        while(current)
        {
            if(str[i]==current->symbol)
            {
                CnclSpline::SnclSegment *cur=current->First;
                while(cur)
                {
                    CnclSpline::SnclSegment *seg=new CnclSpline::SnclSegment(CnclPoint(cur->RealBegin.x/scale*w_scale+x,cur->RealBegin.y/scale+y),CnclPoint(cur->RealEnd.x/scale*w_scale+x,cur->RealEnd.y/scale+y),cur->z,FALSE);
                    spline->Add(seg,FALSE);
                    cur=cur->Next;
                }
                x+=current->end.x/scale*w_scale;
                y+=current->end.y/scale;
                break;
            }
            current=current->Next;
        }
    }
    if(angle!=0)
    {
        angle=double(angle)*2*PI/360;
        CnclSpline::SnclSegment *cur=spline->First;
        while(cur)
        {
            cur->RealBegin.x-=x_c;
            cur->RealBegin.y-=y_c;
            cur->RealEnd.x-=x_c;
            cur->RealEnd.y-=y_c;
            double x=cur->RealBegin.x*cos(angle)-cur->RealBegin.y*sin(angle);
            double y=cur->RealBegin.x*sin(angle)+cur->RealBegin.y*cos(angle);
            cur->RealBegin.x=x;
            cur->RealBegin.y=y;

            x=  cur->RealEnd.x*cos(angle)-  cur->RealEnd.y*sin(angle);
            y=  cur->RealEnd.x*sin(angle)+  cur->RealEnd.y*cos(angle);
            cur->RealEnd.x=x;
            cur->RealEnd.y=y;

            cur->RealBegin.x+=x_c;
            cur->RealBegin.y+=y_c;
            cur->RealEnd.x+=x_c;
            cur->RealEnd.y+=y_c;

            cur=cur->Next;
        }
        spline->Recalc();
    }

}
