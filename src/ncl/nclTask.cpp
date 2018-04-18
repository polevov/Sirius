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
#include "nclTask.h"
#include "common.h"

#include "ImportExport\\nclDXF.h"
#include "ImportExport\\nclDBS.h"

extern CnclTransformView DrawTransform;

CnclTask::CnclTask()
{
    DetailFirst=NULL;
    SheetFirst=NULL;
    Task=NULL;
    TaskBest=NULL;
    BlockFirst=NULL;
    BlockLast=NULL;
    Clear(CLEAR_ALL);
}

CnclTask::~CnclTask()
{
    Clear(CLEAR_ALL);
}

//Добавляет деталь к заданию
BOOL CnclTask::Add(CnclDetail *Detail, long Count,long Flag)
{
    SnclDetail *newDetail=new SnclDetail(*Detail, Count);
    if(!newDetail)
    {
        return FALSE;
    }
    newDetail->DrawMatrix=Detail->DrawMatrix;//&Application->DrawTransform;
    newDetail->Recalc();
    return Add(newDetail, Flag);
}

//Добавляет деталь к заданию
BOOL CnclTask::Add(CnclTask::SnclDetail *Detail,long Flag)
{
    switch(Flag)
    {
    case LOAD_DETAIL:
        Detail->Prev=DetailLast;
        if(!DetailFirst)
            DetailFirst=Detail;
        if(DetailLast)
            DetailLast->Next=Detail;
        DetailLast=Detail;
        break;
    case LOAD_SHEET:
        Detail->Prev=SheetLast;
        if(!SheetFirst)
            SheetFirst=Detail;
        if(SheetLast)
            SheetLast->Next=Detail;
        SheetLast=Detail;
        break;
    }
    return TRUE;
}

//Конструктор класса
CnclTask::SnclDetail::SnclDetail(const CnclDetail &_Detail,long _Count):CnclDetail(_Detail)
{
    Next=NULL;
    Prev=NULL;
    Count=_Count;
    OriginalCount=Count;
}

//Деструктор класса
CnclTask::SnclDetail::~SnclDetail()
{
    CnclDetail::Clear();
}

//загрузка задания из файла
BOOL CnclTask::Load(const WCHAR *FileName,CnclTransformView *DrawTransform,BOOL nest)
{
    BOOL Result=FALSE;
    WCHAR File[MAX_PATH];
    GetCurrentDirectory(sizeof(File),File);
    lstrcat(File,L"\\");
    lstrcat(File,FileName);
    if(!IsFile(File))
    {
        lstrcpy(File,FileName);
    }
    Clear(CLEAR_ALL);
    lstrcpy(TaskFile.FileName,File);
    if(lstrcmpi(&File[lstrlen(File)-3],L"KOL")==0)
    {
        Result=TaskFile.LoadKOL(File);
        LoadKOL(DrawTransform);
    }
    else if(lstrcmpi(&File[lstrlen(File)-3],L"DBS")==0)
    {
        CnclTaskFile::SnclGeomFile GeomFile(File,1,-1/*,TRUE*/);
        Result=LoadDBS(&GeomFile,DrawTransform);
    }
    else if(lstrcmpi(&File[lstrlen(File)-3],L"DXF")==0)
    {
        CnclTaskFile::SnclGeomFile GeomFile(File,1,1/*,TRUE*/);
        Result=LoadDXF(&GeomFile,DrawTransform,nest);
    }
    return Result;
}

//загрузка задания из файла типа KOL
void CnclTask::LoadKOL(CnclTransformView *DrawTransform)
{
    CnclTaskFile::SnclGeomFile *Current=TaskFile.First;
    while(Current)
    {
        if(lstrcmpi(&Current->FileName[lstrlen(Current->FileName)-3],L"DBS")==0)
            LoadDBS(Current,DrawTransform);
        else if(lstrcmpi(&Current->FileName[lstrlen(Current->FileName)-3],L"DXF")==0)
            LoadDXF(Current,DrawTransform,FALSE);
        Current=Current->Next;
    }
}

//загрузка задания из файла типа DBS
BOOL CnclTask::LoadDBS(CnclTaskFile::SnclGeomFile *GeomFile,CnclTransformView *DrawTransform)
{
    WCHAR buf[1024];
    lstrcpy(buf,GeomFile->FileName);
    CnclDBS DBS;
    DBS.Read(buf);
    return LoadGeom(&DBS,DrawTransform,GeomFile->Count,GeomFile->Flag,GeomFile->angle_step);
}

//загрузка задания из файла типа DXF
BOOL CnclTask::LoadDXF(CnclTaskFile::SnclGeomFile *GeomFile,CnclTransformView *DrawTransform,BOOL nest)
{
    WCHAR buf[1024];
    lstrcpy(buf,GeomFile->FileName);
    CnclDXF DXF;
    DXF.Read(buf,GeomFile->Flag>=0,nest);
    return LoadGeom(&DXF,DrawTransform,GeomFile->Count,GeomFile->Flag,GeomFile->angle_step);
}

//если Flag<0,
//то 1 деталь распознается как лист остальные как детали
BOOL CnclTask::LoadGeom(CnclCommonImport *Geom,CnclTransformView *DrawTransform,long Count_det,long Flag,int angle_step)
{
    if(!Geom->First)
    {
        return FALSE;
    }
    if(Flag<0)
        Flag=Geom->CountDetail()>1?LOAD_SHEET:LOAD_DETAIL;
    long ID=Count(DIFFERENT_DETAIL)+Count(DIFFERENT_SHEET)+1;
    CnclCommonImport::SnclDetail *Current=Geom->First;
    SnclDetail *Detail;
    while(Current)
    {
        Add(Current,Count_det,Flag);
        if(Flag==LOAD_DETAIL)
            Detail=DetailLast;
        else
            Detail=SheetLast;
        Detail->DrawMatrix=DrawTransform;
        Detail->angle_step=angle_step;
        Detail->Recalc();
        Detail->ID=ID;
        Flag=LOAD_DETAIL;
        ID++;
        Current=Current->Next;
    }
    return TRUE;
}

//габариты деталей входящих в задание
CnclRect CnclTask::Rect(BOOL Draw,CnclTransformView *TransformView)
{
    CnclRect TaskRect;
    SnclDetail *Current=DetailFirst;
    while(Current)
    {
        TaskRect=TaskRect&Current->Rect(Draw,TransformView);
        Current=Current->Next;
    }
    Current=SheetFirst;
    while(Current)
    {
        TaskRect=TaskRect&Current->Rect(Draw,TransformView);
        Current=Current->Next;
    }
    return TaskRect;
}

//вывод количества деталей в задании
long CnclTask::Count(long Flag)
{
    SnclDetail *Detail;
    long count=0;
    switch(Flag)
    {
    case ALL_DETAIL:
        Detail=DetailFirst;
        while(Detail)
        {
            count+=Detail->OriginalCount;
            Detail=Detail->Next;
        }
        return count;
    case DIFFERENT_DETAIL:
        Detail=DetailFirst;
        while(Detail)
        {
            count++;
            Detail=Detail->Next;
        }
        return count;
    case ALL_SHEET:
        Detail=SheetFirst;
        while(Detail)
        {
            count+=Detail->OriginalCount;
            Detail=Detail->Next;
        }
        return count;
    case DIFFERENT_SHEET:
        Detail=SheetFirst;
        while(Detail)
        {
            count++;
            Detail=Detail->Next;
        }
        return count;

    case NEST:
    case NEST_DETAIL:
    {
        long res=TaskBest->Count(Flag);
        return res;
    }
    case BLOCK:
    {
        CnclBlock *Block=BlockFirst;
        while(Block)
        {
            count++;
            Block=Block->Next;
        }
        return count;
    }
        /*
    case SINGLE_BLOCK:
    {
        CnclBlock *Block=BlockSingleFirst;
        while(Block)
        {
            count++;
            Block=Block->Next;
        }
        return count;
    }*/
    }
    return 0;
}

//удаление всех данных из задания
void CnclTask::Clear(long Flag)
{
    SnclDetail *Current;
    if(Flag==CLEAR_ALL)
    {
        Current=SheetFirst;
        while(Current)
        {
            Current=Current->Next;
            delete SheetFirst;
            SheetFirst=Current;
        }
        SheetLast=NULL;
        SheetFirst=NULL;
        Current=DetailFirst;
        while(Current)
        {
            Current=Current->Next;
            delete DetailFirst;
            DetailFirst=Current;
        }
        DetailLast=NULL;
        DetailFirst=NULL;
        TaskFile.Clear();
    }
    if(Flag==CLEAR_ALL || Flag==CLEAR_NEST)
    {
        delete Task;
        Task=NULL;
        delete TaskBest;
        TaskBest=NULL;
    }
    if(Flag==CLEAR_ALL || Flag==CLEAR_BLOCK)
    {
        CnclBlock *Current=BlockFirst;
        while(Current)
        {
            Current=Current->Next;
            delete BlockFirst;
            BlockFirst=Current;
        }
        BlockLast=NULL;
        BlockFirst=NULL;
        NestBlockOrder.Clear();
    }
    if(Flag==CLEAR_COUNT)
    {
        SnclDetail *Current=DetailFirst;
        while(Current)
        {
            Current->Count=Current->OriginalCount;
            Current=Current->Next;
        }
        Current=SheetFirst;
        while(Current)
        {
            Current->Count=Current->OriginalCount;
            Current=Current->Next;
        }
        CnclBlock *CurBlock=BlockFirst;
        CnclBlock::SnclDetLink *CurLink;
        while(CurBlock)
        {
            CurLink=CurBlock->First;
            while(CurLink)
            {
                CurLink->Nested=FALSE;
                CurLink=CurLink->Next;
            }
            CurBlock=CurBlock->Next;
        }
        /*
        CurBlock=BlockSingleFirst;
        while(CurBlock)
        {
            CurLink=CurBlock->First;
            while(CurLink)
            {
                CurLink->Nested=FALSE;
                CurLink=CurLink->Next;
            }
            CurBlock=CurBlock->Next;
        }
        */
    }
}

CnclTask::CnclNestedTask::SnclNest::SnclNest()
{
    //	Nest=NULL;
    Next=NULL;
    Prev=NULL;
}

//Деструктор класса
CnclTask::CnclNestedTask::SnclNest::~SnclNest()
{
    CnclNest::Clear();
}

//порядковый номер раскроя
long CnclTask::CnclNestedTask::SnclNest::Index()
{
    long i=0;
    SnclNest *Current=this;
    while(Current)
    {
        i++;
        Current=Current->Prev;
    }
    return i;
}

//создание нового раскроя
CnclTask::CnclNestedTask::SnclNest* CnclTask::CnclNestedTask::Add()
{
    SnclNest *Nest=new SnclNest;
    if(!Nest)
    {
        return NULL;
    }
    Nest->Prev=NestLast;
    if(!NestFirst)
    {
        NestFirst=Nest;
        NestCurrent=Nest;
    }
    if(NestLast)
        NestLast->Next=Nest;
    NestLast=Nest;
    if(!NestCurrent)
        NestCurrent=Nest;
    return Nest;
}

/*эта функция может вызываться только из функции раскроя*/
void CnclTask::CnclNestedTask::Delete(CnclTask::CnclNestedTask::SnclNest *Nest)
{
    if(Nest==NestCurrent)
    {
        if(Nest->Prev)
            NestCurrent=Nest->Prev;
        else
            NestCurrent=Nest->Next;
    }
    if(Nest->Next)
    {
        Nest->Next->Prev=Nest->Prev;
    }
    else
    {
        NestLast=Nest->Prev;
        if(NestLast)
            NestLast->Next=NULL;
    }
    if(Nest->Prev)
    {
        Nest->Prev->Next=Nest->Next;
    }
    else
    {
        NestFirst=Nest->Next;
        if(NestFirst)
            NestFirst->Prev=NULL;
    }
    if(Nest==NestFirst && Nest==NestLast)
        Clear();
    else
        delete Nest;
}

void CnclTask::CnclNestedTask::Clear()
{
    SnclNest *CurrentNest=NestFirst;
    while(CurrentNest)
    {
        CurrentNest=CurrentNest->Next;
        delete NestFirst;
        NestFirst=CurrentNest;
    }
    NestFirst=NULL;
    NestLast=NULL;
    NestCurrent=NULL;
}


void CnclTask::Check()
{
    SnclDetail *Current=DetailFirst;
    while(Current)
    {
        Current->Check();
        Current=Current->Next;
    }
    Current=SheetFirst;
    while(Current)
    {
        Current->Check();
        Current=Current->Next;
    }
}

void CnclTask::CnclNestedTask::Nest(CnclTask::SnclDetail *SheetFirst,
                                    CnclTask::CnclNestOrder::SnclNestOrder *NestBlockOrder,
                                    DWORD plug_id,HWND hwndStatusBar/*,BOOL *Stop*/)
{
    SnclNest *CurrentNest;
    if(!SheetFirst)
        return;
    Calc();
    CurrentNest=NestFirst;
    double nest_detail=0;
    while(CurrentNest)
    {


        CnclNest::SnclDetail *CurrentDetail=CurrentNest->First;
        //		while(CurrentDetail)
        {
            CnclNest::SnclDetail *CurDetail;
            CnclNest::SnclDetail *FirstFindDetail;	//первая найденная деталь
            double CurrentID;
            long count=0;
            do{
                FirstFindDetail=NULL;
                count=0;
                CurrentID=-1;
                CurDetail=CurrentNest->First;
                while(CurDetail)
                {
                    if((CurDetail->ID==CurrentID || CurrentID<0) && CurDetail->ID>0)
                    {
                        CurrentID=CurDetail->ID;
                        CurDetail->ID=-CurDetail->ID;
                        FirstFindDetail=CurDetail;
                        count++;
                    }
                    CurDetail=CurDetail->Next;
                }
                if(FirstFindDetail)
                {
                    double cur_area=FirstFindDetail->Area();
                    nest_detail+=count;
                }
            }while(count>0);
        }
        CurrentDetail=CurrentNest->First;
        while(CurrentDetail)
        {
            CurrentDetail->ID=ABS(CurrentDetail->ID);
            CurrentDetail=CurrentDetail->Next;
        }
        CurrentNest=CurrentNest->Next;
    }
    nest_detail=0;
    CurrentNest=NestFirst;
}

void CnclTask::CnclNestedTask::Calc()
{
    SnclNest *Current=NestFirst;
    double i=0;
    NestFactor=0;
    CutNestFactor=0;
    double sheetArea=0;
    double detailArea=0;
    double cutArea=0;
    double cutLastSheetArea=0;
    while(Current)
    {
        if(Current->Count()==0)
        {
            SnclNest *newCurrent=Current->Next;
            Delete(Current);
            Current=newCurrent;
        }
        else
        {
            Current->Calc();
            //			NestFactor+=Current->NestFactor;
            //			CutNestFactor+=Current->CutNestFactor;
            //			i++;
            sheetArea+=Current->Sheet.Area();
            detailArea+=Current->Area();
            cutArea+=Current->NestRect.Area();
            if(Current->Next)
                cutLastSheetArea+=Current->Sheet.Area();
            else
                cutLastSheetArea+=Current->NestRect.Area();
            Current=Current->Next;
        }
    }
    if(sheetArea*cutArea>0)
    {
        NestFactor=detailArea/sheetArea;
        CutNestFactor=detailArea/cutArea;
        CutLastSheetNestFactor=detailArea/cutLastSheetArea;
    }
}

/*
CnclTask::SnclDetail *CnclTask::FindDetail(long ID)
{
    SnclDetail *Current;
    long count=1;
    for(int i=0;i<2;i++)
    {
        if(i==0)
            Current=SheetFirst;
        else
            Current=DetailFirst;
        while(Current)
        {
            if(Current->ID==ID)
                return Current;
            Current=Current->Next;
        }
        count++;
    }
    return NULL;
}
*/

//сортировка детелей в задании
/*
void CnclTask::Sort()
{
    SnclDetail *MainLoop=DetailLast;
    SnclDetail *Loop;
    SnclDetail *buf_loop;
    SnclDetail *buf_next;
    while(MainLoop)
    {
        Loop=DetailFirst;
        while(Loop!=MainLoop)
        {
            if(Loop->Rect(FALSE,NULL).Area()<Loop->Next->Rect(FALSE,NULL).Area())
            {
                //меняем местами Loop и Loop->Next;
                buf_loop=Loop;
                buf_next=Loop->Next;

                buf_next->Prev=buf_loop->Prev;
                buf_loop->Next=buf_next->Next;

                if(buf_loop->Prev)
                    buf_loop->Prev->Next=buf_next;

                if(buf_next->Next)
                    buf_next->Next->Prev=buf_loop;

                buf_loop->Prev=buf_next;
                buf_next->Next=buf_loop;

                if(!buf_loop->Next)
                    DetailLast=buf_loop;
                if(!buf_next->Prev)
                    DetailFirst=buf_next;
                if(MainLoop==buf_next)
                    MainLoop=buf_loop;
            }
            else
            {
                Loop=Loop->Next;
            }
        }
        MainLoop=MainLoop->Prev;
    }
}
*/

CnclTask& CnclTask::operator = (const CnclTask &Task)
{
    Load(Task.TaskFile.FileName,&DrawTransform,FALSE);
    return *this;
}

/*
//добавляет раскрой из DBS файла
void CnclTask::AddDBSNestResult(WCHAR *FileName)
{
    CDoc Doc;
    if(!Doc.File.Read(FileName))
        return;
    Doc.ReadDetail(LOAD_DETAIL,1);
    Doc.Detail.Rename(1);
    if(Doc.Detail.GetCountOfDetail(TRUE)==0)
        return;

    SnclNest *Nest=Add();
    CListOfDetail *Detail=Doc.Detail.First;
    while(Detail)
    {
        if(Detail==Doc.Detail.First)
        {

        }
        else
        {
        }
        Detail=Detail->Next;
    }

    Doc.Detail.Clear();
}
*/

CnclTask::CnclNestOrder::CnclNestOrder()
{
    First=NULL;
    Last=NULL;
}

CnclTask::CnclNestOrder::~CnclNestOrder()
{
    Clear();
}

BOOL CnclTask::CnclNestOrder::Add(CnclTask::CnclNestOrder::SnclNestOrder *NestOrder)
{
    SnclNestOrder *Current=First;
    long i;
    while(Current)	//проверяем есть ли уже такая последовательность
    {
        for(i=0;i<NestOrder->Count;i++)	//поиск несоответствия в 2-х послудовательностях
            if((*Current->IDs[i])!=(*NestOrder->IDs[i]))
                break;
        if(i==NestOrder->Count) //если несоответствий не найдено, то выходим из цикла
            break;
        Current=Current->Next;
    }
    //end_loop:;
    if(Current)
        return FALSE;
    NestOrder->Prev=Last;
    if(!First)
        First=NestOrder;
    if(Last)
        Last->Next=NestOrder;
    Last=NestOrder;
    return TRUE;
}

void CnclTask::CnclNestOrder::Clear()
{
    SnclNestOrder *Current=First;
    while(Current)
    {
        Current=Current->Next;
        delete First;
        First=Current;
    }
    Last=NULL;
    First=NULL;
}

//void CnclTask::CnclNestOrder::OrderFromTask(CnclTask *Task,DWORD TypeBlock)
void CnclTask::CnclNestOrder::OrderFromTask(CnclTask *Task)
{
    SnclNestOrder *NestOrder=new SnclNestOrder;
    CnclTask::CnclBlock* Current;
    NestOrder->Count=Task->Count(BLOCK);
    //	NestOrder->Count=Task->Count(TypeBlock);
    if(NestOrder->Count>0)
    {
        //		Current=TypeBlock==BLOCK?Task->BlockFirst:Task->BlockSingleFirst;
        Current=Task->BlockFirst;
        NestOrder->IDs=new CnclTask::CnclBlock* [NestOrder->Count];
        long i=0;
        while(Current)
        {
            NestOrder->IDs[i]=Current;
            i++;
            Current=Current->Next;
        }
    }
    else
    {
        NestOrder->IDs=NULL;
    }
    Add(NestOrder);
}

CnclTask::CnclNestOrder::SnclNestOrder::SnclNestOrder()
{
    IDs=NULL;
    Count=0;
    Prev=NULL;
    Next=NULL;
}

CnclTask::CnclNestOrder::SnclNestOrder::~SnclNestOrder()
{
    delete [] IDs;
}

BOOL CnclTask::CnclNestOrder::BuildNewOrder()
{
#define MAX_ORDER_IRERATION	10000
    if(!Last)
        return FALSE;
    SnclNestOrder *NestOrder=new SnclNestOrder;
    NestOrder->Count=Last->Count;
    if(NestOrder->Count>0)
    {
        NestOrder->IDs=new CnclTask::CnclBlock* [NestOrder->Count];
        CopyMemory(NestOrder->IDs,Last->IDs,sizeof(CnclTask::CnclBlock*)*Last->Count);
        long iter=0;
        do
        {
            iter++;
            long iteration=long(nclRand()*Last->Count);
            long a,b;
            CnclTask::CnclBlock *buf;
            for(int i=0;i<iteration;i++)
            {
                a=long(nclRand()*Last->Count);
                b=long(nclRand()*Last->Count);
                buf=NestOrder->IDs[b];
                NestOrder->IDs[b]=NestOrder->IDs[a];
                NestOrder->IDs[a]=buf;
            }
        }while(!Add(NestOrder) && iter<MAX_ORDER_IRERATION);
        if(!(iter<MAX_ORDER_IRERATION))
            delete NestOrder;
        return (iter<MAX_ORDER_IRERATION);
    }
    else
    {
        NestOrder->IDs=NULL;
        return FALSE;
    }
}

void CnclTask::CnclNestOrder::SnclNestOrder::Restore(long k, long n)
{
    //  процедура просеивания следующего элемента
    //  До процедуры: a[k+1]...a[n]  - пирамида
    //  После:  a[k]...a[n]  - пирамида   T new_elem;
    long child;
    CnclTask::CnclBlock* new_elem=IDs[k];
    while(k<=n/2)
    {
        // пока у a[k] есть дети
        child=2*k;
        //  выбираем большего сына
        switch(settings.value("Sorting",1).toInt())
        {
        case 1:	//по габаритам
            if(child<n && (IDs[child]->RealRect.Area()<IDs[child+1]->RealRect.Area() ||
                           (IDs[child]->RealRect.Area()==IDs[child+1]->RealRect.Area() &&
                            IDs[child]->Count(TRUE)<IDs[child+1]->Count(TRUE))))
                child++;
            if(new_elem->RealRect.Area()>=IDs[child]->RealRect.Area() ||
                    (new_elem->RealRect.Area()==IDs[child]->RealRect.Area() &&
                     new_elem->Count(TRUE)>=IDs[child]->Count(TRUE)))
                goto end_loop;
            break;
        case 2:	//по площади
            if(child<n && (IDs[child]->Area()<IDs[child+1]->Area() ||
                           (IDs[child]->Area()==IDs[child+1]->Area() &&
                            IDs[child]->Count(TRUE)<IDs[child+1]->Count(TRUE))))
                child++;
            if(new_elem->Area()>=IDs[child]->Area() ||
                    (new_elem->Area()==IDs[child]->Area() &&
                     new_elem->Count(TRUE)>=IDs[child]->Count(TRUE)))
                goto end_loop;
            break;
        default:	//по периметру
            if(child<n && (IDs[child]->Len()<IDs[child+1]->Len() ||
                           (IDs[child]->Len()==IDs[child+1]->Len() &&
                            IDs[child]->Count(TRUE)<IDs[child+1]->Count(TRUE))))
                child++;
            if(new_elem->Len()>=IDs[child]->Len() ||
                    (new_elem->Len()==IDs[child]->Len() &&
                     new_elem->Count(TRUE)>=IDs[child]->Count(TRUE)))
                goto end_loop;
            break;
        }
        // иначе
        IDs[k]=IDs[child];
        // переносим сына наверх
        k=child;
    }
end_loop:;
    IDs[k]=new_elem;
}

void CnclTask::CnclNestOrder::SnclNestOrder::SortOrder()
{
    long i;
    CnclTask::CnclBlock* buf;
    // строим пирамиду
    for(i=Count/2-1; i>=0;i--)
        Restore(i,Count-1);
    // теперь a[0]...a[size-1] пирамида
    for(i=Count-1; i > 0; i--)
    {
        // меняем первый с последним
        buf=IDs[i];
        IDs[i]=IDs[0];
        IDs[0]=buf;
        // восстанавливаем пирамидальность a[0]...a[i-1]
        Restore(0, i-1);
    }
}

void CnclTask::CnclNestOrder::SnclNestOrder::Nesting(CnclTask::CnclBlock::SnclDetLink *Detail)
{
    for(long i=0;i<Count;i++)
    {
        if(Detail->Double)
        {
            CnclBlock::SnclDetLink *Current=IDs[i]->First;
            while(Current)
            {
                if(!Current->Double)
                {
                    if(Current->ID==Detail->ID && Current->Nested==FALSE)
                    {
                        Current->Nested=TRUE;
                        return;
                    }
                }
                Current=Current->Next;
            }
        }
        else
        {
            if(IDs[i]->Count(TRUE)==1)
            {
                if(IDs[i]->First->ID==Detail->ID && IDs[i]->First->Nested==FALSE)
                {
                    IDs[i]->First->Nested=TRUE;
                    return;
                }
            }
        }
    }
}

/*
void CnclTask::CnclNestOrder::TaskFromOrder(CnclTask *Task)
{
    if(!Last)
        return;
    CnclTask::CnclBlock *LastDet=NULL;
    for(long i=0;i<Last->Count;i++)
    {
        if(!LastDet)
        {
            Task->DetailFirst=Last->IDs[i];
            Task->DetailLast=Last->IDs[i];
            Task->DetailFirst->Prev=NULL;
            Task->DetailLast->Next=NULL;
        }
        else
        {
            Task->DetailLast->Next=Last->IDs[i];
            Last->IDs[i]->Prev=Task->DetailLast;
            Task->DetailLast=Last->IDs[i];
            Task->DetailLast->Next=NULL;
        }
        LastDet=Last->IDs[i];
    }
}
*/
long CnclTask::CnclNestedTask::Count(long Flag)
{
    long count=0;
    switch(Flag)
    {
    case NEST:
    {
        SnclNest *Nest=NestFirst;
        while(Nest)
        {
            count++;
            Nest=Nest->Next;
        }
        break;
    }
    case NEST_DETAIL:
    {
        SnclNest *Nest=NestFirst;
        while(Nest)
        {
            count+=Nest->Count();
            Nest=Nest->Next;
        }
        break;
    }
    }
    return count;
}

CnclTask::CnclNestedTask::CnclNestedTask()
{
    NestCurrent=NULL;
    NestFirst=NULL;
    NestLast=NULL;
    NestFactor=0;
    CutNestFactor=0;
}

CnclTask::CnclNestedTask::~CnclNestedTask()
{
    Clear();
}

void CnclTask::ChooseNest()
{
    //			Err<<"ChooseNest EnterCritical"<<endl;
    //	Application->EnterCritical();
    BOOL Change=FALSE;
    if(!TaskBest)
    {
        Change=TRUE;
    }
    else
    {
        //		Task->Calc();
        //		TaskBest->Calc();
        //если все детали раскроены как в Task, так и в TaskBest
        if(Task->Count(NEST_DETAIL)==Count(ALL_DETAIL) &&
                TaskBest->Count(NEST_DETAIL)==Count(ALL_DETAIL))
        {
            if((Round(Task->NestFactor,6)-Round(TaskBest->NestFactor,6))>Epsilon)
                Change=TRUE;
            else
            {
                //если общий коэффициент раскоя одиноков
                if(ABS(Round(Task->NestFactor,6)-Round(TaskBest->NestFactor,6))<Epsilon)
                {
                    //если площадь габаритов деталей на последнем листе меньше в Task
                    if(Task->NestLast->NestRect.Area()<TaskBest->NestLast->NestRect.Area())
                        Change=TRUE;
                    //если площадь габаритов деталей на последнем листе равны в Task
                    else if(Task->NestLast->NestRect.Area()==TaskBest->NestLast->NestRect.Area())
                    {
                        if(Task->NestLast->NestFactor<TaskBest->NestLast->NestFactor)
                            Change=TRUE;
                    }
                }
            }
        }
        else
        {
            //если все детали раскроены только в Task
            if(Task->Count(NEST_DETAIL)==Count(ALL_DETAIL))
            {
                Change=TRUE;
            }
            else
            {
                if(TaskBest->Count(NEST_DETAIL)!=Count(ALL_DETAIL))
                    //если детали не раскроены полностью в обоих случаях
                    if(Round(Task->NestFactor,6)>Round(TaskBest->NestFactor,6))
                        Change=TRUE;
            }
        }
    }
    if(Change)
    {
        if(TaskBest)
            delete TaskBest;
        TaskBest=Task;
        Task=NULL;
        //Application->LeaveCritical();
        //Application->WndTask.Update();
        //		SendMessage(Application->WndMain.hWnd, WM_COMMAND,IDM_ZOOM_ALL,0);
        //			Err<<"ChooseNest EnterCritical"<<endl;
        //Application->EnterCritical();
    }
    //	Application->LeaveCritical();
}

CnclTask::CnclBlock::CnclBlock()
{
    Next=NULL;
    Prev=NULL;
    Last=NULL;
    First=NULL;
    angle_step=0;
}

CnclTask::CnclBlock::~CnclBlock()
{
    Clear();
}

void CnclTask::CnclBlock::Clear()
{
    SnclDetLink *Current=First;
    while(Current)
    {
        Current=Current->Next;
        delete First;
        First=Current;
    }
    Last=NULL;
    First=NULL;
}

CnclTask::CnclBlock::SnclDetLink::SnclDetLink()
{
    Next=NULL;
    Prev=NULL;
    Nested=FALSE;
    Double=FALSE;
}

void CnclTask::CnclBlock::Add(CnclTask::CnclBlock::SnclDetLink* Detail)
{
    if(Detail->ID->angle_step>0)
        angle_step=Detail->ID->angle_step;
    Detail->Prev=Last;
    if(!First)
        First=Detail;
    if(Last)
        Last->Next=Detail;
    Last=Detail;
    Recalc();
}

BOOL CnclTask::CnclBlock::Add(CnclTask::SnclDetail *Detail)
{
    SnclDetLink *newLink=new SnclDetLink;
    if(!newLink)
        return FALSE;
    newLink->ID=Detail;
    if(TestDetail(newLink))
        return TRUE;
    if(newLink->ID->angle_step>0)
        angle_step=newLink->ID->angle_step;
    delete newLink;
    return FALSE;
}

void CnclTask::CnclBlock::Recalc()
{
    RealRect=Rect(FALSE,NULL);
    SnclDetLink *Current=First;
    while(Current)
    {
        Current->ID->Recalc();
        Current=Current->Next;
    }
}

long CnclTask::CnclBlock::Count(BOOL All)
{
    long count=0;
    SnclDetLink *Current=First;
    while(Current)
    {
        if(!Current->Nested || All)
        {
            count++;
        }
        Current=Current->Next;
    }
    return count;
}

CnclRect CnclTask::CnclBlock::Rect(BOOL Draw,CnclTransformView *TransformView)
{
    CnclRect RectBlock;
    SnclDetLink *Current=First;
    while(Current)
    {
        if(!Current->Nested)
        {
            CnclMatrix buf=Current->ID->Matrix;
            Current->ID->Matrix=Current->Matrix*Current->ID->Matrix;
            Current->ID->Recalc();
            RectBlock=RectBlock & Current->ID->Rect(Draw,TransformView);
            Current->ID->Matrix=buf;
            Current->ID->Recalc();
        }
        Current=Current->Next;
    }
    return RectBlock;
}

//вычисление площади блока
double CnclTask::CnclBlock::Area()
{
    double block_area=0;
    SnclDetLink *Current=First;
    while(Current)
    {
        if(!Current->Nested)
        {
            CnclMatrix buf=Current->ID->Matrix;
            Current->ID->Matrix=Current->Matrix*Current->ID->Matrix;
            Current->ID->Recalc();
            block_area+=Current->ID->Area();
            Current->ID->Matrix=buf;
            Current->ID->Recalc();
        }
        Current=Current->Next;
    }
    return block_area;
}

//вычисление периметра блока
double CnclTask::CnclBlock::Len()
{
    double block_len=0;
    SnclDetLink *Current=First;
    while(Current)
    {
        if(!Current->Nested)
        {
            CnclMatrix buf=Current->ID->Matrix;
            Current->ID->Matrix=Current->Matrix*Current->ID->Matrix;
            Current->ID->Recalc();
            block_len+=Current->ID->Len();
            Current->ID->Matrix=buf;
            Current->ID->Recalc();
        }
        Current=Current->Next;
    }
    return block_len;
}

void CnclTask::Add(CnclTask::CnclBlock *Block)
//void CnclTask::Add(CnclTask::CnclBlock *Block,BOOL Single)
{
    /*	if(Single)
    {
        Block->Prev=BlockSingleLast;
        if(!BlockSingleFirst)
            BlockSingleFirst=Block;
        if(BlockSingleLast)
            BlockSingleLast->Next=Block;
        BlockSingleLast=Block;
    }
    else
    {*/
    Block->Prev=BlockLast;
    if(!BlockFirst)
        BlockFirst=Block;
    if(BlockLast)
        BlockLast->Next=Block;
    BlockLast=Block;
    //	}
}

/*
void CnclTask::Delete(SnclDetail *Detail)
{
    CnclBlock *CurrentBlock=BlockSingleFirst;
    CnclBlock::SnclDetLink *CurrentLink;
    while(CurrentBlock)
    {
        CurrentLink=CurrentBlock->First;
        while(CurrentLink)
        {
            if(CurrentLink->ID==Detail)
                goto find;
            CurrentLink=CurrentLink->Next;
        }
        CurrentBlock=CurrentBlock->Next;
    }
find:
    if(CurrentBlock)
    {
        if(CurrentBlock->Next)
        {
            CurrentBlock->Next->Prev=CurrentBlock->Prev;
        }
        else
        {
            BlockSingleLast=CurrentBlock->Prev;
            if(BlockSingleLast)
                BlockSingleLast->Next=NULL;
        }
        if(CurrentBlock->Prev)
        {
            CurrentBlock->Prev->Next=CurrentBlock->Next;
        }
        else
        {
            BlockSingleFirst=CurrentBlock->Next;
            if(BlockSingleFirst)
                BlockSingleFirst->Prev=NULL;
        }
        if(CurrentBlock==BlockSingleLast && CurrentBlock==BlockSingleFirst)
        {
            BlockSingleLast=NULL;
            BlockSingleFirst=NULL;
        }
        delete CurrentBlock;
    }
}
*/

void CnclTask::BuildBlock()
{
    //	PostMessage(Application->WndMain.hProgress, PBM_SETPOS, (WPARAM) 0, 0);
    //	PostMessage(Application->WndMain.hProgress, PBM_SETRANGE, 0, MAKELPARAM(0,Application->Task.Count(ALL_DETAIL)));
    //	PostMessage(Application->WndMain.hwndStatusBar, SB_SETTEXT, 1 | SBT_NOBORDERS, (LPARAM)L"Нажмите Esc для отмены");
    //	ShowWindow(Application->WndMain.hProgress,SW_SHOW);
    //	PostMessage(Application->WndMain.hwndStatusBar, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)L"Блоки:");
    SnclDetail *Current=DetailFirst;
    long i=0;
    /*
    while(Current)
    {
        Current->Matrix.Identity();
        while(Current->Count>0)
        {
            CnclBlock *Block=new CnclBlock;
            Add(Block,FALSE);
            if(Block->Add(Current))
            {
                Current->Count--;
                i++;
            }
            else
                break;
            if(Application->Settings.build_block && Current->Count>0)
            {
                if(Block->Add(Current))
                {
                    Current->Count--;
                    i++;
                }
            }
            do
            {
                if(Block->Count()>1) //если в блоке больше чем 1 деталь, то разбиваем его
                        //и добавляем детали к списку BlockSingle
                {
                    CnclBlock::SnclDetLink *CurrentLink=Block->First;
                    while(CurrentLink)
                    {
                        CnclBlock *SingleBlock=new CnclBlock;
                        Add(SingleBlock,TRUE);
                        SingleBlock->Add(CurrentLink->ID);
                        CurrentLink=CurrentLink->Next;
                    }
                }
                Block=NULL;
                //дублируем последний созданный блок если это возможно
                {
                    BOOL Double=TRUE;
                    CnclBlock::SnclDetLink *CurrentLink=BlockLast->First;
                    while(CurrentLink) //проверяем возможность дублирования
                    {
                        CurrentLink->ID->Count--;
                        if(CurrentLink->ID->Count<0)
                            Double=FALSE;
                        CurrentLink=CurrentLink->Next;
                    }
                    if(!Double) //если дублирование невозможно
                                //восстанавливаем кол-во деталей
                    {
                        CurrentLink=BlockLast->First;
                        while(CurrentLink)
                        {
                            CurrentLink->ID->Count++;
                            CurrentLink=CurrentLink->Next;
                        }
                    }
                    else //дублируем последний блок
                    {
                        Block=new CnclBlock;
                        CurrentLink=BlockLast->First;
                        while(CurrentLink)
                        {
                            CnclBlock::SnclDetLink *newLink=new CnclBlock::SnclDetLink;
                            newLink->ID=CurrentLink->ID;
                            newLink->Matrix=CurrentLink->Matrix;
                            Block->Add(newLink);
                            i++;
                            CurrentLink=CurrentLink->Next;
                        }
                        Add(Block,FALSE);
                    }
                }
                PostMessage(Application->hProgress, PBM_SETPOS, i, 0);
            }while(Block);
        }
        Current=Current->Next;
    }
    */
    while(Current)
    {
        Current->Matrix.Identity();
        while(Current->Count>0)
        {
            CnclBlock *Block=new CnclBlock;
            Add(Block);
            int step_angle=0;
            if(Block->Add(Current))
            {
                Current->Count--;
                i++;
            }
            else
                break;
            if(settings.value("BuildBlock",0).toInt()!=1 && Current->Count>0)
            {
                if(Block->Add(Current))
                {
                    Current->Count--;
                    i++;
                }
            }
            if(settings.value("RectOptimisation",false).toBool())
                Block->RectOptimize();
            do
            {
                //				if(Block->Count(TRUE)>1) //если в блоке больше чем 1 деталь, то разбиваем его
                //				{
                //					CnclBlock::SnclDetLink *CurrentLink=Block->First;
                //					while(CurrentLink)
                //					{
                //						CnclBlock *SingleBlock=new CnclBlock;
                //						Add(SingleBlock);
                //						SingleBlock->Add(CurrentLink->ID);
                //						SingleBlock->Last->Double=TRUE; //устанавливаем флаг дублирования блока
                //						CurrentLink=CurrentLink->Next;
                //					}
                //				}
                if(Block->Count(TRUE)>1) //если в блоке больше чем 1 деталь, то разбиваем его
                {
                    CnclBlock::SnclDetLink *CurrentLink=Block->First;
                    while(CurrentLink)
                    {
                        CnclBlock *SingleBlock=new CnclBlock;
                        Add(SingleBlock);
                        SingleBlock->Add(CurrentLink->ID);
                        SingleBlock->Last->Double=TRUE; //устанавливаем флаг дублирования блока
                        if(settings.value("RectOptimisation",false).toBool())
                            SingleBlock->RectOptimize();
                        CurrentLink=CurrentLink->Next;
                    }
                }
                //дублируем последний созданный блок если это возможно
                {
                    BOOL Double=TRUE;
                    CnclBlock::SnclDetLink *CurrentLink=Block->First;
                    while(CurrentLink) //проверяем возможность дублирования
                    {
                        CurrentLink->ID->Count--;
                        if(CurrentLink->ID->Count<0)
                            Double=FALSE;
                        CurrentLink=CurrentLink->Next;
                    }
                    if(!Double) //если дублирование невозможно
                        //восстанавливаем кол-во деталей
                    {
                        CurrentLink=Block->First;
                        while(CurrentLink)
                        {
                            CurrentLink->ID->Count++;
                            CurrentLink=CurrentLink->Next;
                        }
                        Block=NULL;
                    }
                    else //дублируем последний блок
                    {
                        CurrentLink=Block->First;
                        Block=new CnclBlock;
                        while(CurrentLink)
                        {
                            CnclBlock::SnclDetLink *newLink=new CnclBlock::SnclDetLink;
                            newLink->ID=CurrentLink->ID;
                            newLink->Matrix=CurrentLink->Matrix;
                            Block->Add(newLink);
                            i++;
                            CurrentLink=CurrentLink->Next;
                        }
                        //						Block=newBlock;
                        Add(Block);
                    }
                }
                //				PostMessage(Application->WndMain.hProgress, PBM_SETPOS, i, 0);
            }while(Block);
        }
        Current=Current->Next;
    }
    Current=DetailFirst;
    while(Current)
    {
        Current->Count=Current->OriginalCount;
        Current=Current->Next;
    }
    CnclBlock *CurrentBlock=BlockFirst;
    while(CurrentBlock)
    {
        //		if(CurrentBlock->Count()==1)
        //			Delete(CurrentBlock->First->ID);
        CurrentBlock->Recalc();
        CurrentBlock=CurrentBlock->Next;
    }
    /*
    CurrentBlock=BlockSingleFirst;
    while(CurrentBlock)
    {
        CurrentBlock->Recalc();
        CurrentBlock=CurrentBlock->Next;
    }
    */
    //#ifndef _USRDLL	//Этот блок не выполняется для DLL
    //потому что в c# происходит зависание, если используется
    //функция nclWait
    //	ShowWindow(Application->WndMain.hProgress,SW_HIDE);
    //#endif
    //	PostMessage(Application->WndMain.hwndStatusBar, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)L"Готово");
    //	PostMessage(Application->WndMain.hwndStatusBar, SB_SETTEXT, 1 | SBT_NOBORDERS, (LPARAM)L"");
}

CnclRect CnclTask::CnclBlock::EcvRect(BOOL Draw,CnclTransformView *TransformView)
{
    CnclRect RectBlock;
    SnclDetLink *Current=First;
    while(Current)
    {
        if(!Current->Nested)
        {
            CnclMatrix buf=Current->ID->Ecv->Matrix;
            Current->ID->Ecv->Matrix=Current->Matrix*Current->ID->Ecv->Matrix;
            Current->ID->Ecv->Recalc();
            RectBlock=RectBlock & Current->ID->Ecv->Rect(Draw,TransformView);
            Current->ID->Ecv->Matrix=buf;
            Current->ID->Ecv->Recalc();
        }
        Current=Current->Next;
    }
    return RectBlock;
}

void CnclTask::CnclBlock::EcvRecalc()
{
    SnclDetLink *Current=First;
    if(Current)
    {
        Current->ID->Ecv->Recalc();
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::RectOptimize()
{
    //	return;
#define STEP_ANGLE 360
    SnclDetLink *Current;
    double BestArea=EcvRect(FALSE,NULL).Area();
    double BestAngle=0;
    double curArea;
    long i=0;
    CnclMatrix *oldMatrix=new CnclMatrix[Count(TRUE)];
    Current=First;
    while(Current)	//запоминием старые матрицы преобразования
    {
        oldMatrix[i]=Current->Matrix;
        i++;
        Current=Current->Next;
    }
    EcvMatrixIdentity();
    for(i=1;i<STEP_ANGLE;i++)
    {
        Current=First;
        while(Current)
        {
            Current->Matrix.Rotate(PI/2/STEP_ANGLE);
            Current=Current->Next;
        }

        curArea=EcvRect(FALSE,NULL).Area();
        if(curArea<BestArea)
        {
            BestArea=curArea;
            BestAngle=PI/2/STEP_ANGLE*i;
        }
    }
    Current=First;
    i=0;
    while(Current)	//восстанавливаем матрицы перобразования
    {
        Current->Matrix=oldMatrix[i];
        Current->Matrix.Rotate(BestAngle);
        i++;
        Current=Current->Next;
    }
    delete [] oldMatrix;
}

void CnclTask::CnclBlock::EcvMatrixIdentity()
{
    SnclDetLink *Current=First;
    while(Current)
    {
        Current->ID->Ecv->Matrix.Identity();
        //		Current->ID->Ecv->Matrix=Current->Matrix;
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::EcvMatrixMirror(int Flag)
{
    SnclDetLink *Current=First;
    if(Current)
    {
        Current->ID->Ecv->Matrix.Mirror(Flag);
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::EcvMatrixRotate(double angle)
{
    SnclDetLink *Current=First;
    if(Current)
    {
        Current->ID->Ecv->Matrix.Rotate(angle);
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::EcvMatrixMove(double x,double y)
{
    SnclDetLink *Current=First;
    if(Current)
    {
        Current->ID->Ecv->Matrix.Move(x,y);
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::MatrixIdentity()
{
    SnclDetLink *Current=First;
    while(Current)
    {
        Current->ID->Matrix.Identity();
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::MatrixMirror(int Flag)
{
    SnclDetLink *Current=First;
    if(Current)
    {
        Current->ID->Matrix.Mirror(Flag);
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::MatrixRotate(double angle)
{
    SnclDetLink *Current=First;
    if(Current)
    {
        Current->ID->Matrix.Rotate(angle);
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::MatrixMove(double x,double y)
{
    SnclDetLink *Current=First;
    if(Current)
    {
        Current->ID->Matrix.Move(x,y);
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::GetMatrixFromEcv()
{
    SnclDetLink *Current=First;
    if(Current)
    {
        Current->ID->Matrix=Current->ID->Ecv->Matrix;
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::GetDrawMatrixFromEcv()
{
    SnclDetLink *Current=First;
    if(Current)
    {
        Current->ID->DrawMatrix=Current->ID->Ecv->DrawMatrix;
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::operator --(int)
{
    SnclDetLink *Current=First;
    while(Current)
    {
        if(Current->ID->Count>0)
            Current->ID->Count--;
        Current->Nested=TRUE;
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::operator ++(int)
{
    SnclDetLink *Current=First;
    while(Current)
    {
        Current->ID->Count++;
        Current->Nested=FALSE;
        Current=Current->Next;
    }
}

BOOL CnclTask::CnclBlock::operator !=(CnclTask::CnclBlock &Block)
{
    return !((*this)==Block);
}

BOOL CnclTask::CnclBlock::operator ==(CnclTask::CnclBlock &Block)
{
    SnclDetLink *Current=First;
    SnclDetLink *CurrentDet=Block.First;
    while(Current && CurrentDet)
    {
        if(Current->ID!=CurrentDet->ID)
            return FALSE;
        Current=Current->Next;
        CurrentDet=CurrentDet->Next;
    }
    return !(Current || CurrentDet);
}

void CnclTask::CnclBlock::EcvPaintBlock(CnclRastr *Rastr,BOOL NeedFill,BOOL Inverse)
{
    SnclDetLink *Current=First;
    while(Current)
    {
        if(!Current->Nested)
        {
            CnclMatrix buf=Current->ID->Ecv->Matrix;
            Current->ID->Ecv->Matrix=Current->Matrix*Current->ID->Ecv->Matrix;
            Current->ID->Ecv->Recalc();
            Rastr->PaintDetail(Current->ID->Ecv,TRUE,FALSE);
            Current->ID->Ecv->Matrix=buf;
            Current->ID->Ecv->Recalc();
        }
        Current=Current->Next;
    }
}

void CnclTask::CnclBlock::AddToNest(CnclNest *Nest)
{
    SnclDetLink *Current=First;
    while(Current)
    {
        if(!Current->Nested)
        {
            CnclMatrix buf=Current->ID->Matrix;
            Current->ID->Matrix=Current->Matrix*Current->ID->Matrix;
            Current->ID->Recalc();
            Nest->Add(Current->ID);
            Current->ID->Matrix=buf;
            Current->ID->Recalc();
        }
        Current=Current->Next;
    }
}

BOOL CnclTask::CnclBlock::TestDetail(CnclTask::CnclBlock::SnclDetLink *Detail)
{

    Detail->ID->Ecv->Matrix.Identity();
    if(!First)
    {
        Add(Detail);
        //		if(Application->Settings.align_block)
        //			RectOptimize();
        return TRUE;
    }

    CnclTransformView TransformView;
    CnclRastr Rastr;

    EcvMatrixIdentity();
    CnclRect rect(0,0,settings.value("BuildBlockStep",1000).toInt(),settings.value("BuildBlockStep",1000).toInt());
    Rastr.Create(rect,
                 &TransformView);
    CnclRect BlockRect=EcvRect(FALSE,NULL);
    CnclRect DetailRect=Detail->ID->Ecv->Rect(FALSE,NULL);
    CnclRect rectzoom(BlockRect.Left-DetailRect.Width(),
                      BlockRect.Bottom-DetailRect.Height(),
                      BlockRect.Top+DetailRect.Height(),
                      BlockRect.Right+DetailRect.Width());
    double Rastr_scale=TransformView.ZoomAll(settings.value("BuildBlockStep",1000).toInt(),
                                             settings.value("BuildBlockStep",1000).toInt(),
                                             rectzoom,FALSE);

    CnclRect sheetRect=EcvRect(TRUE,&TransformView);
    DetailRect=Detail->ID->Rect(TRUE,&TransformView);
    unsigned long x_offset=(unsigned long)(DetailRect.Width()); //на эту величину*2 увеличивается ширина раскра листа
    unsigned long y_offset=(unsigned long)(DetailRect.Height()); //на эту величину*2 увеличивается высота раскра листа
    sheetRect=CnclRect(sheetRect.Left-DetailRect.Width(),
                       sheetRect.Bottom-DetailRect.Height(),
                       sheetRect.Top+DetailRect.Height(),
                       sheetRect.Right+DetailRect.Width());
    Rastr.Create(sheetRect, &TransformView);
    EcvPaintBlock(&Rastr,TRUE,FALSE);
    CnclRastr DetailRastr;
    long best_x=Rastr.real_width;
    long best_y=Rastr.real_height;
    long best_angle_x=0;
    long best_angle_y=0;
    long best_angle;
    //	long best_mirror=0;
    //	long best_mirror_y;
    //	long best_mirror_x;

    //	for(int mirror=0;mirror<2;mirror++)
    {
        for(int angle=0;angle<2;angle++)
        {
            Detail->ID->Ecv->Matrix.Identity();
            //			if(mirror>0)
            //				Detail->ID->Ecv->Matrix.Mirror(INVERSE_VERTICAL);
            Detail->ID->Ecv->Matrix.Rotate(PI*angle/*+mirror*PI/2*/);
            Detail->ID->Ecv->Recalc();
            DetailRect=Detail->ID->Ecv->Rect(FALSE,&TransformView);
            DetailRastr.Create(DetailRect,&TransformView);
            DetailRastr.PaintDetail(Detail->ID->Ecv,TRUE,FALSE);
            for(unsigned long x=0;x<Rastr.real_width;x++)
            {
                if(!Rastr.IsCross(x,y_offset,DetailRastr))
                {
                    if(ABS(best_x)>ABS(long(x)-long(x_offset)))
                    {
                        best_x=long(x)-long(x_offset);
                        best_angle_x=angle;
                        //						best_mirror_x=mirror;
                    }
                }
            }

            Detail->ID->Ecv->Matrix.Identity();
            //			if(mirror>0)
            //				Detail->ID->Ecv->Matrix.Mirror(INVERSE_VERTICAL);
            Detail->ID->Ecv->Matrix.Rotate(PI*angle/*+mirror*PI/2*/);
            Detail->ID->Ecv->Recalc();
            DetailRect=Detail->ID->Ecv->Rect(FALSE,&TransformView);
            DetailRastr.Create(DetailRect,&TransformView);
            DetailRastr.PaintDetail(Detail->ID->Ecv,TRUE,FALSE);
            for(unsigned long y=0;y<Rastr.real_height;y++)
            {
                if(!Rastr.IsCross(x_offset,y,DetailRastr))
                {
                    if(ABS(best_y)>ABS(long(y)-long(y_offset)))
                    {
                        best_y=long(y)-long(y_offset);
                        best_angle_y=angle;
                        //						best_mirror_y=mirror;
                    }
                }
            }

        }
    }
    double h_x=Rastr.real_height/3>DetailRastr.real_height?
                Rastr.real_height/3:DetailRastr.real_height;
    double w_y=Rastr.real_width/3>DetailRastr.real_width?
                Rastr.real_width/3:DetailRastr.real_width;
    double w_x=Rastr.real_width/3;
    if(best_x<=0)
        w_x+=(ABS(best_x));
    else
    {
        if((best_x+DetailRastr.real_width)>(Rastr.real_width/3))
            w_x+=(best_x+DetailRastr.real_width)-(Rastr.real_width/3);
    }
    double h_y=Rastr.real_height/3;
    if(best_y<=0)
        h_y+=(ABS(best_y));
    else
    {
        if((best_y+DetailRastr.real_height)>(Rastr.real_height/3))
            h_y+=(best_y+DetailRastr.real_height)-(Rastr.real_height/3);
    }
    if((h_x*w_x)<(h_y*w_y))
    {
        best_angle=best_angle_x;
        best_y=0;
    }
    else
    {
        best_angle=best_angle_y;
        best_x=0;
    }
    /*
    if(unsigned long(ABS(best_x))==Rastr.real_width || best_x==0)
    {
        best_angle=best_angle_y;
        best_x=0;
//		best_mirror=best_mirror_y;
    }
    else if(unsigned long(ABS(best_y))==Rastr.real_height || best_y==0)
    {
        best_angle=best_angle_x;
//		best_mirror=best_mirror_x;
        best_y=0;
    }
    else
    {
        if(((Rastr.real_width-x_offset*2)/ABS(best_x))>
           ((Rastr.real_height-y_offset*2)/ABS(best_y)))
        {
            best_y=0;
            best_angle=best_angle_x;
//			best_mirror=best_mirror_x;
        }
        else
        {
            best_x=0;
            best_angle=best_angle_y;
//			best_mirror=best_mirror_y;
        }
    }
    */
    double k_block=Area()/Rect(FALSE,NULL).Area();
    double k_detail=Detail->ID->Area()/Detail->ID->Rect(FALSE,NULL).Area();
    EcvMatrixIdentity();
    sheetRect=EcvRect(FALSE,NULL);
    Detail->ID->Ecv->Matrix.Identity();
    Detail->ID->Ecv->Matrix.Identity();
    //	if(best_mirror>0)
    //		Detail->ID->Ecv->Matrix.Mirror(INVERSE_VERTICAL);
    Detail->ID->Ecv->Matrix.Rotate(PI*best_angle/*+best_mirror*PI/2*/);
    Detail->ID->Ecv->Recalc();
    DetailRect=Detail->ID->Ecv->Rect(FALSE,NULL);
    //сдвигаем в левый нижний угол листа
    Detail->ID->Ecv->Matrix.Move(sheetRect.Left-DetailRect.Left,
                                 sheetRect.Bottom-DetailRect.Bottom);
    //сдвигаем на позицию BestPosition с учетом маштаба растра
    Detail->ID->Ecv->Matrix.Move(best_x/Rastr_scale,best_y/Rastr_scale);
    Detail->Matrix=Detail->ID->Ecv->Matrix;
    Detail->ID->Recalc();
    Add(Detail);
    CnclMatrix *oldMatrix=new CnclMatrix[Count(TRUE)];
    /*
    SnclDetLink *Current=First;
    long i=0;
    while(Current)	//запоминием старые матрицы преобразования
    {
        oldMatrix[i]=Current->Matrix;
        i++;
        Current=Current->Next;
    }
    if(Application->Settings.align_block)
        RectOptimize();
        */
    double k_all=Area()/Rect(FALSE,NULL).Area();
    //	Last=Last->Prev;
    //	Last->Next=NULL;
    if(k_all/k_detail>1.20 || k_all/k_block>1.20 || (k_all>0.90 && k_all/k_block>1.1))
    {
        //		delete [] oldMatrix;
        return TRUE;
    }
    /*
    Current=First;
    i=0;
    while(Current)	//восстанавливаем матрицы перобразования
    {
        Current->Matrix=oldMatrix[i];
        i++;
        Current=Current->Next;
    }
    delete [] oldMatrix;
    */
    Last=Last->Prev;
    Last->Next=NULL;
    return FALSE;
}

//поиск детали по ее ID
CnclTask::SnclDetail* CnclTask::GetDetail(long ID)
{
    SnclDetail *Current=DetailFirst;
    while(Current)
    {
        if(Current->ID==ID)
            return Current;
        Current=Current->Next;
    }
    Current=SheetFirst;
    while(Current)
    {
        if(Current->ID==ID)
            return Current;
        Current=Current->Next;
    }
    return NULL;
}

CnclTask::SnclDetail* CnclTask::GetDetailBySize(long width,long height,long type)
{
    SnclDetail* Current=(type==0)?SheetFirst:DetailFirst;
    while(Current)
    {
        Current->Matrix.Identity();
        Current->Recalc();
        CnclRect Box=Current->Rect(FALSE,NULL);
        if((width==long(Box.Width()) && height==long(Box.Height())) || (height==long(Box.Width()) && width==long(Box.Height())))
            return Current;
        Current=Current->Next;
    }
    return NULL;
}

CnclTask::CnclBlock* CnclTask::GetBlockBySize(long width,long height)
{
    CnclBlock* Current=BlockFirst;
    while(Current)
    {
        Current->MatrixIdentity();
        Current->Recalc();
        CnclRect Box=Current->Rect(FALSE,NULL);
        if((width==long(Box.Width()) && height==long(Box.Height())) || (height==long(Box.Width()) && width==long(Box.Height())))
            return Current;
        Current=Current->Next;
    }
    return NULL;
}
