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

#ifndef _NCLTASK_H_
#define _NCLTASK_H_

#include "ncldetail.h"
#include "nclnest.h"
#include "nclrastr.h"
#include "ImportExport\\nclCommonImport.h"
#include "nclTaskFile.h"

#define ALL_DETAIL				0	//количество всех деталей в задании
#define DIFFERENT_DETAIL		1	//количество типоразмеров деталей
#define ALL_SHEET				2	//количество всех листов в задании
#define DIFFERENT_SHEET			3	//количество типоразмеров листов
#define NEST					4	//количество раскройных карт
#define NEST_DETAIL				5	//количество раскроенных деталей
#define BLOCK					6	//количество блоков
//#define SINGLE_BLOCK			7	//количество блоков из одной детали

//режимы очисти задания
#define CLEAR_ALL	0	//удаление всех объектов задания
#define CLEAR_NEST	1	//удаление раскроя
#define CLEAR_BLOCK	2	//удаление блоков
#define CLEAR_COUNT	3	//возврат первоначального количества деталей и листов

//#define LINER_TYPE		0	//линейная функция
//#define PARABOL_TYPE	1	//парабола
//#define STEPEN_TYPE		2	//степенная функция

//класс отвечающий за загрузку данных о геометрии деталей
//и листов из файла задания на раскрой
//также этот класс хранит всю информацию о деталях

class CnclTask  
{
public:
    struct SnclDetail:public CnclDetail	//список деталей для раскроя
    {
        //		SnclDetail();
        SnclDetail(const CnclDetail &_Detail, long Count);
        ~SnclDetail();
        SnclDetail *Next;		//указатель на следующую деталь
        SnclDetail *Prev;		//указатель на предыдущую деталь
        long Count;				//текущее количество деталей данного типа
        long OriginalCount;		//начальное количество деталей
    };


    class CnclBlock	//список блоков для раскроя
    {
    public:
        struct SnclDetLink
        {
            SnclDetLink();
            SnclDetail *ID;	//ссылка на деталь
            SnclDetLink *Next;
            SnclDetLink *Prev;
            CnclMatrix Matrix;	//матрица начального положения внутри блока
            BOOL Nested;  //раскроена это деталь блока или еще нет
            BOOL Double;  //является ли этот блок копией другого блока
        };
        void Add(SnclDetLink *Detail);
        BOOL Add(SnclDetail *Detail);
        void Clear();
        void Recalc();		//пересчет параметров блока
        CnclRect Rect(BOOL Draw,CnclTransformView *TransformView);	//расчет габаритного прямоугольника
        double Area();	//вычисление площади блока
        double Len();	//вычисление периметра блока
        CnclBlock();
        long Count(BOOL All);	//подсчет количества деталей в блоке
        ~CnclBlock();
        SnclDetLink *First;	//ссылка на первую деталь блока
        SnclDetLink *Last;	//ссылка на последнюю деталь блока
        CnclBlock *Next;	//ссылка на следующий блок
        CnclBlock *Prev;	//ссылка на предыдущий блок
        CnclRect RealRect;	//габаритный прямоугольник
        void operator --(int);		//уменьшение кол-ва делатей в блоке
        void operator ++(int);		//увеличение кол-ва делатей в блоке
        BOOL operator !=(CnclBlock &Block);	//оператор сравнения блоков
        BOOL operator ==(CnclBlock &Block);	//оператор сравнения блоков
        CnclRect EcvRect(BOOL Draw,CnclTransformView *TransformView);
        void MatrixIdentity();
        void MatrixMirror(int Flag);
        void MatrixRotate(double angle);
        void MatrixMove(double x,double y);

        void EcvRecalc();
        void EcvMatrixIdentity();
        void EcvMatrixMirror(int Flag);
        void EcvMatrixRotate(double angle);
        void EcvMatrixMove(double x,double y);
        void GetMatrixFromEcv();	//присваевает матрицу преобразования из эквидестанты к детали
        void GetDrawMatrixFromEcv();
        void EcvPaintBlock(CnclRastr *Rastr,BOOL NeedFill,BOOL Inverse);
        void AddToNest(CnclNest *Nest);
        void RectOptimize();	//оптимизировать блок по площади габаритного прямоугольника
        int angle_step; //количество шагов вращения
    private:
        BOOL TestDetail(SnclDetLink *Detail);
    };
    //Параметры определяющие тип задания на раскрой
    //и выбор алгоритма раскроя для этого задания
    /*
    struct STaskParam
    {
        double TeorNest;	//отношение площади деталей к площади листов задания
        double Podobie;		//коэффициент одинаковых деталей в задании
        double Box;			//коэффициент квадратности деталей в задании
        double Otv;			//процент деталей с отверстиями
    }TaskParam;
*/
    class CnclNestOrder
    {
    public:
        struct SnclNestOrder
        {
            CnclBlock **IDs;		//список ссылок на блоки в определенном порядке
            SnclNestOrder *Next;	//ссылка на следующую последовательность
            SnclNestOrder *Prev;	//ссылка на предыдущую последовательность
            SnclNestOrder();
            ~SnclNestOrder();
            void Restore(long s, long k);	//восстанавливает дерево (сортировка деревом)
            void SortOrder();			//сортировка последовательности (сортировка деревом)
            long Count;					//количество деталей в последовательности
            void Nesting(CnclBlock::SnclDetLink *Detail);	//уменьшение кол-ва деталей в списке
            //			void Nesting(SnclDetail *Detail);	//установка флага Nested для детали Detail
        };
        BOOL Add(SnclNestOrder *NestOrder);	//добавление новой последовательности
        void Clear();			//удаление всех последовательностей
        SnclNestOrder *First;	//ссылка на первую последовательность
        SnclNestOrder *Last;	//ссылка на последнюю последовательность
        CnclNestOrder();
        ~CnclNestOrder();
        void OrderFromTask(CnclTask *Task);	//создание последовательности из задания
        //		void OrderFromTask(CnclTask *Task,DWORD TypeBlock);	//создание последовательности из задания
        BOOL BuildNewOrder();		//создание новой последовательности
        //		void TaskFromOrder(CnclTask *Task);	//создание задание по последовательности
    };

    class CnclNestedTask
    {
    public:
        CnclNestedTask();
        ~CnclNestedTask();
        struct SnclNest:public CnclNest
        {
            SnclNest();
            ~SnclNest();
            long Index();		//порядковый номер раскроя
            SnclNest *Next;		//указатель на следующий раскрой
            SnclNest *Prev;		//указатель на предыдущий раскрой
        };
        SnclNest *NestFirst;	//указатель на первый раскрой
        SnclNest *NestLast;		//указатель на последний раскрой
        SnclNest *NestCurrent;	//указатель на текущий раскрой
        SnclNest* Add();		//добавить новый раскрой
        void Delete(SnclNest *Nest);	//удалить раскрой
        long Count(long Flag);			//подсчет
        void Clear();			//удалить все раскрои
        void Nest(SnclDetail *SheetFirst,
                  CnclNestOrder::SnclNestOrder *NestBlockOrder,
                  DWORD plug_id,HWND hwndStatusBar/*,BOOL *Stop*/); //раскроить задание
        //		void Nest(SnclDetail *SheetFirst,
        //			CnclNestOrder::SnclNestOrder *NestBlockOrder,
        //			CnclNestOrder::SnclNestOrder *NestDetailOrder,
        //			DWORD plug_id,HWND hwndStatusBar/*,BOOL *Stop*/); //раскроить задание
        void Calc();			//подсчет коэффициентов раскроя
        double NestFactor;		//средний коэффициент раскроя
        double CutNestFactor;	//средний коэффициент раскроя с отрезанной правой частью листа
        double CutLastSheetNestFactor;//средний коэффициент раскроя с отрезанной правой частью последнего листа
    };

    CnclTask();
    ~CnclTask();
    BOOL Add(CnclDetail *Detail, long Count,long Flag);
    BOOL Load(const WCHAR *FileName,CnclTransformView *DrawTransform,BOOL nest);
    CnclRect Rect(BOOL Draw,CnclTransformView *TransformView);
    //void Draw(BOOL DrawArrows);
    long Count(long Flag);
    void Clear(long Flag);
    void Check();
    //	SnclDetail *FindDetail(long ID);
    void ChooseNest();	//выбор лючшего раскроя
    //	void Sort();
    //	void NCLNest(HWND hwndStatusBar,BOOL *Stop);

    BOOL LoadGeom(CnclCommonImport *Geom,CnclTransformView *DrawTransform,long Count_det,long Flag,int angle_step);
private:
    //	long DetailCount;		//количество типоразмеров деталей
    //	long det_count;			//количество деталей в задании
    //	long SheetCount;		//количество типоразмеров листов
    //	long sheet_count;		//количество листов в задании
    BOOL Add(SnclDetail *Detail,long Flag);
    void LoadKOL(CnclTransformView *DrawTransform);
    BOOL LoadDBS(CnclTaskFile::SnclGeomFile *GeomFile,CnclTransformView *DrawTransform);
    BOOL LoadDXF(CnclTaskFile::SnclGeomFile *GeomFile,CnclTransformView *DrawTransform,BOOL nest);
    //	WCHAR TaskFile[MAX_PATH];

    void Add(CnclBlock *Block);	//добавляет блок
    //	void Add(CnclBlock *Block,BOOL Single);	//добавляет блок
    //	void Delete(SnclDetail *Detail);	//удаляет блок из списка блоков из одной детали
    //	void AddDBSNestResult(WCHAR *FileName);
    //	CnclRastr Rastr1;
    //	CnclRastr Rastr2;
public:
    //	CnclNest Nest;	//раскройная карта задания
    void BuildBlock();
    CnclTask& operator = (const CnclTask &Task); //оператор присваивания для задач
    SnclDetail* GetDetail(long ID);

    CnclNestOrder NestBlockOrder;//хранит порядок раскроя блоков
    //	CnclNestOrder NestDetailOrder;//хранит порядок раскроя деталей

    SnclDetail *DetailFirst;	//указатель на первую деталь в задании
    SnclDetail *DetailLast;		//указатель на последнюю деталь в задании
    SnclDetail *SheetFirst;		//указатель на первый лист в задании
    SnclDetail *SheetLast;		//указатель на последний лист в задании
    CnclBlock  *BlockFirst;		//указатель на первый блок
    CnclBlock  *BlockLast;		//указатель на последний блок
    //	CnclBlock  *BlockSingleFirst;		//указатель на первый блок из 1 детали
    //	CnclBlock  *BlockSingleLast;		//указатель на последний блок из 1 детали

    CnclNestedTask *Task;	   /*ссылка на раскраиваемое задание*/
    CnclNestedTask *TaskBest; /*ссылка на лучшее на текущий момент раскроенное задание*/

    CnclTaskFile TaskFile;
    SnclDetail* GetDetailBySize(long width,long height,long type); //поиск детали по ее габаритам
    CnclBlock* GetBlockBySize(long width,long height); //поиск блока по его габаритам
};
//extern CnclTask Task;

#endif
