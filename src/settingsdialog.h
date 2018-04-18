#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QMap>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    static void DefaultSettings();
    static void InitSettings();

private slots:
    void on_buttonBox_accepted();

    void on_TaskButton_clicked();

    void on_DetButton_clicked();

    void on_SheetButton_clicked();

private:
    static QMap<QString, QVariant> GetDefaultSettings();
    Ui::SettingsDialog *ui;
    QFileDialog dir_dlg;
};

#endif // SETTINGSDIALOG_H
