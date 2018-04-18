#include "statusbar.h"
#include <QBoxLayout>

StatusBar::StatusBar(QWidget *parent):QObject(parent)
{
    QBoxLayout *layout=new QBoxLayout(QBoxLayout::LeftToRight,parent);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,10,0);

    labels<<new QLabel(parent);
    layout->addWidget(labels.last());
    labels<<new QLabel(parent);
    labels.last()->setMinimumWidth(100);
    layout->addWidget(labels.last());
    labels<<new QLabel(parent);
    labels.last()->setMinimumWidth(100);
    layout->addWidget(labels.last());
    labels<<new QLabel(parent);
    labels.last()->setMinimumWidth(100);
    layout->addWidget(labels.last());
    labels<<new QLabel(parent);
    labels.last()->setMinimumWidth(100);
    layout->addWidget(labels.last());

    layout->setStretchFactor(labels.first(),1);
}

void StatusBar::SetText(int index, QString text)
{
    if(index<labels.count())
        labels[index]->setText(text);
}

void StatusBar::Clear()
{
    foreach (QLabel* label, labels)
    {
        label->setText("");
    }
}
