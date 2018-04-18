#ifndef EXECUTEFORM_H
#define EXECUTEFORM_H

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class ExecuteForm;
}

class ExecuteForm : public QWidget
{
    Q_OBJECT

public:
    explicit ExecuteForm(QWidget *parent = 0);
    ~ExecuteForm();
    bool canClose();
public slots:
    void SetMessage(QString message);
    void resize();
    void on_error(QString error);
    void hide();
    void show();

private slots:
    void on_TerminateButton_clicked();
    void on_exitButton_clicked();

signals:
    void Terminate();
    void closed();

private:
    Ui::ExecuteForm *ui;
    QMessageBox *mb;
};

#endif // EXECUTEFORM_H
