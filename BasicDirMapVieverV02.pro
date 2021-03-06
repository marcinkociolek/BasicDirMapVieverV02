#-------------------------------------------------
#
# Project created by QtCreator 2020-02-04T10:39:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BasicDirMapVieverV02
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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp\
        ../../ProjectsLib/LibMarcin/DispLib.cpp \
        ../../ProjectsLib/LibMarcin/StringFcLib.cpp \
        ../../ProjectsLib/LibMarcin/RegionU16Lib.cpp\
        ../../ProjectsLib/LibMarcin/tileparams.cpp  \
        ../../ProjectsLib/LibMarcin/dirdetectionparams.cpp \
        ../../ProjectsLib/LibMarcin/NormalizationLib.cpp \
        ../../ProjectsLib/LibMarcin/displayfordirdetection.cpp

HEADERS += \
        mainwindow.h\
        ../../ProjectsLib/LibMarcin/DispLib.h \
        ../../ProjectsLib/LibMarcin/StringFcLib.h \
        ../../ProjectsLib/LibMarcin/RegionU16Lib.h\
        ../../ProjectsLib/LibMarcin/tileparams.h \
        ../../ProjectsLib/LibMarcin/dirdetectionparams.h \
        ../../ProjectsLib/LibMarcin/NormalizationLib.h \
        ../../ProjectsLib/LibMarcin/displayfordirdetection.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += /usr/include/boost/
INCLUDEPATH += ../../ProjectsLib/LibMarcin/
INCLUDEPATH += ../SkopiaSegment/
INCLUDEPATH +=/home/marcin/Documents/ProjectsForeign/usr/opencv541CudaDynamicGTK2/include/opencv4/


LIBS += -ltiff
LIBS += -lboost_filesystem
LIBS += -lboost_regex

LIBS += -L/home/marcin/Documents/ProjectsForeign/usr/opencv541CudaDynamicGTK2/lib/

LIBS += -lopencv_highgui
LIBS += -lopencv_core
LIBS += -lopencv_imgproc

LIBS += -lopencv_features2d
LIBS += -lopencv_imgcodecs

LIBS += -lopencv_dnn

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
