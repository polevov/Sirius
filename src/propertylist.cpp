#include "propertylist.h"
#include "tableitemdelegate.h"
#include <QDebug>

PropertyList::PropertyList()
{
}

TaskParams& PropertyList::operator[](QString key)
{
    for(int i=0;i<Items.size();i++)
    {
        if(Items.at(i).name==key)
        {
            return Items[i];
        }
    }
    TaskParams tp;
    tp.name=key;
    Items.append(tp);
    return Items.last();
}



TaskParams& PropertyList::operator[](int index)
{
    return Items[index];
}

void PropertyList::clear()
{
    Items.clear();
}

void PropertyList::append(TaskParams tp)
{
    Items.append(tp);
}

int PropertyList::indexOf(QString key)
{
    for(int i=0;i<Items.size();i++)
    {
        if(Items[i].name==key)
        {
            return i;
        }
    }
    return -1;
}

TaskParams &PropertyList::last()
{
    return Items.last();
}


void PropertyList::SetValue(QString key, QVariant value, QString description)
{
    bool result=false;
    for(int i=0;i<Items.size();i++)
    {
        if(Items[i].name==key)
        {
            Items[i].value=value;
            result=true;
            break;
        }
    }
    if(result==false)
    {
        TaskParams tp;
        tp.name=key;
        tp.value=value;
        tp.decription=description;
        if(key=="fileName" && description.isEmpty())
            tp.decription="Имя файла";
        if(key=="itemType" && description.isEmpty())
            tp.decription="Тип";
        if(key=="count" && description.isEmpty())
            tp.decription="Количество";
        if(value.type()==QVariant::Int)
            tp.type="int";
        if(value.type()==QVariant::Double)
            tp.type="double";
        if(value.type()==QVariant::String)
            tp.type="string";
        if(value.type()==QVariant::Bool)
            tp.type="bool";
        Items.append(tp);
    }
}

void PropertyList::FillThreeWidget(QTreeWidget* PropertiesList)
{
    TaskParams it;
    QString group="/";
    QString group_description="\n";
    QTreeWidgetItem *current_item=0;
    foreach(it,Items)
    {
        //int tab_index=group.count("\\")-2;

        while(group!=it.group)
        {
            if(it.group.startsWith(group))
            {
                QString add=it.group.mid(group.length(),it.group.indexOf("/",group.length())-group.length());
                QString decription=it.group_description.mid(group_description.length(),it.group_description.indexOf("\n",group_description.length())-group_description.length());
                group=group+add+"/";
                group_description=group_description+decription+"\n";
                QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()<<decription);
                item->setData(0,PARAM_GROUP,true);
                item->setData(0,PARAM_VALUE,add);
                if(!current_item)
                {
                    PropertiesList->addTopLevelItem(item);
                }
                else
                    current_item->addChild(item);
                current_item=item;
            }
            else
            {
                group=group.left(group.length()-1);
                int index=group.lastIndexOf("/");
                group=group.left(index<=0?1:index+1);

                group_description=group_description.left(group_description.length()-1);
                index=group_description.lastIndexOf("\n");
                group_description=group_description.left(index<=0?1:index+1);

                current_item=current_item->parent();
            }
        }

        QString type=it.type;
        QVariant value=it.value;
        QString name=it.name;
        QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()<<it.decription<<value.toString());
        if(!it.readOnly)
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(0,Qt::UserRole,name);
        item->setData(0,PARAM_GROUP,false);

        if(it.value_list.size()>0)
        {
            item->setTextAlignment(1,Qt::AlignLeft|Qt::AlignVCenter);
            item->setData(1,PARAM_VALUE_LIST,it.value_list);
            item->setData(1,Qt::DisplayRole,it.getName(value));
        }
        else
        {
            item->setTextAlignment(1,Qt::AlignRight|Qt::AlignVCenter);
        }
        item->setData(1,PARAM_VALUE,value);
        item->setData(1,Qt::UserRole,type);
        if(!current_item)
            PropertiesList->addTopLevelItem(item);
        else
            current_item->addChild(item);
    }
    PropertiesList->setItemDelegate(new TableItemDelegate(true,PropertiesList));
    PropertiesList->expandAll();
}

void PropertyList::saveTreeToSettings(QTreeWidget *PropertiesList, QSettings *save_settings)
{
    for(int i=0;i<PropertiesList->topLevelItemCount();i++)
    {
        saveTreeToSettings(PropertiesList->topLevelItem(i),save_settings);
    }
}


void PropertyList::saveTreeToSettings(QTreeWidgetItem *item, QSettings *save_settings)
{
    int childCount=item->childCount();
    if(childCount==0 && !item->data(0,PARAM_GROUP).toBool())
    {
        QVariant value=item->data(1,PARAM_VALUE);
        QString name=item->data(0,Qt::UserRole).toString();
        QString group="/";
        SetValue(name,value);
        if(save_settings)
        {
            group=operator [](name).group;
            save_settings->setValue("Property"+group+name,value);
        }
    }
    else
    {
        for(int i=0;i<item->childCount();i++)
        {
            saveTreeToSettings(item->child(i),save_settings);
        }
    }
}
