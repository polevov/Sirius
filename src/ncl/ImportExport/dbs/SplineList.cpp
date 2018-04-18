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
#include "SplineList.h"

CSplineList::CSplineList()
{
    First=NULL;
    Last=NULL;

}

CSplineList::~CSplineList()
{
    Clear();
}

void CSplineList::Clear()
{
    CListOfSpline *Current;
    Current=First;
    while(Current)
    {
        Current=Current->Next;
        delete First->Spline;
        delete First;
        First=Current;
    }
    First=NULL;
    Last=NULL;
}

BOOL CSplineList::AddSpline(CSpline *SplineAdd)
{
    CListOfSpline *NewSpline;
    NewSpline=new CListOfSpline;
    if (!NewSpline) return FALSE;
    NewSpline->Spline=SplineAdd;
    if (Last) Last->Next=NewSpline;
    Last=NewSpline;
    Last->Next=NULL;
    if (!First)	First=NewSpline;
    return TRUE;
}

BOOL CSplineList::Read(char *Data)
{
    CSpline *NewSpline;
    NewSpline=new CSpline;
    if(!NewSpline) return FALSE;
    if(!NewSpline->Read(Data))
        return FALSE;
    return AddSpline(NewSpline);
}

CSpline * CSplineList::GetSplineByID(unsigned short ID)
{
    CListOfSpline *Current=First;
    while(Current)
    {
        if(Current->Spline->ID==ID)
            return Current->Spline;
        Current=Current->Next;
    }
    return NULL;
}



