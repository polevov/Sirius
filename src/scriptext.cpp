#include "scriptext.h"
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

scriptExt::scriptExt(Task *currentTask, QObject *parent) : QObject(parent)
{
    task=currentTask;
    hProcess=INVALID_HANDLE_VALUE;
}

void scriptExt::SetCurrentFileName(QString FileName)
{
    CurrentFileName=FileName;
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
    QStringList DocumentsLocation=QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    QString CurrentDir=task->TaskJobDir.isEmpty()?(DocumentsLocation.count()>0?DocumentsLocation[0]:""):task->TaskJobDir;
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
    switch (path)
    {
    case PATH_APP:
        return QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
        break;
    case PATH_JOB:
        return QDir::toNativeSeparators(task->TaskJobDir);
        break;
    case PATH_TASK:
        return QDir::toNativeSeparators(task->TaskDir);
        break;
    case PATH_DRAWS:
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
    return QDir::toNativeSeparators(CurrentFileName);
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
    QJsonDocument json;
    QJsonObject json_object;
    QJsonArray task_arr;
    foreach (TaskItem item, task->Items)
    {
        QJsonObject json_item;
        for (int i=0;i<item.properties.Items.size();i++)
        {
            if(item.properties.Items[i].type=="bool")
                json_item[item.properties.Items[i].name]=item.properties.Items[i].value.toBool();
            else if(item.properties.Items[i].type=="double")
                json_item[item.properties.Items[i].name]=item.properties.Items[i].value.toDouble();
            else if(item.properties.Items[i].type=="int")
                json_item[item.properties.Items[i].name]=item.properties.Items[i].value.toInt();
            else
                json_item[item.properties.Items[i].name]=item.properties.Items[i].value.toString();

        }
        json_item["selected"]=item.selected;
        task_arr.append(json_item);
    }
    json_object["task"]=task_arr;

    QJsonObject task_param;
    TaskParams i;
    foreach (i, params.Items)
    {
        if(i.type=="double")
            task_param[i.name]=i.value.toDouble();
        else if(i.type=="int")
            task_param[i.name]=i.value.toInt();
        else if(i.type=="bool")
            task_param[i.name]=i.value.toBool();
        else
            task_param[i.name]=i.value.toString();
    }
    json_object["parameters"]=task_param;

    json.setObject(json_object);
    QString str=json.toJson(QJsonDocument::Indented);
    return str;
}

