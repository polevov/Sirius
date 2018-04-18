// nclCommonImport.h: interface for the CnclCommonImport class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _nclCommonImport_H_
#define _nclCommonImport_H_

#include "../nclDetail.h"
#include "../nclNest.h"

class CnclCommonImport
{
public:
    struct SnclDetail:public CnclDetail	//список деталей для раскроя
    {
        SnclDetail();
        ~SnclDetail();
        SnclDetail *Next;		//указатель на следующую деталь
        SnclDetail *Prev;		//указатель на предыдущую деталь
        BOOL Text;
    };
public:
    long CountDetail();
    void Clear();
    BOOL Add();
    CnclCommonImport();
    CnclCommonImport(WCHAR *_Name,long _Count,long _Flag,long angle_step);
    ~CnclCommonImport();

    SnclDetail *First;	//готовая деталь
    SnclDetail *Last;	//готовая деталь
    void BuildDetail(WCHAR *Name,BOOL nest);
    CnclSpline Spline;	//хранение промежуточного результата считанного из DXF
    CnclNest nest;

    long Count;
    long Flag;
    long angle_step;
    WCHAR Name[512];
private:
    void BuildNest();
};

#endif
