#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QThread>
#include "common.h"
#include <settingsdialog.h>
#include <QXmlStreamReader>
#include "propertylist.h"
#include <ncl/nclTransformView.h>
#include <QDebug>

QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Sirius","sirius");
PropertyList params;
CnclTransformView DrawTransform;

class I:public QThread
{
public:
    static void sleep(unsigned long secs) {QThread::sleep(secs);}
};


void LoadParams(bool LoadDefault=false)
{
    QString name;
    QFile file(QApplication::applicationDirPath()+"/config/sirius_property.xml");
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
                        params.last().value=settings.value("Property"+group+name,params.last().value).toBool();
                    if(params.last().type=="int")
                        params.last().value=settings.value("Property"+group+name,params.last().value).toInt();
                    if(params.last().type=="double")
                        params.last().value=settings.value("Property"+group+name,params.last().value).toDouble();
                    if(params.last().type=="string")
                        params.last().value=settings.value("Property"+group+name,params.last().value).toString();
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
                group+=sa.value("name")+"/";
                group_description+=sa.value("description")+"\n";
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
}

int main(int argc, char *argv[])
{
    //settings.setDefaultFormat();
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
    ss.show();
    LoadParams();
    MainWindow w;
    I::sleep(1);
    w.show();
    ss.finish(&w);
    return a.exec();
}
