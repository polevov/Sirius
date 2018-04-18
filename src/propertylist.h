#ifndef PROPERTYLIST_H
#define PROPERTYLIST_H

#include <QList>
#include <QTreeWidget>
#include <QSettings>

struct TaskParams
{
    TaskParams()
    {
        group="/";
        group_description="\n";
        readOnly=false;
    }
    bool readOnly;
    QVariant value;
    QString name;
    QString group;
    QString group_description;
    QString decription;
    QString type;
    QList<QVariant> value_list;
    QString getName(QVariant value)
    {
        foreach (QVariant i, value_list)
        {
            QMap<QString,QVariant> map=i.toMap();
            if(map.first()==value)
                return map.firstKey();

        }
        return value.toString();
    }
};

class PropertyList
{
public:
    QList<TaskParams> Items;
    PropertyList();
    TaskParams& operator[](QString key);
    TaskParams& operator[](int index);
    void clear();
    void append(TaskParams tp);
    int indexOf(QString key);
    TaskParams& last();
    //QPair<QString,TaskParams> operator[](int index);
    void SetValue(QString key,QVariant value,QString description=QString());
    void FillThreeWidget(QTreeWidget* PropertiesList);
    void saveTreeToSettings(QTreeWidget *PropertiesList, QSettings *save_settings=0);
private:
    void saveTreeToSettings(QTreeWidgetItem *item, QSettings *save_settings);
};

#endif // PROPERTYLIST_H
