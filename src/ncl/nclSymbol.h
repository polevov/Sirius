#ifndef _NCLSYMBOL_
#define _NCLSYMBOL_

#include "nclspline.h"

class CnclSymbol :
        public CnclSpline
{
public:
    CnclSymbol(void);
    ~CnclSymbol(void);
    CnclSymbol *Next;
    WCHAR symbol;
    CnclPoint end;
};

#endif
