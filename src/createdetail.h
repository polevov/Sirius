#ifndef CREATEDETAIL_H
#define CREATEDETAIL_H

#include <QDialog>
#include "graphicsview.h"

namespace Ui {
class CreateDetail;
}

class CreateDetail : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDetail(QWidget *parent = 0);
    ~CreateDetail();

private:
    Ui::CreateDetail *ui;
    GraphicsView* GView;
};

#endif // CREATEDETAIL_H
