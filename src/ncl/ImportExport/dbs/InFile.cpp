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
#include "InFile.h"
#include "../../nclFile.h"

CInFile::CInFile()
{
    First=NULL;
    Last=NULL;
}

CInFile::~CInFile()
{
    Clear();
}

void CInFile::Clear()
{
    CListInFile *Current=First;
    while(Current)
    {
        Current=Current->Next;
        delete [] First->Data;
        delete First;
        First=Current;
    }
    First=NULL;
    Last=NULL;
}

BOOL CInFile::Read(const WCHAR *FileName)
{
    //  ErrorMessage.clear();
    Clear();
    unsigned short Number; //число слов в записи, тип записи, признак раверса
    long Pos;	//позиция в файле
    int exit=0;	//признак конца файла (если равен 2)
    //Открытие файла геометрии
    CnclFile InFile(FileName,GENERIC_READ,OPEN_EXISTING);
    //если попытка открытия неудачная, то выход из программы
    if(!InFile)
    {
        return FALSE;
    }
    //чтение файла геометрии
    //	WCHAR *New;
    while((exit!=2) && InFile.good())
    {
        Pos=InFile.tellg();
        InFile.read((char*)&Number,2);
        //если запись содержит меннее 5+1 слов, то выход по ошибке
        if(Number<=2) return FALSE;
        //если считано подряд два числа -1, то конец файла
        if(Number==65535)
            exit++;
        else
        {
            CListInFile *New;
            exit=0;
            //			InFile.ignore(6);
            //			New=new WCHAR[Number*4+4];
            New=new CListInFile;
            if(!New) return FALSE;
            New->Next=NULL;
            New->Data=new char[Number*4+4];
            if(!New->Data) return FALSE;
            if(Last) Last->Next=New;
            Last=New;
            Last->Next=NULL;
            if(!First) First=New;
            InFile.seekg(Pos);
            InFile.read(New->Data,Number*4+4);
        }
    }
    InFile.close();
    //	Out.close();
    return TRUE;
}


