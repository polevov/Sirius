#include "tableitemdelegate.h"
#include <QLineEdit>
#include <QComboBox>
#include <QPainter>
#include <QSpinBox>
#include <QDebug>

TableItemDelegate::TableItemDelegate(bool ShowGrid, QObject *parent):QStyledItemDelegate(parent),ShowGrid(ShowGrid)
{

}

QWidget *TableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *widget=0;
    //qDebug()<<index.data(PARAM_GROUP).toBool()<<index.;
    if(index.data(PARAM_GROUP).toBool() || index.column()==0)
        return 0;
    QList<QVariant> list=index.data(PARAM_VALUE_LIST).toList();
    if(list.size()>0)
    {
        QComboBox* editor=new QComboBox(parent);

        foreach (QVariant i, list)
        {
            QMap<QString,QVariant> map=i.toMap();
            editor->addItem(map.firstKey(),map.first());
        }
        widget=editor;
    }
    else
    {
        if(index.data(Qt::UserRole)=="int")
        {
            QSpinBox* editor=new QSpinBox(parent);
            editor->setMaximum(1000000);
            editor->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
            widget=editor;
        }
        else
        {
            QLineEdit* editor=new QLineEdit(parent);
            editor->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
            widget=editor;
            if(index.data(Qt::UserRole)=="double")
                editor->setValidator(new QDoubleValidator(editor));
        }
    }
    return widget;
}

void TableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QList<QVariant> list=index.data(PARAM_VALUE_LIST).toList();
    if(list.size()>0)
    {
        QComboBox *line=static_cast<QComboBox*>(editor);
        foreach (QVariant i, list)
        {
            QMap<QString,QVariant> map=i.toMap();
            if(map.first()==index.model()->data(index,PARAM_VALUE))
                line->setCurrentIndex(list.indexOf(i));
        }
    }
    else
    {
        if(qobject_cast<QLineEdit*>(editor))
        {
            QLineEdit *line=static_cast<QLineEdit*>(editor);
            line->setText(index.model()->data(index,PARAM_VALUE).toString());
        }
        else if(qobject_cast<QSpinBox*>(editor))
        {
            QSpinBox *line=static_cast<QSpinBox*>(editor);
            line->setValue(index.model()->data(index,PARAM_VALUE).toInt());
        }
    }
}

void TableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QVariant value;
    QList<QVariant> list=index.data(PARAM_VALUE_LIST).toList();
    if(list.size()>0)
    {
        QComboBox *edit=static_cast<QComboBox*>(editor);
        value=edit->currentData();
        model->setData(index,edit->currentText());
    }
    else
    {
        if(qobject_cast<QLineEdit*>(editor))
        {
            QLineEdit *edit=static_cast<QLineEdit*>(editor);
            value=edit->text();
        }
        else if(qobject_cast<QSpinBox*>(editor))
        {
            QSpinBox *edit=static_cast<QSpinBox*>(editor);
            value=edit->value();
        }
        model->setData(index,value);
    }
    model->setData(index,value,PARAM_VALUE);
}

void TableItemDelegate::updateEditorGeometry(QWidget *editor, QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

QSize TableItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize s=QStyledItemDelegate::sizeHint(option,index);
    s.setHeight(s.height()+EXTEND_ITEM_HEIGHT);
    return s;
}


