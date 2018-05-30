#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QStatusBar>
#include <newtaskdialog.h>
#include <settingsdialog.h>
#include <QXmlStreamReader>
#include "programaction.h"
#include <QMessageBox>
#include "scriptcontrol.h"
#include "tableitemdelegate.h"
#include "statusbar.h"
#include "taskitemdialog.h"
#include "ncl/ImportExport/nclDBS.h"
#include "createdetail.h"

extern CnclTransformView DrawTransform;

MainWindow::MainWindow(QSplashScreen* SplashScreen, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    currentDetailDir=settings->value("Settings/dir_detail").toString();
    currentSheetDir=settings->value("Settings/dir_sheet").toString();
    currentTaskDir=settings->value("Settings/dir_project").toString();

    currentTask=new Task(this);
    thread=new QThread;

    ui->setupUi(this);
    ui->MainTabWidget->tabBar()->hide();
    ui->TaskButton->setChecked(true);
    ui->ParamsButton->setVisible(false);
    taskSB=new StatusBar(ui->TaskStatusBarFrame);
    connect(ui->newTask_btn,SIGNAL(clicked()),this,SLOT(on_actionNewTask_triggered()));
    connect(ui->openTask_btn,SIGNAL(clicked()),this,SLOT(on_actionOpen_triggered()));

    GViewTask=new GraphicsView(taskSB,this);
    QBoxLayout *sl=new QBoxLayout(QBoxLayout::Down,ui->GPageTask);
    sl->setMargin(0);
    sl->addWidget(GViewTask);

    ui->TaskSplitter->setStretchFactor(1,4);
    ui->TaskSplitter->setStretchFactor(0,1);
    ui->ResultSplitter->setStretchFactor(1,3);
    ui->ResultSplitter->setStretchFactor(0,1);
    ui->ViewSplitter->setStretchFactor(0,1);
    ui->ViewSplitter->setStretchFactor(1,5);
    ui->FileViewSplitter->setStretchFactor(1,1);
    ui->FileViewSplitter->setStretchFactor(2,2);
    ui->Task->header()->setStretchLastSection(false);
    ui->Task->header()->setSectionResizeMode(0,QHeaderView::Stretch);

    ExecuteDlg=new ExecuteForm(this);
    resultSB=new StatusBar(ui->ResultStatusBarFrame);
    GViewResult=new GraphicsView(resultSB,this);
    sl=new QBoxLayout(QBoxLayout::Down,ui->GPageResult);
    sl->setMargin(0);
    sl->addWidget(GViewResult);

    fsm_dirs=new FileSystemModel(this);
    fsm_dirs->setRootPath("");
    fsm_dirs->setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
    ui->DirsView->setModel(fsm_dirs);
    ui->DirsView->hideColumn(1);
    ui->DirsView->hideColumn(2);
    ui->DirsView->hideColumn(3);
    ui->DirsView->header()->hide();

    fsm_files=new FileSystemModel(this);
    fsm_files->setRootPath("");
    fsm_files->setFilter(QDir::AllEntries|QDir::AllDirs|QDir::NoDot);
    QStringList Filters;
    Filters<<"*.dxf"<<"*.dbs";
    fsm_files->setNameFilters(Filters);
    fsm_files->setNameFilterDisables(false);
    ui->FilesView->setModel(fsm_files);
    connect(ui->FilesView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(rowChangedSlotFiles(QModelIndex,QModelIndex)));
    connect(ui->DirsView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(rowChangedSlotDirs(QModelIndex,QModelIndex)));
    ui->FilesView->installEventFilter(this);
    ui->Task->installEventFilter(this);
    restoreGeometry(settings->value("MainWnd/geometry").toByteArray());
    restoreState(settings->value("MainWnd/windowState").toByteArray());
    ui->TaskSplitter->restoreState(settings->value("MainWnd/TaskSplitterState").toByteArray());
    ui->ViewSplitter->restoreState(settings->value("MainWnd/ViewSplitterState").toByteArray());
    ui->FileViewSplitter->restoreState(settings->value("MainWnd/FileViewSplitterState").toByteArray());
    ui->ResultSplitter->restoreState(settings->value("MainWnd/ResultSplitterState").toByteArray());
    ui->task_btn->setChecked(true);
    on_task_btn_toggled(true);
    ui->DirsView->expand(ui->DirsView->currentIndex());
    ui->Task->setItemDelegate(new TableItemDelegate(false,this));
    ui->MainTabWidget->setCurrentWidget(ui->TaskTab);
    on_MainTabWidget_currentChanged(ui->MainTabWidget->currentIndex());
    LoadResult();
    ui->PropButton->setShortcut(QKeySequence(Qt::Key_E|Qt::CTRL));
    ui->UpButton->setShortcut(QKeySequence(Qt::Key_Up|Qt::CTRL));
    ui->DownButton->setShortcut(QKeySequence(Qt::Key_Down|Qt::CTRL));
    ui->actionExit->setShortcut(QKeySequence(Qt::Key_F4|Qt::ALT));
    trayIcon=new QSystemTrayIcon(QIcon(":/icons/main_icon/icon16.png"),this);
    trayIcon->show();
    trayIcon->hide();
    on_Task_currentItemChanged(nullptr,nullptr);
    connect(ui->menuBar,SIGNAL(hovered(QAction*)),this,SLOT(ReloadMenu(QAction*)));
    if(SplashScreen)
        SplashScreen->showMessage("загрузка меню ...",Qt::AlignBottom|Qt::AlignLeft,Qt::white);
    LoadMenu();
    ui->nestPreview_btn->setChecked(settings->value("Settings/nestPreview").toBool());
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReloadChildMenu(QMenu *menu)
{
    foreach (QAction* action, menu->actions())
    {
        if(action->isSeparator())
        {
            continue;
        }
        else if(action->menu())
        {
            ReloadChildMenu(action->menu());
        }
        else
        {
            ProgramAction* pa=static_cast<ProgramAction*>(action);
            sc=new ScriptControl(pa->script,currentTask,GetCurrentFileName(),ui->MainTabWidget->currentIndex(),this);
            QVariant result=sc->Execute("check");
            if(result.isValid())
               action->setEnabled(result.toBool());
            delete sc;
        }
    }
}

QString MainWindow::GetCurrentFileName()
{
    if(ui->MainTabWidget->currentWidget()!=ui->ParamsTab)
    {
        return ui->MainTabWidget->currentWidget()==ui->TaskTab?CurrentFileNameTask:CurrentFileNameResult;
    }
    else
    {
        return "";
    }
}

void MainWindow::ReloadMenu(QAction* action)
{
    if(action->parentWidget()==ui->menuProgs)
    {
        ReloadChildMenu(ui->menuProgs);
    }
}

void MainWindow::rowChangedSlotDirs(QModelIndex index, QModelIndex)
{
    QString sPath=fsm_dirs->fileInfo(index).absoluteFilePath();
    ui->FilesView->setRootIndex(fsm_files->setRootPath(sPath));
    GViewTask->clear();
    CurrentFileNameTask="";
    if(ui->task_btn->isChecked())
    {
        QDir dir(sPath);
        QStringList files=dir.entryList(QStringList()<<"*.sirius",QDir::Files|QDir::NoSymLinks);
        if(files.count()>0)
        {
            LoadTask(dir.absoluteFilePath(files[0]));
        }
    }
}

void MainWindow::setWindowTitle(QString title)
{
    QMainWindow::setWindowTitle(QString("Интегрированная САПР СИРИУС")+(title.isEmpty()?"":" - ")+title);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if(thread->isRunning())
    {
        event->ignore();
        return;
    }
    settings->setValue("MainWnd/geometry", saveGeometry());
    settings->setValue("MainWnd/windowState", saveState());

    settings->setValue("MainWnd/TaskSplitter", ui->TaskSplitter>saveGeometry());
    settings->setValue("MainWnd/TaskSplitterState", ui->TaskSplitter>saveState());
    settings->setValue("MainWnd/ViewSplitter", ui->ViewSplitter->saveGeometry());
    settings->setValue("MainWnd/ViewSplitterState", ui->ViewSplitter->saveState());
    settings->setValue("MainWnd/FileViewSplitter", ui->FileViewSplitter>saveGeometry());
    settings->setValue("MainWnd/FileViewSplitterState", ui->FileViewSplitter>saveState());
    settings->setValue("MainWnd/ResultSplitter", ui->ResultSplitter->saveGeometry());
    settings->setValue("MainWnd/ResultSplitterState", ui->ResultSplitter->saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::rowChangedSlotFiles(QModelIndex index, QModelIndex)
{
    if(fsm_files->fileInfo(index).suffix().toUpper()=="DBS" || fsm_files->fileInfo(index).suffix().toUpper()=="DXF")
    {
        GViewTask->Load(fsm_files->fileInfo(index).absoluteFilePath());
        CurrentFileNameTask=fsm_files->fileInfo(index).absoluteFilePath();
    }
    else
    {
        GViewTask->clear();
        CurrentFileNameTask="";
    }
}

void MainWindow::LoadResult()
{
    ui->ResultTree->clear();
    QDir dir(currentTask->TaskJobDir);
    QStringList filer;
    filer<<currentTask->TaskName+"_*.dbs";
    filer<<currentTask->TaskName+"_*.dxf";
    QStringList filers=(settings->value("Settings/text_files").toString()+";"+settings->value("Settings/other_files").toString()).split(";");
    foreach (QString ext, filers)
    {
        if(!ext.trimmed().isEmpty())
            filer<<currentTask->TaskName+"_"+ext;
    }
    QFileInfoList file_list=dir.entryInfoList(filer);
    QString LastFile=ui->ResultTree->property("LastFile").toString();
    foreach (QFileInfo fi, file_list)
    {
        QString name=fi.completeBaseName();
        QString index=name.mid(name.indexOf("_")+1);
        QList<QTreeWidgetItem*> items=ui->ResultTree->findItems(index,Qt::MatchExactly);
        QTreeWidgetItem* current_item=0;
        if(items.count()>0)
            current_item=items[0];
        else
        {
            current_item=new QTreeWidgetItem(QStringList()<<index);
            ui->ResultTree->addTopLevelItem(current_item);
        }
        QTreeWidgetItem* item=new QTreeWidgetItem(QStringList()<<fi.fileName());
        current_item->addChild(item);
        if(fi.fileName()==LastFile)
        {
            ui->ResultTree->setCurrentItem(item);
        }
        else
        {
            if(LastFile.isEmpty() && (fi.suffix().toUpper()=="DBS" || fi.suffix().toUpper()=="DXF"))
            {
                ui->ResultTree->setCurrentItem(item);
                LastFile=fi.fileName();
                ui->ResultTree->setProperty("LastFile",LastFile);
            }
        }
    }
    ui->ResultTree->expandAll();
    ui->ResultButton->setText(QString("Результат\n(%1)").arg(ui->ResultTree->topLevelItemCount()));
    ui->ResultButton->setEnabled(ui->ResultTree->topLevelItemCount()>0);
}

void MainWindow::LoadMenu()
{
    QString menuFile=GetStandartLocation(QStandardPaths::AppLocalDataLocation)+"/config/sirius_menu.xml";
    QFile file(menuFile);
    file.open(QIODevice::ReadOnly);
    QXmlStreamReader sr(&file);
    QMenu *menu=ui->menuProgs;
    while (!sr.atEnd() && !sr.hasError())
    {
        QXmlStreamReader::TokenType token = sr.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement)
        {
            QXmlStreamAttributes sa=sr.attributes();
            if (sr.name() == "item")
            {
                ProgramAction *pa=new ProgramAction(this);
                pa->setText(sa.value("name").toString());
                pa->script=sa.value("script").toString();
                menu->addAction(pa);
                connect(pa,SIGNAL(triggered(bool)),this,SLOT(menu_execute()));
            }
            if(sr.name() == "separator")
                menu->addSeparator();
            if(sr.name() == "submenu")
                menu=menu->addMenu(sa.value("name").toString());
        }
        if(token == QXmlStreamReader::EndElement)
        {
            if(sr.name() == "submenu")
            {
                menu = dynamic_cast<QMenu*>(menu->parent());
            }
        }
    }
    ReloadChildMenu(ui->menuProgs);
}

bool MainWindow::AddNewFile(QString FileName)
{
    QFileInfo fi(FileName);
    if(currentTask->TaskName=="")
    {
        if(!NewTask())
            return false;
    }
    int id_type=ui->sheet_btn->isChecked()?DetailType::typeSheet:DetailType::typeDetail;
    currentTask->Add(FileName,id_type);
    AddNewItem(FileName,1,id_type);
    currentTask->Save();
    LoadResult();
    return true;
}

void MainWindow::AddNewItem(QString FileName, int count, int itemType)
{
    QFileInfo fi(FileName);
    QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()<<fi.fileName()<<QString::number(count));
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    if(itemType==DetailType::typeDetail)
        item->setIcon(0,QIcon(":/icons/detail.png"));
    else if(itemType==DetailType::typeSheet)
        item->setIcon(0,QIcon(":/icons/sheet.png"));
    item->setData(0,PARAM_GROUP,false);
    item->setData(1,PARAM_VALUE,count);
    item->setData(1,Qt::UserRole,"int");
    item->setTextAlignment(1,Qt::AlignRight|Qt::AlignVCenter);
    ui->Task->addTopLevelItem(item);
}

bool MainWindow::NewTask()
{
    NewTaskDialog td(this);
    if(td.exec())
    {
        currentTask->TaskName=td.TaskName();
        currentTask->TaskDir=td.TaskDir()+"/"+currentTask->TaskName;
        currentTask->TaskDrawDir=td.TaskDir()+"/"+currentTask->TaskName+"/draws";
        currentTask->TaskJobDir=td.TaskDir()+"/"+currentTask->TaskName+"/job";
    }
    else
        return false;
    ui->Task->clear();
    currentTask->Items.clear();
    ui->sheet_btn->click();
    currentTask->Save();
    setWindowTitle(currentTask->TaskName);
    return true;
}


void MainWindow::on_TaskButton_toggled(bool checked)
{
    if(checked)
        ui->MainTabWidget->setCurrentWidget(ui->TaskTab);
}

void MainWindow::on_ResultButton_toggled(bool checked)
{
    if(checked)
    {
        ui->MainTabWidget->setCurrentWidget(ui->ResultTab);
        LoadResult();
    }
}

void MainWindow::on_ParamsButton_toggled(bool checked)
{
    if(checked)
        ui->MainTabWidget->setCurrentWidget(ui->ParamsTab);
}


void MainWindow::on_FilesView_doubleClicked(const QModelIndex &index)
{
    if(fsm_files->fileInfo(index).isDir())
    {
        QString sPath=fsm_files->fileInfo(index).absoluteFilePath();
        ui->FilesView->setRootIndex(fsm_files->setRootPath(sPath));
        ui->DirsView->setCurrentIndex(fsm_dirs->index(sPath));
    }
    else
    {
        AddNewFile(fsm_files->fileInfo(index).absoluteFilePath());
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::KeyPress)
    {
        QKeyEvent *key=static_cast<QKeyEvent *>(event);
        if(key->key()==Qt::Key_Return)
        {
            if(watched==ui->FilesView)
            {
                on_FilesView_doubleClicked(ui->FilesView->currentIndex());
            }
            else if(watched==ui->Task)
            {
                on_PropButton_clicked();
            }
        }
        if(watched==ui->Task && ui->Task->currentItem())
        {
            int index=ui->Task->indexOfTopLevelItem(ui->Task->currentItem());
            if(key->key()==Qt::Key_Plus)
            {
                currentTask->Items[index].properties["count"].value=currentTask->Items[index].properties["count"].value.toInt()+1;
                ui->Task->currentItem()->setText(1,currentTask->Items[index].properties["count"].value.toString());
                currentTask->Save();
                return true;
            }
            else if(key->key()==Qt::Key_Minus)
            {
                if(currentTask->Items[index].properties["count"].value.toInt()>1)
                {
                    currentTask->Items[index].properties["count"].value=currentTask->Items[index].properties["count"].value.toInt()-1;
                    ui->Task->currentItem()->setText(1,currentTask->Items[index].properties["count"].value.toString());
                    currentTask->Save();
                }
                return true;
            }
        }
    }
    else if(event->type()==QEvent::FocusIn)
    {
        if(watched==ui->Task)
            on_Task_currentItemChanged(ui->Task->currentItem(),0);
        else if(watched==ui->FilesView)
            rowChangedSlotFiles(ui->FilesView->currentIndex(),QModelIndex());
    }
    return false;
}


void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog s_dlg(this);
    s_dlg.exec();
}

void MainWindow::SwitchDir()
{
    ui->task_btn->setIcon(QIcon(QPixmap(":/icons/tasks.png")).pixmap(24,24,ui->task_btn->isChecked()?QIcon::Normal:QIcon::Disabled));
    ui->sheet_btn->setIcon(QIcon(QPixmap(":/icons/sheets.png")).pixmap(24,24,ui->sheet_btn->isChecked()?QIcon::Normal:QIcon::Disabled));
    ui->detail_btn->setIcon(QIcon(QPixmap(":/icons/details.png")).pixmap(24,24,ui->detail_btn->isChecked()?QIcon::Normal:QIcon::Disabled));
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    emit resize();
}

void MainWindow::on_task_btn_toggled(bool checked)
{
    if(checked)
    {
        QString path=currentTaskDir;//settings->value("Settings/dir_project").toString();
        QDir dir;
        dir.mkpath(path);
        ui->DirsView->setCurrentIndex(fsm_dirs->index(path));
        ui->FilesView->hide();
        SwitchDir();
    }
    else
    {
        currentTaskDir=fsm_dirs->fileInfo(ui->DirsView->currentIndex()).absoluteFilePath();
    }
}

void MainWindow::on_sheet_btn_toggled(bool checked)
{
    if(checked)
    {
        QString path=currentSheetDir;//settings->value("Settings/dir_sheet").toString();
        QDir dir;
        dir.mkpath(path);
        fsm_files->SetDrawsIcon(QIcon(":/icons/sheet.png"));
        ui->DirsView->setCurrentIndex(fsm_dirs->index(path));
        ui->FilesView->show();
        ui->FilesView->repaint();
        SwitchDir();
    }
    else
    {
        currentSheetDir=fsm_dirs->fileInfo(ui->DirsView->currentIndex()).absoluteFilePath();
    }
}

void MainWindow::on_detail_btn_toggled(bool checked)
{
    if(checked)
    {
        QString path=currentDetailDir;//settings->value("Settings/dir_detail").toString();
        QDir dir;
        dir.mkpath(path);
        fsm_files->SetDrawsIcon(QIcon(":/icons/detail.png"));
        ui->DirsView->setCurrentIndex(fsm_dirs->index(path));
        ui->FilesView->show();
        ui->FilesView->repaint();
        SwitchDir();
    }
    else
    {
        currentDetailDir=fsm_dirs->fileInfo(ui->DirsView->currentIndex()).absoluteFilePath();
    }
}

void MainWindow::menu_execute()
{
   // ExecuteDialog *dlg=new ExecuteDialog(this);
    //this->setDisabled(true);
    //setWindowFlag(Qt::WindowCloseButtonHint,false);
    //show();
    ui->centralWidget->setDisabled(true);
    ui->menuBar->setDisabled(true);
    ProgramAction *ac=(ProgramAction*)sender();
    ExecuteDlg->SetMessage(ac->text());
    sc=new ScriptControl(ac->script,currentTask,GetCurrentFileName(),ui->MainTabWidget->currentIndex());
    sc->moveToThread(thread);
    connect(thread,SIGNAL(started()),sc,SLOT(Execute()));
    //connect(sc,SIGNAL(finished()),thread,SLOT(quit()));
    connect(sc,SIGNAL(switchToTab(int)),ui->MainTabWidget,SLOT(setCurrentIndex(int)));
    connect(ExecuteDlg,SIGNAL(closed()),this,SLOT(script_finish()));
    connect(sc,SIGNAL(finished()),ExecuteDlg,SLOT(hide()));
    connect(sc,SIGNAL(setMessage(QString)),ExecuteDlg,SLOT(SetMessage(QString)));
    connect(ExecuteDlg,SIGNAL(Terminate()),this,SLOT(TerminateProcess()));
    //connect(sc,SIGNAL(hideWindow()),dlg,SLOT(hide()));
    connect(sc,SIGNAL(hideWindow()),this,SLOT(hide()));
    connect(sc,SIGNAL(hideWindow()),trayIcon,SLOT(show()));
    //connect(sc,SIGNAL(finished()),trayIcon,SLOT(hide()));
    connect(sc,SIGNAL(error(QString)),ExecuteDlg,SLOT(on_error(QString)));
    connect(this,SIGNAL(resize()),ExecuteDlg,SLOT(resize()));
    thread->start();
    ExecuteDlg->show();
}

void MainWindow::on_ResultTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->TextResult->clear();
    GViewResult->clear();
    CurrentFileNameResult="";
    //ui->ResultStatusText->setText("");
    resultSB->Clear();
    if(!current)
        return;
    QFileInfo fi(currentTask->TaskJobDir+"/"+current->text(0));
    QString text_files=settings->value("Settings/text_files").toString().toLower();
    if(fi.isFile() && fi.isReadable())
    {
        if(fi.suffix().toUpper()=="DBS" || fi.suffix().toUpper()=="DXF")
        {
            ui->ViewResult->setCurrentWidget(ui->GPageResult);
            GViewResult->Load(fi.absoluteFilePath());
            CurrentFileNameResult=fi.absoluteFilePath();
        }
        else if(text_files.indexOf(fi.suffix().toLower())>=0)
        {
            ui->ViewResult->setCurrentWidget(ui->TPageResult);
            QFile file(fi.absoluteFilePath());
            if(file.open(QIODevice::ReadOnly))
            {
                QTextStream ts(&file);
                ui->TextResult->setText(ts.readAll());
                CurrentFileNameResult=fi.absoluteFilePath();
                resultSB->SetText(0,QDir::toNativeSeparators(CurrentFileNameResult));
            }
        }
        else
        {
            ui->TextResult->clear();
            GViewResult->clear();
            CurrentFileNameResult=fi.absoluteFilePath();
            resultSB->SetText(0,QDir::toNativeSeparators(CurrentFileNameResult));
        }
        ui->ResultTree->setProperty("LastFile",current->text(0));
    }
}

void MainWindow::on_UpButton_clicked()
{
    if(ui->Task->currentItem())
    {
        int index=ui->Task->indexOfTopLevelItem(ui->Task->currentItem());
        QTreeWidgetItem *item=ui->Task->takeTopLevelItem(index);
        if(index==0)
        {
            ui->Task->addTopLevelItem(item);
            currentTask->Items.move(index,currentTask->Items.size()-1);
        }
        else
        {
            ui->Task->insertTopLevelItem(index-1,item);
            currentTask->Items.move(index,index-1);
        }
        ui->Task->setCurrentItem(item);
        currentTask->Save();
    }
}

void MainWindow::on_DownButton_clicked()
{

    if(ui->Task->currentItem())
    {
        int index=ui->Task->indexOfTopLevelItem(ui->Task->currentItem());
        QTreeWidgetItem *item=ui->Task->takeTopLevelItem(index);
        if(index==ui->Task->topLevelItemCount())
        {
            ui->Task->insertTopLevelItem(0,item);
            currentTask->Items.move(index,0);
        }
        else
        {
            ui->Task->insertTopLevelItem(index+1,item);
            currentTask->Items.move(index,index+1);
        }
        ui->Task->setCurrentItem(item);
        currentTask->Save();
    }
}

void MainWindow::on_TypeButton_clicked()
{
    if(ui->Task->currentItem())
    {
        int index=ui->Task->indexOfTopLevelItem(ui->Task->currentItem());
        int current_type=currentTask->Items[index].properties["itemType"].value.toInt();
        switch(current_type)
        {
        case DetailType::typeDetail:
            currentTask->Items[index].properties["itemType"].value=DetailType::typeSheet;
            ui->Task->currentItem()->setIcon(0,QIcon(":/icons/sheet.png"));
            break;
        case DetailType::typeSheet:
            currentTask->Items[index].properties["itemType"].value=DetailType::typeDetail;
            ui->Task->currentItem()->setIcon(0,QIcon(":/icons/detail.png"));
            break;
        }
        currentTask->Save();
    }
}

void MainWindow::on_DelButton_clicked()
{
    if(ui->Task->currentItem())
    {
        int index=ui->Task->indexOfTopLevelItem(ui->Task->currentItem());
        delete ui->Task->takeTopLevelItem(index);
        currentTask->Items.removeAt(index);
        currentTask->Save();
    }
}

void MainWindow::on_AddButton_clicked()
{
    if(currentTask->TaskName=="")
    {
        if(!NewTask())
            return;
    }
    QFileDialog dialog(this);
    dialog.setNameFilter("Файлы геометрии (*.dbs *.dxf)");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if (dialog.exec())
    {
        QStringList fileNames=dialog.selectedFiles();
        foreach (QString fileName, fileNames)
        {
            if(!AddNewFile(fileName))
                break;
        }
    }
}

void MainWindow::on_MainTabWidget_currentChanged(int index)
{
    if(ui->MainTabWidget->currentWidget()==ui->TaskTab)
        GViewTask->Load(CurrentFileNameTask);

    ui->TaskButton->setChecked(ui->MainTabWidget->currentWidget()==ui->TaskTab);
    ui->ResultButton->setChecked(ui->MainTabWidget->currentWidget()==ui->ResultTab);
    ui->ParamsButton->setChecked(ui->MainTabWidget->currentWidget()==ui->ParamsTab);

    ui->TaskButton->setIcon(QIcon(QPixmap(":/icons/task.png")).pixmap(32,32,ui->TaskButton->isChecked()?QIcon::Normal:QIcon::Disabled));
    ui->ResultButton->setIcon(QIcon(QPixmap(":/icons/result.png")).pixmap(32,32,ui->ResultButton->isChecked()?QIcon::Normal:QIcon::Disabled));
    ui->ParamsButton->setIcon(QIcon(QPixmap(":/icons/settings.png")).pixmap(32,32,ui->ParamsButton->isChecked()?QIcon::Normal:QIcon::Disabled));
}

void MainWindow::on_actionNewTask_triggered()
{
    NewTask();
}

void MainWindow::on_Task_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(current)
    {
        int index=ui->Task->indexOfTopLevelItem(current);
        bool needNest=false;
        int detail_index=(currentTask->Items[index].properties["itemType"].value==DetailType::typeDetail?index:-1);
        if(detail_index>=0)
        {
            while(index>0)
            {
                index--;
                if(currentTask->Items[index].properties["itemType"].value==DetailType::typeSheet)
                {
                    needNest=true;
                    break;
                }
            }
        }
        else
        {
            int listSize=currentTask->Items.size();
            int currentIndex=index;
            while(currentIndex<listSize && !needNest)
            {
                needNest=currentTask->Items[currentIndex].properties["itemType"].value==DetailType::typeDetail;
                currentIndex++;
            }
        }
        QString fileName=currentTask->TaskDrawDir+"/"+ui->Task->currentItem()->text(0);
        if(needNest && settings->value("Settings/nestPreview").toBool())
        {

            CnclTask nclTask;
            nclTask.Task=new CnclTask::CnclNestedTask;
            nclTask.Task->Add();

            QString fileName=currentTask->TaskDrawDir+"/"+currentTask->Items[index].properties["fileName"].value.toString();
            WCHAR buf[1024];
            lstrcpy(buf,(WCHAR*)fileName.utf16());
            CnclDBS DBS;
            DBS.Read(buf);
            nclTask.LoadGeom(&DBS,&DrawTransform,currentTask->Items[index].properties["count"].value.toInt(),currentTask->Items[index].properties["itemType"].value.toInt(),0);
            if(nclTask.SheetFirst)
                nclTask.Task->NestFirst->SetSheet(nclTask.SheetFirst);
            int count=currentTask->Items.size();
            index++;
            while(index<count && currentTask->Items[index].properties["itemType"].value==DetailType::typeDetail)
            {
                if(index==detail_index || detail_index==-1)
                {
                    QString fileName=currentTask->TaskDrawDir+"/"+currentTask->Items[index].properties["fileName"].value.toString();
                    lstrcpy(buf,(WCHAR*)fileName.utf16());
                    DBS.Read(buf);
                    nclTask.LoadGeom(&DBS,&DrawTransform,currentTask->Items[index].properties["count"].value.toInt(),currentTask->Items[index].properties["itemType"].value.toInt(),0);
                }
                index++;
            }
            CnclRect rect=nclTask.SheetFirst->Rect(FALSE,&DrawTransform);
            double x=rect.Left;
            double y=rect.Bottom;
            double height=0;
            CnclTask::SnclDetail *DetailCurrent=nclTask.DetailFirst;
            while(DetailCurrent)
            {
                DetailCurrent->Matrix.Identity();
                DetailCurrent->Recalc();
                CnclRect rectDetail=DetailCurrent->Rect(FALSE,&DrawTransform);
                for(int i=0;i<DetailCurrent->Count;i++)
                {
                    CnclDetail *newDetail=(CnclDetail*)DetailCurrent;
                    newDetail->Matrix.Identity();
                    if((x+rectDetail.Width())>rect.Right)
                    {
                        x=rect.Left;
                        y+=height;
                        height=0;
                    }
                    newDetail->Matrix.Move(x-rectDetail.Left,y-rectDetail.Bottom);
                    x+=rectDetail.Width();
                    newDetail->Recalc();
                    nclTask.Task->NestFirst->Add(newDetail);
                    if(height<rectDetail.Height())
                        height=rectDetail.Height();
                }
                DetailCurrent=DetailCurrent->Next;
            }

            GViewTask->clear();
            GViewTask->DrawNest(nclTask.Task->NestFirst,fileName,nclTask.Task->NestFactor);
        }
        else
        {
            GViewTask->Load(fileName);
        }
        CurrentFileNameTask=fileName;
        currentTask->Select(index);
    }
    else
    {
        currentTask->Select(-1);
        GViewTask->clear();
    }
    ui->DelButton->setEnabled(current);
    ui->UpButton->setEnabled(current);
    ui->DownButton->setEnabled(current);
    ui->PropButton->setEnabled(current);
    ui->TypeButton->setEnabled(current);
}

void MainWindow::TerminateProcess()
{
    if(sc)
        sc->TerminateProcess();
}

//void MainWindow::on_error(QString message)
//{
    //QMessageBox mb;
    //mb.critical(this,"Ошибка",message);
//}


void MainWindow::on_Task_itemChanged(QTreeWidgetItem *item, int column)
{
    int row=ui->Task->indexOfTopLevelItem(item);
    switch (column)
    {
    case 1:
        currentTask->Items[row].properties.SetValue("count",item->text(column).toInt());
        break;
    default:
        break;
    }
    currentTask->Save();
    on_Task_currentItemChanged(item,0);
}

void MainWindow::on_Task_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(column==0)
    {
        QPoint p=ui->Task->mapFromGlobal(QCursor::pos());
        QRect rect=ui->Task->visualItemRect(item);
        if(p.x()<=rect.height()-EXTEND_ITEM_HEIGHT)
            on_TypeButton_clicked();
        else
            on_PropButton_clicked();
    }
}


void MainWindow::on_PropButton_clicked()
{
    if(!ui->Task->currentItem())
        return;
    int index=ui->Task->indexOfTopLevelItem(ui->Task->currentItem());
    TaskItemDialog ti(&currentTask->Items[index].properties, this);
    if(ti.exec())
    {
        currentTask->Save();
        ui->Task->currentItem()->setText(0,currentTask->Items[index].properties["fileName"].value.toString());
        ui->Task->currentItem()->setText(1,currentTask->Items[index].properties["count"].value.toString());
        int itemType=currentTask->Items[index].properties["itemType"].value.toInt();
        if(itemType==DetailType::typeDetail)
            ui->Task->currentItem()->setIcon(0,QIcon(":/icons/detail.png"));
        else if(itemType==DetailType::typeSheet)
            ui->Task->currentItem()->setIcon(0,QIcon(":/icons/sheet.png"));
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::script_finish()
{
    thread->quit();
    trayIcon->hide();
    ui->centralWidget->setDisabled(false);
    ui->menuBar->setDisabled(false);
    if(!isVisible())
    {
        show();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QFileDialog dialog(this);
    dialog.setNameFilter("Файлы заданий (*.sirius)");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDirectory(currentTaskDir);
    if (dialog.exec())
    {
        LoadTask(dialog.selectedFiles().first());
        ui->detail_btn->setChecked(true);
    }
}


void MainWindow::LoadTask(QString fileName)
{
    currentTask->Load(fileName);
    setWindowTitle(currentTask->TaskName);
    ui->Task->clear();
    foreach (TaskItem ti, currentTask->Items)
    {
        AddNewItem(ti.properties["fileName"].value.toString(),ti.properties["count"].value.toInt(),ti.properties["itemType"].value.toInt());
    }
    ui->ResultTree->setProperty("LastFile","");
    LoadResult();
    GViewTask->Load(currentTask->TaskJobDir+"/"+ui->ResultTree->property("LastFile").toString());
}


void MainWindow::on_nestPreview_btn_toggled(bool checked)
{
    settings->setValue("Settings/nestPreview",checked);
    on_Task_currentItemChanged(ui->Task->currentItem(),nullptr);
}


void MainWindow::on_toolButton_clicked()
{
    CreateDetail cd(this);
    cd.exec();
}
