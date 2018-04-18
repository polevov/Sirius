#include "newtaskdialog.h"
#include "ui_newtaskdialog.h"
#include "common.h"
#include <QDir>
#include <QFileDialog>

NewTaskDialog::NewTaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTaskDialog)
{
    ui->setupUi(this);
    ui->task_path->setText(settings.value("Settings/dir_project").toString());
    ui->task_name->setText("task");
    int i=0;
    QDir dir;
    while(dir.exists(QString("%1/%2").arg(ui->task_path->text(),ui->task_name->text())))
    {
        i++;
        ui->task_name->setText(QString("task%1").arg(QString::number(i)));
    }
    QWidget::setWindowFlag(Qt::WindowContextHelpButtonHint,false);
    QColor color=QPalette().color(QPalette::Base);
    QString StyleSheet=QString("QFrame{background-color:rgb(%1,%2,%3);};").arg(QString::number(color.red()),QString::number(color.green()),QString::number(color.blue()));
    ui->frameTask->setStyleSheet(StyleSheet);
}

NewTaskDialog::~NewTaskDialog()
{
    delete ui;
}

QString NewTaskDialog::TaskName()
{
    return ui->task_name->text();
}

QString NewTaskDialog::TaskDir()
{
    return ui->task_path->text();
}


void NewTaskDialog::on_buttonBox_accepted()
{
    QDir dir;
    dir.mkpath(QString("%1/%2").arg(ui->task_path->text(),ui->task_name->text()));
}

void NewTaskDialog::on_TaskButton_clicked()
{
    QString dir=QFileDialog::getExistingDirectory(this,QString(),ui->task_path->text(),QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
     if(!dir.isEmpty())
    {
        ui->task_path->setText(QDir::toNativeSeparators(dir));
    }
}
