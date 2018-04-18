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

SOURCE_PATH=$${_PRO_FILE_PWD_}
RELEASE_DESTINATION_PATH=$${OUT_PWD}/release
DEBUG_DESTINATION_PATH=$${OUT_PWD}/debug


win32
{
    RC_FILE = $${_PRO_FILE_PWD_}/win.rc
}

QMAKE_POST_LINK = "xcopy /y /e /i $$quote($$shell_path($$clean_path($${SOURCE_PATH}/config))) $$quote($$shell_path($$clean_path($${DEBUG_DESTINATION_PATH}/config)))"
QMAKE_POST_LINK += " && xcopy /y /e /i $$quote($$shell_path($$clean_path($${SOURCE_PATH}/config))) $$quote($$shell_path($$clean_path($${RELEASE_DESTINATION_PATH}/config)))"

FORMS += \
    mainwindow.ui \
    newtaskdialog.ui \
    settingsdialog.ui \
    taskitemdialog.ui \
    menudialog.ui \
    executeform.ui

HEADERS += \
    mainwindow.h \
    graphicsview.h \
    detailgraphicsitem.h \
    filesystemmodel.h \
    newtaskdialog.h \
    settingsdialog.h \
    scriptext.h \
    programaction.h \
    tableitemdelegate.h \
    scriptcontrol.h \
    ncl/ImportExport/nclCBS.h \
    ncl/ImportExport/nclCommonImport.h \
    ncl/ImportExport/nclDBS.h \
    ncl/ImportExport/nclDXF.h \
    ncl/ImportExport/dbs/define.h \
    ncl/ImportExport/dbs/Detail.h \
    ncl/ImportExport/dbs/DetailList.h \
    ncl/ImportExport/dbs/Doc.h \
    ncl/ImportExport/dbs/Figure.h \
    ncl/ImportExport/dbs/FigureList.h \
    ncl/ImportExport/dbs/InFile.h \
    ncl/ImportExport/dbs/ListInFile.h \
    ncl/ImportExport/dbs/ListOfDetail.h \
    ncl/ImportExport/dbs/ListOfFigure.h \
    ncl/ImportExport/dbs/ListOfPoint.h \
    ncl/ImportExport/dbs/ListOfSpline.h \
    ncl/ImportExport/dbs/Point.h \
    ncl/ImportExport/dbs/Segment.h \
    ncl/ImportExport/dbs/Spline.h \
    ncl/ImportExport/dbs/SplineList.h \
    ncl/ImportExport/dbs/Transform.h \
    ncl/nclFile.h \
    common.h \
    task.h \
    ncl/nclDetail.h \
    ncl/nclNest.h \
    ncl/nclMatrix.h \
    ncl/nclSpline.h \
    ncl/nclTransformView.h \
    ncl/nclSegment.h \
    ncl/nclPoint.h \
    ncl/nclVector.h \
    ncl/nclRect.h \
    ncl/nclTask.h \
    ncl/nclRastr.h \
    ncl/nclPixelCoord.h \
    ncl/nclTaskFile.h \
    ncl/nclFont.h \
    ncl/nclSymbol.h \
    ncl/define.h \
    statusbar.h \
    taskitemdialog.h \
    propertylist.h \
    highlighter.h \
    menudialog.h \
    executeform.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    graphicsview.cpp \
    detailgraphicsitem.cpp \
    filesystemmodel.cpp \
    newtaskdialog.cpp \
    settingsdialog.cpp \
    scriptext.cpp \
    programaction.cpp \
    tableitemdelegate.cpp \
    scriptcontrol.cpp \
    ncl/ImportExport/nclCBS.cpp \
    ncl/ImportExport/nclCommonImport.cpp \
    ncl/ImportExport/nclDBS.cpp \
    ncl/ImportExport/nclDXF.cpp \
    ncl/ImportExport/dbs/Detail.cpp \
    ncl/ImportExport/dbs/DetailList.cpp \
    ncl/ImportExport/dbs/Doc.cpp \
    ncl/ImportExport/dbs/Figure.cpp \
    ncl/ImportExport/dbs/FigureList.cpp \
    ncl/ImportExport/dbs/InFile.cpp \
    ncl/ImportExport/dbs/ListInFile.cpp \
    ncl/ImportExport/dbs/ListOfDetail.cpp \
    ncl/ImportExport/dbs/ListOfFigure.cpp \
    ncl/ImportExport/dbs/ListOfPoint.cpp \
    ncl/ImportExport/dbs/ListOfSpline.cpp \
    ncl/ImportExport/dbs/Point.cpp \
    ncl/ImportExport/dbs/Segment.cpp \
    ncl/ImportExport/dbs/Spline.cpp \
    ncl/ImportExport/dbs/SplineList.cpp \
    ncl/ImportExport/dbs/Transform.cpp \
    ncl/nclFile.cpp \
    task.cpp \
    ncl/nclDetail.cpp \
    ncl/nclNest.cpp \
    ncl/nclMatrix.cpp \
    ncl/nclSpline.cpp \
    ncl/nclTransformView.cpp \
    ncl/nclSegment.cpp \
    ncl/nclPoint.cpp \
    ncl/nclVector.cpp \
    ncl/nclRect.cpp \
    ncl/nclTask.cpp \
    ncl/nclRastr.cpp \
    ncl/nclPixelCoord.cpp \
    ncl/nclTaskFile.cpp \
    ncl/nclFont.cpp \
    ncl/nclSymbol.cpp \
    statusbar.cpp \
    taskitemdialog.cpp \
    propertylist.cpp \
    highlighter.cpp \
    menudialog.cpp \
    executeform.cpp

RESOURCES += \
    rc.qrc

DISTFILES += \
    config/sirius_menu.xml \
    config/scripts/ncl.js \
    config/sirius_property.xml \
    win.rc \
    config/scripts/pgd.js \
    config/scripts/pgd_edit.js \
    config/scripts/dxf_import.js \
    config/scripts/wsrs.js \
    config/scripts/wsrs_cut.js \
    config/scripts/wsrs_edit.js \
    config/scripts/rm-launch.js \
    config/scripts/costsharp.js \
    config/scripts/laser.js \
    config/scripts/kometa.js \
    config/scripts/trum3050.js \
    config/scripts/esab.js \
    config/scripts/common/savetask.js \
    config/scripts/editp.js \
    config/scripts/ncm30.js \
    config/scripts/ncm.js \
    config/scripts/pdbs.js \
    config/scripts/report.js \
    config/scripts/td.js \
    config/scripts/wwwdb.js
