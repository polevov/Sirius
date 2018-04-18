//#undef _WIN32_IE
//#if (__BCPLUSPLUS__<0x0550) //если версия компилятора Borland <5.5,
//#define _WIN32_IE 0x0200	//приложение будет совместимо с Win95 без установки IE
//#endif
#ifndef _NCLDEFINE_
#define _NCLDEFINE_

#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <commctrl.h>
#include <stdio.h>
//#include <gl\gl.h>
//#include <gl\glu.h>

//#include "nclError.h"
#include "nclTask.h"
//#include "nclCommandLine.h"
#include "nclFile.h"

bool IsFile(WCHAR* FileName);
WCHAR* Trim(WCHAR *str);
double StrToDouble(WCHAR *str);
WCHAR* DoubleToStr(double num,WCHAR *buf);
void nclRand(long dum);
float nclRand();
double Round(double num, long pr);

//Разрешить незамнкнутые контуры
#define USE_Unclosed	TRUE

//число pi
#define PI			3.141592653589793
//погрешность в вычислениях
#define Epsilon		0.00001

//макрокоманда определения модуля числа не зависимо от его типа
#define ABS(x)		((x)<0 ? -(x) : x)

//макрокоманда определения знака числа не зависимо от его типа
//принимает значение -1, если число меньше 0
//принимает значение 1, если число больше либо равно 0
#define SIGN(x)		((x)<0 ? -1 : 1)

//результат пересечения
#define CROSS_NO		0	//не пересекаются
#define CROSS_BEGIN		1	//совпадают начала сегментов
#define CROSS_END		2	//совпадают концы сегментов
#define CROSS_NEXT		3	//сегмент является продолжением (конец сегмента совпадает с началом следующего)
#define CROSS_PREV		4	//сегмент является предыдущим (начало сегмента совпадает с концом следующего)
#define CROSS_YES		5	//сегменты пересекаются
#define CROSS_IN		6	//один контур (деталь) лежит внутри другого контура (детали)
#define CROSS_ON		7	//один контур (деталь) лежит на другом контуре (детали)

//лежит ли точка внутри контура/детали
#define IN_SPLINE_NO_CLOSED	0	//контур не замнкнут
#define IN_YES				1	//внутри контура
#define IN_NO				2	//вне контура

#define POPUP_MENU		2	//какой пункт главного меню будет использоваться
//как контекстное меню главного окна программы

//определение события вращение колесика мышы
//требуется если компиляция идет под Windows 9x
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL           0x020A
#endif
/*
#if(_WIN32_IE==0x0200)

    #define LVN_ITEMACTIVATE        (LVN_FIRST-14)
    #define LVM_SETSELECTIONMARK    (LVM_FIRST + 67)
    #define NM_KEYDOWN              (NM_FIRST-15)
    #define CDRF_SKIPDEFAULT        0x00000004
    #define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 54)
    #define LVS_EX_GRIDLINES        0x00000001
    #define LVS_EX_FULLROWSELECT    0x00000020
    #define LVM_GETSELECTIONMARK    (LVM_FIRST + 66)
    #define NM_CUSTOMDRAW           (NM_FIRST-12)
    #define CDRF_NEWFONT            0x00000002
    #define CDRF_DODEFAULT          0x00000000
    #define CDDS_ITEM               0x00010000
    #define CDDS_PREPAINT           0x00000001
    #define CDDS_ITEMPREPAINT       (CDDS_ITEM | CDDS_PREPAINT)
    #define CDRF_NOTIFYITEMDRAW     0x00000020
    #define LVM_GETSUBITEMRECT      (LVM_FIRST + 56)

#ifndef _AFXDLL
    typedef struct tagNMCUSTOMDRAWINFO {
        NMHDR  hdr;
        DWORD  dwDrawStage;
        HDC    hdc;
        RECT   rc;
        DWORD  dwItemSpec;
        UINT   uItemState;
        LPARAM lItemlParam;
    } NMCUSTOMDRAW, FAR * LPNMCUSTOMDRAW;
    typedef struct tagNMLVCUSTOMDRAW {
        NMCUSTOMDRAW nmcd;
        COLORREF clrText;
        COLORREF clrTextBk;
    #if (_WIN32_IE >= 0x0400)
        int iSubItem;
    #endif
    } NMLVCUSTOMDRAW, *LPNMLVCUSTOMDRAW;
    typedef struct tagNMTBHOTITEM
    {
        NMHDR   hdr;
        int     idOld;
        int     idNew;
        DWORD   dwFlags;           // HICF_*
    } NMTBHOTITEM, * LPNMTBHOTITEM;
#endif
    #define ListView_GetSelectionMark(hwnd) \
        (int)SNDMSG((hwnd), LVM_GETSELECTIONMARK, 0, 0)

    #define ListView_GetSubItemRect(hwnd, iItem, iSubItem, code, prc) \
        (BOOL)SNDMSG((hwnd), LVM_GETSUBITEMRECT, (WPARAM)(int)(iItem), \
        ((prc) ? ((((LPRECT)(prc))->top = iSubItem), (((LPRECT)(prc))->left = code), (LPARAM)(prc)) : (LPARAM)(LPRECT)NULL))

    #define TBSTYLE_FLAT            0x0800
    #define TBN_HOTITEMCHANGE       (TBN_FIRST - 13)
#endif
*/

#define DRAW_RASTR FALSE

#if DRAW_RASTR
#define MIN_STEP_COUNT	10	//минимально возможное число шагов при укладке
#else
#define MIN_STEP_COUNT	100	//минимально возможное число шагов при укладке
#endif

#define	MIN_BLOCK_STEP_COUNT	100	//минимальное число шагов при формировании блоков

#define ID_EDIT			1000
#define ID_SPIN			1001
#define ID_BUTTON		1002
#define ID_TOOLBAR		1003

#endif
//#define TEMP_NAME "~tmp_"		//начало имени временных файлов
