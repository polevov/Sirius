#include "scriptext.h"
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

scriptExt::scriptExt(Task *currentTask, QString CurrentFileName, int CurrentTab, QObject *parent) : QObject(parent),currentFileName(CurrentFileName),currentTab(CurrentTab)
{
    task=currentTask;
    hProcess=INVALID_HANDLE_VALUE;
}

QString scriptExt::saveToFile(QString FileName,QString Content)
{
    QFileInfo fi(FileName);
    QString newFileName=fi.absoluteFilePath();
    QDir().mkpath(QFileInfo(newFileName).absolutePath());
    QFile file(newFileName);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream ts(&file);
        ts<<Content;
        file.close();
    }
    else
        return "";
    return QDir::toNativeSeparators(newFileName);

}
bool scriptExt::execute(QString FileName, QStringList Arguments, bool StartHidden)
{
    QString arg;
    foreach (QString str, Arguments)
    {
        arg+=" \""+str+"\"";
    }
   return execute(FileName,arg,StartHidden);
}

bool scriptExt::execute(QString FileName, QString Arguments, bool StartHidden)
{
    QString ProgramFile=QDir::toNativeSeparators(FileName);
    if(!QFile::exists(ProgramFile))
        ProgramFile=QDir::toNativeSeparators(QCoreApplication::applicationDirPath()+"/utils/"+ProgramFile);
    if(!QFile::exists(ProgramFile))
        ProgramFile=QDir::toNativeSeparators(QCoreApplication::applicationDirPath()+"/"+ProgramFile);
    if(!QFile::exists(ProgramFile))
    {
        qDebug()<<QString("Файл программы не найден: %1!").arg(FileName);
        return false;
    }
    STARTUPINFO si;
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb=sizeof(STARTUPINFO);
    si.dwFlags=STARTF_USESHOWWINDOW;
    si.wShowWindow=(StartHidden?SW_HIDE:SW_SHOW);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
    QString DocumentsLocation=GetStandartLocation(QStandardPaths::DocumentsLocation);
    QString CurrentDir=task->TaskJobDir.isEmpty()?(!DocumentsLocation.isEmpty()?DocumentsLocation:""):task->TaskJobDir;
    if(CreateProcess(0,(TCHAR*)(ProgramFile+" "+Arguments).utf16(),NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,(TCHAR*)CurrentDir.utf16(),&si,&pi))
    {
        if(!StartHidden)
            emit hideWindow();
        hProcess=pi.hProcess;
        WaitForSingleObject(pi.hProcess,INFINITE);
        DWORD exitCode=0;
        if(!GetExitCodeProcess(hProcess,&exitCode) || exitCode==100)
        {
            return false;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        hProcess=INVALID_HANDLE_VALUE;
    }
    else
    {
        DWORD err_code=GetLastError();
        LPWSTR err;
        DWORD result=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                         NULL,
                         err_code,
                         MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                         (LPWSTR)&err,
                         0,
                         NULL);
        if(result)
        {
            qDebug()<<QString("Файл \"%2\". Ошибка: %1").arg(QString::fromWCharArray(err,result).trimmed(),ProgramFile);
            LocalFree(err);
        }
        else
        {
            qDebug()<<QString("Файл\" %2\". Код ошибки: %1.").arg(QString::number(err_code),ProgramFile);
        }
        hProcess=INVALID_HANDLE_VALUE;
        return false;
    }
    return true;
}

QString scriptExt::getPath(int path)
{
    //qDebug()<<path;
    switch (path)
    {
    case dirApplication:
        return QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
        break;
    case dirJob:
        return QDir::toNativeSeparators(task->TaskJobDir);
        break;
    case dirTask:
        return QDir::toNativeSeparators(task->TaskDir);
        break;
    case dirDraws:
        return QDir::toNativeSeparators(task->TaskDrawDir);
        break;
    default:
        return "";
        break;
    }
}

QString scriptExt::getCurrentTaskName()
{
    return task->TaskName;
}

QString scriptExt::getCurrentFilePath()
{
    return QDir::toNativeSeparators(currentFileName);
}

QString scriptExt::getFileName(QString fileName)
{
    return QDir::toNativeSeparators(QFileInfo(fileName).fileName());
}

QString scriptExt::getFileSuffix(QString fileName)
{
    return QDir::toNativeSeparators(QFileInfo(fileName).suffix());
}

QString scriptExt::getFileDir(QString fileName)
{
    return QDir::toNativeSeparators(QFileInfo(fileName).path());
}

QVariant scriptExt::getProperty(QString property)
{
    return params[property].value;
}

QString scriptExt::getCurrentTask()
{
    return task->toJSON();
}

int scriptExt::getCurrentTab()
{
    return currentTab;
}

