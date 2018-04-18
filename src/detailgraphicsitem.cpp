#include "detailgraphicsitem.h"
#include <QBrush>

DetailGraphicsItem::DetailGraphicsItem(QGraphicsItem *parent) : QGraphicsPathItem(parent)
{

}

DetailGraphicsItem::DetailGraphicsItem(const QPainterPath &path, QGraphicsItem *parent):QGraphicsPathItem(path,parent)
{
}

void DetailGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(QBrush(QColor(255,255,128)));
}

void DetailGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(QBrush(QColor(128,255,128)));
}
