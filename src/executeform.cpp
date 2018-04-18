#include "executeform.h"
#include "ui_executeform.h"
#include <QMainWindow>
#include <QDebug>

ExecuteForm::ExecuteForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExecuteForm)
{
    ui->setupUi(this);
    ui->showError->hide();
    setAttribute(Qt::WA_ShowWithoutActivating,true);
    setAutoFillBackground(true);
    QWidget::setWindowFlags(Qt::SubWindow);
    //QPalette p=palette();
    //p.setBrush(QPalette::Background,QBrush(QColor(80,80,80,128)));
    //setPalette(p);
    mb=new QMessageBox(this);
    //ui->exitButton->setShortcut(Qt::Key_Escape);
    ui->exitButton->setIcon(QIcon(QPixmap(":/icons/close.png")).pixmap(16,16,QIcon::Disabled));
    hide();
}

ExecuteForm::~ExecuteForm()
{
    delete ui;
}

bool ExecuteForm::canClose()
{
    return ui->showError->document()->isEmpty();
}

void ExecuteForm::SetMessage(QString message)
{
    ui->Message->setText(message);
    ui->Caption->setText(message);
}

#define MESSAGE_HEIGHT  300
#define EMPTY_MESSAGE_HEIGHT  100

void ExecuteForm::resize()
{
    QMainWindow* mw=static_cast<QMainWindow*>(parent());
    QRect mw_rect=mw->rect();
    if(!ui->showError->document()->isEmpty())
    {
        setGeometry(0,(mw_rect.height()-MESSAGE_HEIGHT),mw_rect.width(),MESSAGE_HEIGHT);
        ui->exitButton->setEnabled(true);
        ui->frameCaption->setVisible(true);
        ui->frameButton->setVisible(false);
        ui->Message->setVisible(false);
        ui->showError->setVisible(true);
        setFocus();
    }
    else
    {
        setGeometry(mw_rect.width()/4,(mw_rect.height()-EMPTY_MESSAGE_HEIGHT)/2,mw_rect.width()/2,EMPTY_MESSAGE_HEIGHT);
        ui->exitButton->setEnabled(false);
        ui->frameCaption->setVisible(false);
        ui->frameButton->setVisible(true);
        ui->Message->setVisible(true);
        ui->showError->setVisible(false);
    }
}

void ExecuteForm::on_error(QString error)
{
    ui->showError->insertPlainText(error);
    resize();
}

void ExecuteForm::hide()
{
    if(mb->isVisible())
        mb->close();
    emit closed();
    if(!ui->showError->document()->isEmpty())
        return;
    QWidget::hide();
}

void ExecuteForm::show()
{
    resize();
    QWidget::show();
}

void ExecuteForm::on_TerminateButton_clicked()
{
    mb->setWindowTitle("Отмена");
    mb->setText(QString("Прервать выполнение прораммы \"%1\"?").arg(ui->Message->text()));
    mb->setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    mb->setDefaultButton(QMessageBox::Yes);
    mb->setIcon(QMessageBox::Question);
    if(!ui->showError->document()->isEmpty() || mb->exec()==QMessageBox::Yes)
    {
        hide();
        emit Terminate();
    }
}

void ExecuteForm::on_exitButton_clicked()
{
    ui->showError->setText(QString());
    hide();
}
