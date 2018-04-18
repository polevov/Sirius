#ifndef DETAILGRAPHICSITEM_H
#define DETAILGRAPHICSITEM_H

#include <QGraphicsPathItem>

class DetailGraphicsItem : public QGraphicsPathItem
{
public:
    explicit DetailGraphicsItem(QGraphicsItem *parent = Q_NULLPTR);
    explicit DetailGraphicsItem(const QPainterPath &path, QGraphicsItem *parent = Q_NULLPTR);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
signals:

public slots:
};

#endif // DETAILGRAPHICSITEM_H
