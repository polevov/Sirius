#include "createdetail.h"
#include "ui_createdetail.h"

CreateDetail::CreateDetail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateDetail)
{
    ui->setupUi(this);
    GView=new GraphicsView(nullptr,this);
    QBoxLayout *sl=new QBoxLayout(QBoxLayout::Down,ui->Preview);
    sl->setMargin(0);
    sl->addWidget(GView);
    ui->PropertiesList->header()->setStretchLastSection(false);
    ui->PropertiesList->header()->setSectionResizeMode(0,QHeaderView::Stretch);
}

CreateDetail::~CreateDetail()
{
    delete ui;
}
