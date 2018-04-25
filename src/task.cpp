#include "task.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDirIterator>
#include <QDebug>

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
    QStringList files;
    foreach (TaskItem ti, Items)
    {
        files<<ti.properties["fileName"].value.toString();
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
    QDirIterator it(TaskDir+"/draws/",QDir::Files|QDir::NoDotAndDotDot);
    while(it.hasNext())
    {
        it.next();
        if(files.indexOf(it.fileName())<0)
            QFile::remove(it.filePath());
    }
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
                        value_sheet["Лист"]=DetailType::typeSheet;
                        propertys.Items.last().value_list.append(value_sheet);
                        QMap<QString,QVariant> value_detail;
                        value_detail["Деталь"]=DetailType::typeDetail;
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

QString Task::toJSON()
{
    QJsonDocument json;
    QJsonObject json_object;
    QJsonArray task_arr;
    foreach (TaskItem item, Items)
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
    //qDebug()<<str;
    return str;
}
