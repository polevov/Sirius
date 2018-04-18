#ifndef PROGRAMACTION_H
#define PROGRAMACTION_H

#include <QAction>

class ProgramAction : public QAction
{
    Q_OBJECT
public:
    ProgramAction(QObject *parent=nullptr);
    QString script;
};

#endif // PROGRAMACTION_H
