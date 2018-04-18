#ifndef _NCLFONT_
#define _NCLFONT_

#include "nclSymbol.h"
//#include "nclPoint.h"


class CnclFont
{
public:
    CnclFont();
    ~CnclFont();
    CnclSymbol *First;
    CnclSymbol *Last;
    void Clear();
    BOOL Add(CnclSymbol *Symbol);
    void Load(WCHAR *FileName);
    int H; //Высота шрифта над строкой
    int U; //Высота шрифта под строкой. Общая высота шрифта H+U
    void PrintText(CnclSpline* detail,WCHAR *str,double x, double y,double height,double angle,double w_scale);

protected:
    int numbers[1024];
    int ParseString(WCHAR *str,BOOL hex2000);
    CnclPoint steck[10];
    int steck_full;
    double scale;
};

#endif
