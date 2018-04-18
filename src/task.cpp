#include "task.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

Task::Task(QObject *parent) : QObject(parent)
{
}

void Task::Add(PropertyList properties)
{
    Changed=true;
    QFileInfo fi(properties["fileName"].value.toString());
    QDir dir;
    dir.mkpath(TaskDrawDir);
    if(!QFile::exists(TaskDrawDir+"/"+fi.fileName()))
        QFile::copy(properties["fileName"].value.toString(),TaskDrawDir+"/"+fi.fileName());
    properties.SetValue("fileName",fi.fileName());
    Items.append(TaskItem(properties));
}

void Task::Add(QString FileName, int itemType)
{
    PropertyList properties;
    properties.SetValue("fileName",FileName,"Имя файла");
    properties.SetValue("itemType",itemType,"Тип");
    properties.SetValue("count",1,"Количество");
    Add(properties);
}

void Task::Select(int index)
{
    for(int i=0;i<Items.count();i++)
        Items[i].selected=i==index;
}

void Task::Save()
{
    if(TaskName.isEmpty())
        return;
    QFile file(TaskDir+"/"+TaskName+".sirius");
    file.open(QIODevice::WriteOnly);
    QXmlStreamWriter sw(&file);
    sw.setAutoFormatting(true);
    sw.writeStartDocument();
    sw.writeStartElement("task");
    sw.writeAttribute("version","1");
    foreach (TaskItem ti, Items)
    {
        sw.writeStartElement("detail");
        for (int i=0;i<ti.properties.Items.size();i++)
        {
            sw.writeAttribute(ti.properties[i].name,ti.properties[i].value.toString());
        }
        sw.writeEndElement();
    }
    sw.writeEndElement();
    sw.writeEndDocument();
    file.close();
}

QString Task::SaveKOL()
{
    if(TaskName.isEmpty())
        return "";
    QDir dir;
    dir.mkpath(TaskJobDir);
    QString FileName=TaskJobDir+"/"+TaskName+".kol";
    QFile file(FileName);
    file.open(QIODevice::WriteOnly);
    QTextStream ts(&file);
    foreach (TaskItem ti, Items)
    {
        ts<<QDir::toNativeSeparators(TaskDrawDir+"/"+ti.properties["fileName"].value.toString())<<" "<<ti.properties["count"].value.toString()<<" "<<ti.properties["itemType"].value.toString()<<"\r\n";
    }
    return QDir::toNativeSeparators(FileName);
}

void Task::Load(QString FileName)
{
    Items.clear();
    QFile file(FileName);
    file.open(QIODevice::ReadOnly);
    QFileInfo fi(FileName);
    TaskDir=QDir::toNativeSeparators(fi.absolutePath());
    TaskName=QDir::toNativeSeparators(fi.completeBaseName());
    TaskDrawDir=QDir::toNativeSeparators(TaskDir+"/draws");
    TaskJobDir=QDir::toNativeSeparators(TaskDir+"/job");
    QXmlStreamReader sr(&file);
    while (!sr.atEnd() && !sr.hasError())
    {
        QXmlStreamReader::TokenType token = sr.readNext();
        if (token == QXmlStreamReader::StartDocument)
                    continue;
        if (token == QXmlStreamReader::StartElement)
        {
            if (sr.name() == "task")
            {
                QXmlStreamAttributes sa=sr.attributes();
            }
            if (sr.name() == "detail")
            {
                QXmlStreamAttributes sa=sr.attributes();
                PropertyList propertys;
                foreach (QXmlStreamAttribute at, sa)
                {
                    propertys.SetValue(at.name().toString(),at.value().toString());
                    if(at.name()=="fileName")
                    {
                        propertys.Items.last().type="string";
                        propertys.Items.last().readOnly=true;
                    }
                    if(at.name()=="itemType")
                    {
                        propertys.Items.last().type="int";
                        QMap<QString,QVariant> value_sheet;
                        value_sheet["Лист"]=ID_SHEET;
                        propertys.Items.last().value_list.append(value_sheet);
                        QMap<QString,QVariant> value_detail;
                        value_detail["Деталь"]=ID_DETAIL;
                        propertys.Items.last().value_list.append(value_detail);
                    }
                    if(at.name()=="count")
                        propertys.Items.last().type="int";
                }
                propertys.Items.move(propertys.indexOf("fileName"),0);
                propertys.Items.move(propertys.indexOf("itemType"),1);
                propertys.Items.move(propertys.indexOf("count"),2);
                Add(propertys);
            }
        }
    }
}
