#include "graphicsview.h"
//#include "ncl/ImportExport/dbs/doc.h"
#include "common.h"
#include "detailgraphicsitem.h"
#include "math.h"
#include <QDir>
#include "ncl/nclTask.h"
#include "ncl/define.h"
#include <QDebug>
extern CnclTransformView DrawTransform;

GraphicsView::GraphicsView(StatusBar *info_label, QWidget *parent) : QGraphicsView(parent)
{
    sb=info_label;
    scene=new QGraphicsScene(this);
    scale(1,-1);
    setScene(scene);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
}

void GraphicsView::DrawDetail(CnclDetail *CurrentDet, QPen pen, QBrush brush, QPainterPath *UnclosedPath,bool sheet)
{
    QPainterPath DetailPath;
    CnclDetail::SnclSpline *CurrentSpline=CurrentDet->First;
    while(CurrentSpline)
    {
        QPainterPath Path;
        CnclSpline::SnclSegment *CurrentSegment=CurrentSpline->First;
        CnclPoint newBegin=CurrentSegment->GetNewCoord(CurrentSegment->Begin,TRUE,NULL);
        CnclPoint newEnd=CurrentSegment->GetNewCoord(CurrentSegment->End,TRUE,NULL);
        Path.moveTo(newBegin.x,newBegin.y);
        while(CurrentSegment)
        {
            CnclPoint newBegin=CurrentSegment->GetNewCoord(CurrentSegment->Begin,TRUE,NULL);
            CnclPoint newEnd=CurrentSegment->GetNewCoord(CurrentSegment->End,TRUE,NULL);
            if(CurrentSegment->z==0)
            {
                Path.lineTo(newEnd.x,newEnd.y);
            }
            else
            {
                CnclPoint Center=CurrentSegment->CenterArc(FALSE,TRUE,&DrawTransform);
                double radius=CurrentSegment->RadiusArc(Center,FALSE,&DrawTransform);
                double begin_angl=CurrentSegment->BeginAngl(Center)/PI*180;
                double angl=CurrentSegment->Angl(Center)/PI*180;
                if(CurrentSegment->z>0)
                    Path.arcTo(Center.x-radius,Center.y-radius,radius*2,radius*2,-begin_angl,-angl);
                else
                    Path.arcTo(Center.x-radius,Center.y-radius,radius*2,radius*2,-begin_angl,angl);
            }
            CurrentSegment=CurrentSegment->Next;
        }
        if(CurrentSpline->IsClose())
        {
            DetailPath.addPath(Path);
        }
        else
        {
            UnclosedPath->addPath(Path);
        }
        CurrentSpline=CurrentSpline->Next;
    }
    DetailGraphicsItem *gp_item=new DetailGraphicsItem(DetailPath);
    gp_item->setAcceptHoverEvents(!sheet);
    gp_item->setPen(pen);
    gp_item->setBrush(brush);
    scene->addItem(gp_item);
}

void GraphicsView::Load(QString file_name)
{
    sb->Clear();
    scene->clear();

    CnclTask Task;
    Task.Load((WCHAR*)QDir::toNativeSeparators(file_name).utf16(),&DrawTransform,FALSE);
    Task.Task=new CnclTask::CnclNestedTask;
    Task.Task->Add();
    CnclTask::SnclDetail *Current=Task.DetailFirst;
    while(Current)
    {
        Current->new_EcvBuild(settings->value("DistDetToDet").toDouble()/2);
        Task.Task->NestFirst->Add(Current);
        Current=Current->Next;
    }
    if(Task.SheetFirst)
        Task.Task->NestFirst->SetSheet(Task.SheetFirst);
    delete Task.TaskBest;
    Task.TaskBest=0;
    Task.ChooseNest();
    if(!Task.TaskBest || Task.TaskBest->NestFirst->Count()==0)
        return;

    QPen pen;
    QBrush brush;
    QBrush sheet_brush;
    pen.setWidth(0);
    pen.setCosmetic(true);
    pen.setColor(QColor(0,128,0));
    QPen sheet_pen;
    sheet_pen.setWidth(2);
    sheet_pen.setCosmetic(true);
    sheet_pen.setColor(QColor(255,0,0));
    brush.setColor(QColor(128,255,128));
    brush.setStyle(Qt::SolidPattern);
    QPainterPath UnclosedPath;
    CnclRect rect;
    double square=0;
    double perimeter=0;
    CnclTask::CnclNestedTask::SnclNest* CurrentNest=Task.TaskBest->NestFirst;
    DrawDetail(&CurrentNest->Sheet,sheet_pen,sheet_brush,&UnclosedPath,true);
    while(CurrentNest)
    {
        CnclNest::SnclDetail *CurrentDet=CurrentNest->First;
        while(CurrentDet)
        {
            if(rect.Width()==0)
                rect=CurrentDet->Rect(false,&DrawTransform);
            else
                rect=rect&CurrentDet->Rect(false,&DrawTransform);
            square+=CurrentDet->Area();
            perimeter+=CurrentDet->Len();
            DrawDetail(CurrentDet,pen,brush,&UnclosedPath);
            CurrentDet=CurrentDet->Next;
        }
        if(rect.Width()==0)
            rect=CurrentNest->Sheet.Rect(false,&DrawTransform);
        else
            rect=rect&CurrentNest->Sheet.Rect(false,&DrawTransform);
        CurrentNest=CurrentNest->Next;
    }
    DetailGraphicsItem *gp_item=new DetailGraphicsItem(UnclosedPath);
    gp_item->setPen(pen);
    scene->addItem(gp_item);
    resizeEvent(0);
    Task.TaskBest->Calc();
    sb->SetText(0,QDir::toNativeSeparators(file_name));
    if(Task.TaskBest->NestFactor>0)
        sb->SetText(1,QString("K=%1\%").arg(QLocale(QLocale::Russian).toString(Task.TaskBest->NestFactor*100,'f',2)));
    sb->SetText(2,QString("%1x%2").arg(QLocale(QLocale::Russian).toString(rect.Width(),'f',0),QLocale(QLocale::Russian).toString(rect.Height(),'f',0)));
    sb->SetText(3,QString("S=%1").arg(QLocale(QLocale::Russian).toString(square,'f',0)));
    sb->SetText(4,QString("P=%1").arg(QLocale(QLocale::Russian).toString(perimeter,'f',0)));
}

void GraphicsView::clear()
{
    scene->clear();
    sb->Clear();
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    QRectF BoundingRect=scene->itemsBoundingRect();
    scene->setSceneRect(BoundingRect);
    fitInView(BoundingRect, Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(event);
}

