#include "taskitemdialog.h"
#include "ui_taskitemdialog.h"
#include "common.h"
#include "tableitemdelegate.h"

void TaskItemDialog::closeEvent(QCloseEvent *event)
{
    settings->setValue("TaskItemDialog/geometry", saveGeometry());
    QDialog::closeEvent(event);
}

TaskItemDialog::TaskItemDialog(PropertyList *properties, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskItemDialog),properties(properties)
{
    ui->setupUi(this);
    setWindowTitle(QString("Свойства: %1").arg(properties->operator[]("fileName").value.toString()));
    QWidget::setWindowFlag(Qt::WindowContextHelpButtonHint,false);
    ui->PropertiesList->header()->setStretchLastSection(false);
    ui->PropertiesList->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    restoreGeometry(settings->value("TaskItemDialog/geometry").toByteArray());
    properties->FillThreeWidget(ui->PropertiesList);
}

TaskItemDialog::~TaskItemDialog()
{
    delete ui;
}

void TaskItemDialog::on_buttonBox_accepted()
{
    properties->saveTreeToSettings(ui->PropertiesList);
}
