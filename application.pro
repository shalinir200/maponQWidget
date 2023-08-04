#-------------------------------------------------
#
# Project created by QtCreator 2023-06-05T15:25:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = application
TEMPLATE = app

#for GL
INCLUDEPATH += /opt/Qt5.4.2/5.4/Src/qtwebengine/src/3rdparty/chromium/third_party/mesa/src/include/

MOC_DIR=./MOC
UI_DIR=./UI
OBJECTS_DIR=./OBJ

#DEFINES += INCLUDE_S57

CAIRGIS_INCLUDE = ../CAIRGISLibs/include/CAIRGIS
SUPPORT_INCLUDE = ../CAIRGISLibs/include/Support

INCLUDEPATH += .
INCLUDEPATH += $$CAIRGIS_INCLUDE/
INCLUDEPATH += $$CAIRGIS_INCLUDE/CGKernel
INCLUDEPATH += $$SUPPORT_INCLUDE

LIBS += -L../CAIRGISLibs/lib/libSupport

LIBS += -lproj
LIBS += -lspatialite
LIBS += -lrasterlite
LIBS += -lgeos
LIBS += -lgeos_c
LIBS += -lgdal

LIBS += -L../CAIRGISLibs/lib/libCAIRGIS

LIBS += -lKernel
LIBS += -lGeoEvents
LIBS += -lQueryAnalysis
LIBS += -lConnection
LIBS += -lProjection
LIBS += -lGeoComputation
#LIBS += -lS52Renderer
#LIBS += -lS57ReadWrite
LIBS += -lOGRReadWrite
LIBS += -lOGRConverter
LIBS += -lDGNReadWrite
LIBS += -lIMGRS
LIBS += -lFileConnection


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
