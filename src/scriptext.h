#ifndef SCRIPTEXT_H
#define SCRIPTEXT_H

#include <QObject>
#include <QFile>
#include <task.h>
#include <windows.h>

class scriptExt : public QObject
{
    Q_OBJECT
public:
    explicit scriptExt(Task *currentTask, QString CurrentFileName, int CurrentTab, QObject *parent = nullptr);
    enum {tabTask,tabResult};
    enum {dirApplication,dirTask,dirJob,dirDraws};

private:
    QFile file;
    Task *task;
    QString currentFileName;
    int currentTab;
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
    int getCurrentTab();
signals:
    void switchToTab(int index);
    void setMessage(QString message);
    void hideWindow();
};

#endif // SCRIPTEXT_H
