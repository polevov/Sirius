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

#include "nclNest.h"
#include "ImportExport\\ncldbs.h"
#include "ImportExport\\ncldxf.h"
#include "ImportExport\\nclcbs.h"

CnclNest::CnclNest()
{
    NestFactor=0;
    CutNestFactor=0;
    //	LenNest=0;
    First=NULL;
    Last=NULL;
}

CnclNest::~CnclNest()
{
    Clear();
}

//конструктор класса
CnclNest::SnclDetail::SnclDetail()
{
    Next=NULL;
    Prev=NULL;
}

//деструктор класса
CnclNest::SnclDetail::~SnclDetail()
{
}

//добавляет деталь к карте раскроя
BOOL CnclNest::Add(CnclDetail *Detail)
{
    if(!Detail)
        return FALSE;
    SnclDetail *newDetail=new SnclDetail(*Detail);
    //	newDetail->DrawMatrix=Detail->DrawMatrix;// &Application.DrawTransform;
    //	newDetail->Ecv=new CnclDetail(*Detail->Ecv);
    //	newDetail->Ecv->DrawMatrix=Detail->DrawMatrix;
    if(!newDetail)
        return FALSE;

    newDetail->Prev=Last;
    if(!First)
        First=newDetail;
    if(Last)
        Last->Next=newDetail;
    Last=newDetail;

    return TRUE;
}

//устанавливает лист для карты раскроя
void CnclNest::SetSheet(CnclDetail *Detail)
{
    if(!Detail)
        return;
    Sheet=*Detail;
    Sheet.DrawMatrix=Detail->DrawMatrix;// &Application.DrawTransform;
    //	Sheet.Ecv=new CnclDetail(*Detail->Ecv);
    //	Sheet.Ecv->DrawMatrix=Detail->DrawMatrix;
}

//вывод карты на экран
/*
void CnclNest::Draw(BOOL DrawArrows)
{
//	Sheet.Draw(DrawArrows);
    TV_ITEM Item;
    Item.hItem=TreeView_GetSelection(Application->WndTask.hwndTree);
    Item.mask=TVIF_PARAM;
    TreeView_GetItem(Application->WndTask.hwndTree,&Item);

    RECT Rect;
    GetClientRect(Application->WndView.hWnd,&Rect);
    CnclRect detRect;
    SnclDetail *Current=First;
    while(Current)
    {
        detRect=Current->Rect(TRUE,NULL);
        if(!((detRect.Right<Rect.left) || (detRect.Left>Rect.right) ||
             (detRect.Bottom>Rect.bottom) || (detRect.Top<Rect.top)))
        {
            if(Item.lParam==Current->ID || (DWORD(Item.lParam)==DWORD(Current)))
                glColor4dv(Application->Settings.select_color);
            else
                glColor4dv(Application->Settings.detail_color);
            Current->Draw(Application->Settings.Fill,DrawArrows);

#ifdef _DEBUG

            glColor4d(0,1,0,1);
            Current->Ecv->DrawMatrix=Current->DrawMatrix;
            Current->Ecv->Matrix=Current->Matrix;
            Current->Ecv->Recalc();
            Current->Ecv->Draw(Application->Settings.Fill,DrawArrows);

#endif
        }
        Current=Current->Next;
    }

    if(Sheet.ID==Item.lParam)
        glColor4dv(Application->Settings.select_color);
    else
        glColor4dv(Application->Settings.sheet_color);

    glLineWidth(2);
    Sheet.Draw(FALSE,DrawArrows);
    glLineWidth(1);

//	glColor3d(1.0,1.0,1.0);
}
*/

//габариты карты раскроя
CnclRect CnclNest::Rect(BOOL Draw,CnclTransformView *TransformView)
{
    CnclRect NestRect=Sheet.Rect(Draw,TransformView);
    SnclDetail *Current=First;
    while(Current)
    {
        NestRect=NestRect&Current->Rect(Draw,TransformView);
        Current=Current->Next;
    }
    return NestRect;
}

//конструктор класса
CnclNest::SnclDetail::SnclDetail(const CnclDetail &_Detail):CnclDetail(_Detail)
{
    //	Detail=_Detail;
    Next=NULL;
    Prev=NULL;
}

void CnclNest::Clear()
{
    Sheet.Clear();
    SnclDetail *Current=First;
    while(Current)
    {
        First=First->Next;
        delete Current;
        Current=First;
    }
}

//сохранение раскроя в DBS файл
void CnclNest::Save(WCHAR *FileName)
{
#ifndef _USRDLL//показывать вкладки только для EXE
    /*
    {
        if(Application->Settings.saveDBS)
            CnclDBS::Save(this,FileName);
        if(Application->Settings.saveCBS)
            CnclCBS::Save(this,FileName);
        if(Application->Settings.saveDXF)
            CnclDXF::Save(this,FileName);
    }
    if(Application->Settings.saveBMP)
        CnclBMP::Save(this,FileName);
        */
#endif
}

//количество деталей в раскрое
long CnclNest::Count()
{
    SnclDetail *Current=First;
    long count=0;
    while(Current)
    {
        count++;
        Current=Current->Next;
    }
    return count;
}


void CnclNest::Calc()
{
    if(Sheet.Area()==0)
        return;
    SnclDetail *Current=First;
    CnclRect Rect;
    CnclRect SheetRect=Sheet.Rect(FALSE,NULL);
    while(Current)
    {
        Rect=Rect&Current->Rect(FALSE,NULL);
        Current=Current->Next;
    }
    NestRect=Rect;
    NestFactor=Area()/Sheet.Area();
    CutNestFactor=Area()/NestRect.Area();
}

double CnclNest::Area()
{
    double area=0;
    SnclDetail *Current=First;
    while(Current)
    {
        area+=Current->Area();
        Current=Current->Next;
    }
    return area;
}

double CnclNest::Len()
{
    double len=0;
    SnclDetail *Current=First;
    while(Current)
    {
        len+=Current->Len();
        Current=Current->Next;
    }
    return len;
}
