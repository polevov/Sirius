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

#include "define.h"
#include "Doc.h"

CDoc::CDoc()
{
    FileName[0]='\0';
}

CDoc::~CDoc()
{

}
/*
BOOL CDoc::Read(const WCHAR *FileName)
{
    if1stream InFile(FileName);
    WCHAR FileGeom[512];
    int ID;	//идентификатор 0-лист, 1-деталь
    int Count; //количество деталей или листов
    if(!InFile)
    {
        return FALSE;
    }
    Spline.Clear();
    Detail.Clear();
    Sheet.Clear();

    while(InFile.good())
    {
        InFile>>FileGeom>>Count>>ID;
        if(strlen(FileGeom)==0) continue;
        if(Count<=0 || ID>1 || ID<0)
        {
            return FALSE;
        }
        os1trstream(FileGeom,sizeof(FileGeom),ios::ate)<<".dbs"<<'\0';
        if(!File.Read(FileGeom))
            continue;
        ReadDetail(ID,Count);
        Detail.Rename(100000);
        Sheet.Rename(100000);
        FileGeom[0]='\0';
    }
    InFile.close();
    File.Clear();
    return TRUE;
}
*/
BOOL CDoc::ReadDetail(int ID,long Count)
{
    CListInFile *Current=File.First;
    CDetailList *DetailOrSheet;
    if(ID==DetailType::typeSheet)
        DetailOrSheet=&Sheet;
    else
        DetailOrSheet=&Detail;
    while(Current)
    {
        //Если тип записи 8, то добавляем деталь к списку деталей (ID=1)
        // или списку листов (ID=0)
        if(*(unsigned short*)&Current->Data[8]==TYPE_DETAIL_SPLINE)
        {
            //создаем новую запись в списке Detail или в списке Sheet
            //и считываем в нее информацию о партиде детали и количестве деталей
            if(!DetailOrSheet->Read(Current,&File,Count)) return FALSE;
            //определяем количество контуров в детали CountOfFigure
            //и считываем ичх из файла
            unsigned short CountOfFigure=(unsigned short)
                    (*(unsigned short*)&Current->Data[0]-3);
            CListInFile *CurrentFig;
            for(unsigned short i=0;i<CountOfFigure;i++)
            {
                //цикл для просмотра всех записей файла
                CurrentFig=File.First;
                while(CurrentFig)
                {
                    //если найдена запись типа 1 или типа 2
                    //и номер контура совпадает с номером контура в детали
                    //то добавляем этот контур к списку контуров
                    if(*(unsigned short*)&CurrentFig->Data[8]==TYPE_GEOM_SPLINE ||
                            *(unsigned short*)&CurrentFig->Data[8]==TYPE_GEOM_FIGURE)
                        if(*(unsigned short*)&CurrentFig->Data[12]==*(unsigned short*)&Current->Data[16+i*4])
                        {
                            switch(*(unsigned short*)&CurrentFig->Data[8])
                            {
                            //если тип записи 1, то добавляем контур к списку Spline и списку Figure
                            case TYPE_GEOM_SPLINE:
                            {
                                //если контур с таким номером уже добавлен, то добавлять
                                //повторно его ненужно
                                if(Spline.GetSplineByID(*(unsigned short*)&CurrentFig->Data[12]))
                                    break;
                                //считать информацию о координатах котрура в список Spline
                                if(!Spline.Read(CurrentFig->Data))
                                {
                                    return FALSE;
                                }
                                //считать информацию о координатах полюса и повороте котура в список Figure
                                if(!DetailOrSheet->Last->Detail->Figure.Read(CurrentFig->Data,Spline.Last->Spline))
                                {
                                    return FALSE;
                                }
                                break;
                            }
                                //если тип записи 2, то добавляем контур к списку Figure
                            case TYPE_GEOM_FIGURE:
                            {
                                //поиск оригинального контура в списке Spline
                                CSpline *AddToFigure=Spline.GetSplineByID(*(unsigned short*)&CurrentFig->Data[32]);
                                //если оригинальный контур не найден, то ищем его в файле
                                if(!AddToFigure)
                                {
                                    CListInFile *FindSpline=File.First;
                                    while(FindSpline)
                                    {
                                        if(*(unsigned short*)&FindSpline[8]==TYPE_GEOM_SPLINE)
                                            if(*(unsigned short*)&FindSpline[12]==*(unsigned short*)&CurrentFig->Data[32])
                                                break;
                                        FindSpline=FindSpline->Next;
                                    }
                                    //если оригинальный контур не найден в файле, то выход по ошибке
                                    if(!FindSpline)
                                    {
                                        return FALSE;
                                    }
                                    //считать информацию о координатак оригинального контура в список Spline
                                    if(!Spline.Read(FindSpline->Data))
                                    {
                                        return FALSE;
                                    }
                                    //считать информацию о полюсе и повороте оригинального контура в список Figure
                                    if(!DetailOrSheet->Last->Detail->Figure.Read(FindSpline->Data,Spline.Last->Spline))
                                    {
                                        return FALSE;
                                    }
                                    AddToFigure=Spline.Last->Spline;
                                }
                                //считать текущий контур в список контуров Figure и связать с ним оригинальный контур
                                //AddToFigure, который может быть найден в списке Spline или добавлен в него
                                //непосредственно перед этим
                                if(!DetailOrSheet->Last->Detail->Figure.Read(CurrentFig->Data,AddToFigure))
                                {
                                    return FALSE;
                                }
                                break;
                            }
                            }
                        }
                    CurrentFig=CurrentFig->Next;
                }
            }
        }
        Current=Current->Next;
    }
    return TRUE;
}
