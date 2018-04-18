#ifndef TABLEITEMDELEGATE_H
#define TABLEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QAbstractItemModel>

#define PARAM_VALUE_LIST    Qt::UserRole+2
#define PARAM_VALUE         Qt::UserRole+1
#define PARAM_GROUP         Qt::UserRole+3

#define EXTEND_ITEM_HEIGHT  6

class TableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
private:
    bool ShowGrid;
public:
    TableItemDelegate(bool ShowGrid=true, QObject *parent = Q_NULLPTR);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // TABLEITEMDELEGATE_H
