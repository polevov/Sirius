#ifndef SCRIPTCONTROL_H
#define SCRIPTCONTROL_H
#include <QObject>
#include <scriptext.h>
#include <task.h>

class ScriptControl:public scriptExt
{
    Q_OBJECT
public:
    ScriptControl(QString script,Task *currentTask, QObject *parent=nullptr);
    void SetScript(QString script);
    void TerminateProcess();
public slots:
    QVariant Execute(QString function="run");
    // void terminate();
signals:
    void finished();
    void error(QString message);
private:
    QString _script;
    //scriptExt *ext;
};

#endif // SCRIPTCONTROL_H
