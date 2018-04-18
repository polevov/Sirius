#include "filesystemmodel.h"

FileSystemModel::FileSystemModel(QObject *parent):QFileSystemModel(parent)
{

}

void FileSystemModel::SetDrawsIcon(QIcon icon)
{
    draw_icon=icon;
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    QFileInfo fi=FileSystemModel::fileInfo(index);
    if(role == Qt::DecorationRole)
    {
        if(fi.isFile())
        {
            return draw_icon;
        }
        else
        {
            QDir dir(fi.absoluteFilePath());
            QStringList files=dir.entryList(QStringList()<<"*.sirius",QDir::Files|QDir::NoSymLinks);
            if(files.count()>0)
            {
                return QIcon(":/icons/task16.png");
            }
        }
    }
    return QFileSystemModel::data(index,role);
}
