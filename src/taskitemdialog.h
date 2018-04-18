#ifndef TASKITEMDIALOG_H
#define TASKITEMDIALOG_H

#include <QDialog>
#include <QMap>
#include "propertylist.h"

namespace Ui {
class TaskItemDialog;
}

class TaskItemDialog : public QDialog
{
    Q_OBJECT

private slots:
    void closeEvent(QCloseEvent * event);
    void on_buttonBox_accepted();

public:
    explicit TaskItemDialog(PropertyList *properties, QWidget *parent = 0);
    ~TaskItemDialog();

private:
    Ui::TaskItemDialog *ui;
    PropertyList *properties;
};

#endif // TASKITEMDIALOG_H
