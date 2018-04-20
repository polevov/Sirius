#ifndef NCLTASK_H
#define NCLTASK_H

#include <QList>
#include "ncl/ImportExport/dbs/define.h"
#include "common.h"
//#include "ncl/

struct TaskItem
{
    TaskItem(PropertyList properties):properties(properties),selected(false)
    {
    }
    PropertyList properties;
    bool selected;
};

class Task : public QObject
{
    Q_OBJECT
public:
    QList<TaskItem> Items;
    Task(QObject *parent);
    QString TaskName;
    QString TaskDir;
    QString TaskJobDir;
    QString TaskDrawDir;
    bool Changed;
    void Add(PropertyList properties);
    void Add(QString FileName,int itemType);
    void Select(int index);
    void Save();
    void Load(QString FileName);
    QString toJSON();
private:
};

#endif // NCLTASK_H
