#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include "common.h"
#include <settingsdialog.h>
#include <QXmlStreamReader>
#include "propertylist.h"
#include <ncl/nclTransformView.h>
#include <QStandardPaths>
#include <QDebug>

QSettings *settings;
PropertyList params;
CnclTransformView DrawTransform;

QString GetStandartLocation(QStandardPaths::StandardLocation location)
{
    QStringList dirs=QStandardPaths::standardLocations(location);
    if(dirs.count()>0)
    {
        return dirs[0];
    }
    return QString();
}

void CopyConfigFiles(QString fromDir)
{
    QString AppDataDir=GetStandartLocation(QStandardPaths::AppLocalDataLocation);
    QString AppDir=fromDir+"/config/";
    QDirIterator it(AppDir,QDir::NoDotAndDotDot|QDir::Files,QDirIterator::Subdirectories);
    QDir dir;
    while(it.hasNext())
    {
        it.next();
        //if(!dir.exists())
        QString FileName=it.fileName();
        QString FileDir=it.fileInfo().path().mid(AppDir.length());
        dir.setPath(AppDataDir+"/config/"+FileDir);
        if(!dir.exists())
        {
            dir.mkpath(dir.path());
        }
        QFile::remove(dir.path()+"/"+FileName);
        QFile::copy(it.filePath(),dir.path()+"/"+FileName);
    }
    QDirIterator it_clear(AppDataDir+"/config/",QStringList()<<"*.jsc",QDir::NoDotAndDotDot|QDir::Files,QDirIterator::Subdirectories);
    while(it_clear.hasNext())
    {
        it_clear.next();
        QFile::remove(it_clear.filePath());
    }
}

void LoadParams(bool LoadDefault=false)
{
    //Загружаем параметры
    QString name;
    QFile file(GetStandartLocation(QStandardPaths::AppLocalDataLocation)+"/config/sirius_property.xml");
    file.open(QIODevice::ReadOnly);
    QXmlStreamReader sr(&file);
    params.clear();
    QString group="/";
    QString group_description="\n";
    while (!sr.atEnd() && !sr.hasError())
    {
        QXmlStreamReader::TokenType token = sr.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        QXmlStreamAttributes sa=sr.attributes();
        if (token == QXmlStreamReader::StartElement)
        {
            if (sr.name() == "property")
            {
                name=sa.value("name").toString();
                params.append(TaskParams());
                params.last().name=name;
                params.last().group=group;
                params.last().group_description=group_description;
                params.last().decription=sa.value("description").toString();
                params.last().type=sa.value("type").toString();
                if(params.last().type=="bool")
                    params.last().value=sa.value("value").toString()=="true";
                if(params.last().type=="int")
                    params.last().value=sa.value("value").toInt();
                if(params.last().type=="double")
                    params.last().value=sa.value("value").toDouble();
                if(!LoadDefault)
                {
                    if(params.last().type=="bool")
                        params.last().value=settings->value("Property"+group+name,params.last().value).toBool();
                    if(params.last().type=="int")
                        params.last().value=settings->value("Property"+group+name,params.last().value).toInt();
                    if(params.last().type=="double")
                        params.last().value=settings->value("Property"+group+name,params.last().value).toDouble();
                    if(params.last().type=="string")
                        params.last().value=settings->value("Property"+group+name,params.last().value).toString();
                }
            }
            if (sr.name() == "value" && !name.isEmpty())
            {
                QString value_name=sa.value("name").toString();
                QMap<QString,QVariant> value;
                if(params.last().type=="int")
                    value[value_name]=sa.value("value").toInt();
                if(params.last().type=="bool")
                    value[value_name]=(bool)sa.value("value").toInt();
                if(params.last().type=="double")
                    value[value_name]=sa.value("value").toDouble();
                if(params.last().type=="string")
                    value[value_name]=sa.value("value").toString();
                params.last().value_list.append(value);
            }
            if(sr.name()=="group")
            {
                group+=sa.value("name").toString()+"/";
                group_description+=sa.value("description").toString()+"\n";
            }
        }
        if (token == QXmlStreamReader::EndElement)
        {
            if(sr.name()=="group")
            {
                group=group.left(group.length()-1);
                int index=group.lastIndexOf("/");
                group=group.left(index<=0?1:index+1);

                group_description=group_description.left(group_description.length()-1);
                index=group_description.lastIndexOf("\n");
                group_description=group_description.left(index<=0?1:index+1);
            }
        }
    }
    for(int i=0;i<params.Items.size();i++)
    {
        if(params[i].type=="bool" && params[i].value_list.size()==0)
        {
            QMap<QString,QVariant> value_no;
            value_no["Нет"]=false;
            params[i].value_list.append(value_no);
            QMap<QString,QVariant> value_yes;
            value_yes["Да"]=true;
            params[i].value_list.append(value_yes);
        }
    }
    //копируем файлы настроек меню и скрипты
    CopyConfigFiles(QApplication::applicationDirPath());
    QString AppDataDir=GetStandartLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(AppDataDir+"/config/");
    CopyConfigFiles(AppDataDir);
}

int main(int argc, char *argv[])
{
    settings=new QSettings(QSettings::IniFormat,QSettings::UserScope,"Sirius","sirius");
    //settings->setDefaultFormat();
    SettingsDialog::InitSettings();
    QApplication a(argc, argv);
    QApplication::setStyle("windowsvista");
    QPixmap pm(":/pic/splash.png");
    QPainter paint(&pm);
    paint.setRenderHint(QPainter::Antialiasing);
    paint.setRenderHint(QPainter::SmoothPixmapTransform);
    paint.setPen(Qt::white);
    paint.setBrush(Qt::white);
    QFont font("Arial",40,75);
    QPainterPath pp;
    pp.addText(490,100,font,SIRIUS_VERSION);
    paint.drawPath(pp);
    QSplashScreen ss(pm);
    ss.showMessage("старт программы ...",Qt::AlignBottom|Qt::AlignLeft,Qt::white);
    ss.show();
    LoadParams(); 
    MainWindow w(&ss);
    w.show();
    ss.finish(&w);
    int result=a.exec();
    delete settings;
    return result;
}
