#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    FileSystemModel(QObject *parent = Q_NULLPTR);
    void SetDrawsIcon(QIcon icon);
private:
    QVariant data(const QModelIndex &index, int role) const;
    QIcon draw_icon;
};

#endif // FILESYSTEMMODEL_H
