#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QLabel>
#include <ncl/nclNest.h>
#include <statusbar.h>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(StatusBar *info_label, QWidget *parent = nullptr);
    void Load(QString file_name);
    void clear();
private:
    QGraphicsScene *scene;
    void resizeEvent(QResizeEvent *event);
    StatusBar *sb;
    void DrawDetail(CnclDetail *CurrentDet, QPen pen, QBrush brush, QPainterPath *UnclosedPath, bool sheet=false);
signals:

public slots:
};

#endif // GRAPHICSVIEW_H
