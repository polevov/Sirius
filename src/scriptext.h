#ifndef SCRIPTEXT_H
#define SCRIPTEXT_H

#include <QObject>
#include <QFile>
#include <task.h>
#include <windows.h>

#define PATH_APP        0
#define PATH_TASK       1
#define PATH_JOB        2
#define PATH_DRAWS      3

class scriptExt : public QObject
{
    Q_OBJECT
public:
    explicit scriptExt(Task *currentTask, QObject *parent = nullptr);
    void SetCurrentFileName(QString FileName);

private:
    QFile file;
    Task *task;
    QString CurrentFileName;
protected:
    HANDLE hProcess;
public slots:
    QString saveToFile(QString FileName, QString Content);
    bool execute(QString FileName, QString Arguments, bool StartHidden);
    bool execute(QString FileName, QStringList Arguments, bool StartHidden);
    QString getPath(int path);
    QString getCurrentTaskName();
    QString getCurrentFilePath();
    QString getFileName(QString fileName);
    QString getFileSuffix(QString fileName);
    QString getFileDir(QString fileName);
    QVariant getProperty(QString property);
    QString getCurrentTask();
signals:
    void switchToTab(int index);
    void setMessage(QString message);
    void hideWindow();
};

#endif // SCRIPTEXT_H
