#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <common.h>
#include <QStandardPaths>
#include <QFileDialog>
#include <QPalette>
#include <tableitemdelegate.h>
#include "common.h"
#include <QDebug>

//#define TAB_INDEX(tab_index) QString((tab_index)*6,' ')

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->dir_project->setText(settings->value("Settings/dir_project").toString());
    ui->dir_sheet->setText(settings->value("Settings/dir_sheet").toString());
    ui->dir_detail->setText(settings->value("Settings/dir_detail").toString());
    ui->textFiles->setText(settings->value("Settings/text_files","*.cbs;*.trc;*.tld;*.tld").toString());
    ui->otherFiles->setText(settings->value("Settings/other_files").toString());
    dir_dlg.setFileMode(QFileDialog::Directory);
    dir_dlg.setOption(QFileDialog::ShowDirsOnly);
    QWidget::setWindowFlag(Qt::WindowContextHelpButtonHint,false);
    QColor color=QPalette().color(QPalette::Base);
    QString StyleSheet=QString("QFrame{background-color:rgb(%1,%2,%3);};").arg(QString::number(color.red()),QString::number(color.green()),QString::number(color.blue()));
    ui->frameTask->setStyleSheet(StyleSheet);
    ui->frameDet->setStyleSheet(StyleSheet);
    ui->frameSheet->setStyleSheet(StyleSheet);
    ui->PropertiesList->header()->setStretchLastSection(false);
    ui->PropertiesList->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    params.FillThreeWidget(ui->PropertiesList);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

QMap<QString, QVariant> SettingsDialog::GetDefaultSettings()
{
    QMap<QString, QVariant> default_settings;
    QString home_dir=GetStandartLocation(QStandardPaths::HomeLocation);
    if(!home_dir.isEmpty())
    {
        default_settings["Settings/dir_project"]=QDir::toNativeSeparators(home_dir+"/Sirius/Project");
        default_settings["Settings/dir_sheet"]=QDir::toNativeSeparators(home_dir+"/Sirius/Sheets");
        default_settings["Settings/dir_detail"]=QDir::toNativeSeparators(home_dir+"/Sirius/Details");
    }
    default_settings["Settings/text_files"]="*.cbs;*.trc;*.tld;*.tld";
    default_settings["Settings/other_files"]="";
    default_settings["Settings/nestPreview"]=true;
    return default_settings;
}

void SettingsDialog::DefaultSettings()
{
    QMap<QString, QVariant> default_settings=GetDefaultSettings();
    settings->setValue("Settings/dir_project",default_settings["Settings/dir_project"]);
    settings->setValue("Settings/dir_sheet",default_settings["Settings/dir_sheet"]);
    settings->setValue("Settings/dir_detail",default_settings["Settings/dir_detail"]);
    settings->setValue("Settings/text_files",default_settings["Settings/text_files"]);
    settings->setValue("Settings/other_files",default_settings["Settings/other_files"]);
    settings->setValue("Settings/nestPreview",default_settings["Settings/nestPreview"]);
}

#define INIT_SETTINGS(key,settings,default_settings) settings->setValue(key,settings->value(key,default_settings.value(key)))
void SettingsDialog::InitSettings()
{
    QMap<QString,QVariant> default_settings=GetDefaultSettings();

    INIT_SETTINGS("Settings/dir_project",settings,default_settings);
    INIT_SETTINGS("Settings/dir_sheet",settings,default_settings);
    INIT_SETTINGS("Settings/dir_detail",settings,default_settings);

    INIT_SETTINGS("Settings/text_files",settings,default_settings);
    INIT_SETTINGS("Settings/other_files",settings,default_settings);
    INIT_SETTINGS("Settings/nestPreview",settings,default_settings);
}

void SettingsDialog::on_buttonBox_accepted()
{
    settings->setValue("Settings/dir_project",ui->dir_project->text());
    settings->setValue("Settings/dir_sheet",ui->dir_sheet->text());
    settings->setValue("Settings/dir_detail",ui->dir_detail->text());
    settings->setValue("Settings/text_files",ui->textFiles->text());
    settings->setValue("Settings/other_files",ui->otherFiles->text());
    params.saveTreeToSettings(ui->PropertiesList,settings);
}

void SettingsDialog::on_TaskButton_clicked()
{
    QString dir=QFileDialog::getExistingDirectory(this,QString(),ui->dir_detail->text(),QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
     if(!dir.isEmpty())
    {
        ui->dir_project->setText(QDir::toNativeSeparators(dir));
    }
}

void SettingsDialog::on_DetButton_clicked()
{
    QString dir=QFileDialog::getExistingDirectory(this,QString(),ui->dir_detail->text(),QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
    {
        ui->dir_detail->setText(QDir::toNativeSeparators(dir));
    }
}

void SettingsDialog::on_SheetButton_clicked()
{
    QString dir=QFileDialog::getExistingDirectory(this,QString(),ui->dir_sheet->text(),QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
    {
        ui->dir_sheet->setText(QDir::toNativeSeparators(dir));
    }
}
