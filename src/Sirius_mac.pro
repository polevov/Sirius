#-------------------------------------------------
#
# Project created by QtCreator 2018-01-10T11:49:04
#
#-------------------------------------------------
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets qml

TARGET = Sirius
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

FORMS += \
    mainwindow.ui \
    newtaskdialog.ui \
    settingsdialog.ui

HEADERS += \
    mainwindow.h \
    graphicsview.h \
    detailgraphicsitem.h \
    ncl/define.h \
    ncl/ImportExport/dbs/Segment.h \
    ncl/ImportExport/dbs/nclVector.h \
    ncl/ImportExport/dbs/Point.h \
    ncl/ImportExport/dbs/Transform.h \
    ncl/ImportExport/dbs/DetailList.h \
    ncl/ImportExport/dbs/doc.h \
    ncl/ImportExport/dbs/Figure.h \
    ncl/ImportExport/dbs/InFile.h \
    ncl/ImportExport/dbs/ListInFile.h \
    ncl/ImportExport/dbs/ListOfDetail.h \
    ncl/ImportExport/dbs/ListOfFigure.H \
    ncl/ImportExport/dbs/ListOfPoint.h \
    ncl/ImportExport/dbs/ListOfSpline.h \
    ncl/ImportExport/dbs/Spline.h \
    ncl/ImportExport/dbs/SplineList.h \
    ncl/ImportExport/dbs/Detail.h \
    ncl/ImportExport/dbs/FigureList.h \
    filesystemmodel.h \
    ncltask.h \
    newtaskdialog.h \
    settingsdialog.h \
    programaction.h \
    scriptext.h



SOURCES += \
    main.cpp \
    mainwindow.cpp \
    graphicsview.cpp \
    detailgraphicsitem.cpp \
    ncl/ImportExport/dbs/Segment.cpp \
    ncl/ImportExport/dbs/nclVector.cpp \
    ncl/ImportExport/dbs/Point.cpp \
    ncl/ImportExport/dbs/DetailList.cpp \
    ncl/ImportExport/dbs/Spline.cpp \
    ncl/ImportExport/dbs/SplineList.cpp \
    ncl/ImportExport/dbs/Transform.cpp \
    ncl/ImportExport/dbs/Doc.cpp \
    ncl/ImportExport/dbs/Figure.cpp \
    ncl/ImportExport/dbs/FigureList.cpp \
    ncl/ImportExport/dbs/InFile.cpp \
    ncl/ImportExport/dbs/ListInFile.cpp \
    ncl/ImportExport/dbs/ListOfDetail.cpp \
    ncl/ImportExport/dbs/ListOfFigure.cpp \
    ncl/ImportExport/dbs/ListOfPoint.cpp \
    ncl/ImportExport/dbs/ListOfSpline.cpp \
    ncl/ImportExport/dbs/OutFile.cpp \
    ncl/ImportExport/dbs/Detail.cpp \
    filesystemmodel.cpp \
    ncltask.cpp \
    newtaskdialog.cpp \
    settingsdialog.cpp \
    programaction.cpp \
    scriptext.cpp

RESOURCES += \
    rc.qrc


