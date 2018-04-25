#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filesystemmodel.h"
#include "graphicsview.h"
#include <task.h>
#include "settingsdialog.h"
#include <QSettings>
#include <QTableWidgetItem>
#include <QStyledItemDelegate>
#include <QTreeWidgetItem>
#include <QThread>
#include <scriptcontrol.h>
#include <QSystemTrayIcon>
#include <statusbar.h>
#include <executeform.h>

#define NEST_NESTING			0
#define NEST_TEST				1
#define NEST_TERMINATE			2
#define NEST_BREAK				3
#define NEST_NESTING_NEEDLOAD	4

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void closeEvent(QCloseEvent * event);
    void rowChangedSlotFiles(QModelIndex index, QModelIndex);
    void rowChangedSlotDirs(QModelIndex index, QModelIndex);
    void setWindowTitle(QString title);

    void on_TaskButton_toggled(bool checked);
    void on_ResultButton_toggled(bool checked);
    void on_ParamsButton_toggled(bool checked);

    void on_FilesView_doubleClicked(const QModelIndex &index);

    void on_actionSettings_triggered();

    void on_task_btn_toggled(bool checked);

    void on_sheet_btn_toggled(bool checked);

    void on_detail_btn_toggled(bool checked);

    void menu_execute();

    void on_ResultTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_UpButton_clicked();

    void on_DownButton_clicked();

    void on_TypeButton_clicked();

    void on_DelButton_clicked();

    void on_AddButton_clicked();

    void on_MainTabWidget_currentChanged(int index);

    void on_actionNewTask_triggered();

    void on_Task_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void TerminateProcess();

   // void on_error(QString message);

    void on_Task_itemChanged(QTreeWidgetItem *item, int column);

    void on_Task_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_PropButton_clicked();

    void on_actionExit_triggered();

    void script_finish();
    void ReloadMenu(QAction*action);

private:
    bool eventFilter(QObject *watched, QEvent *event);
    Ui::MainWindow *ui;
    FileSystemModel *fsm_dirs;
    FileSystemModel *fsm_files;
    GraphicsView *GViewTask;
    GraphicsView *GViewResult;
    Task *currentTask;
    void LoadResult();
    void LoadMenu();
    bool AddNewFile(QString FileName);
    void AddNewItem(QString FileName, int count, int itemType);
    bool NewTask();
    QThread *thread;
    ScriptControl *sc;
    QSystemTrayIcon *trayIcon;
    void SwitchDir();
    QString CurrentFileNameTask;
    QString CurrentFileNameResult;
    StatusBar *resultSB;
    StatusBar *taskSB;
    ExecuteForm *ExecuteDlg;
    void ReloadChildMenu(QMenu *menu);
    QString GetCurrentFileName();
protected:
    void resizeEvent(QResizeEvent* event);
 signals:
    resize();
};

#endif // MAINWINDOW_H
