#-------------------------------------------------
#
# Project created by QtCreator 2013-10-07T22:43:45
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sudoku-qt
TEMPLATE = app

SOURCES += main.cpp \
           test.h \
           test.cpp \

include(Model/Model.pri)
include(View/View.pri)
include(Controller/Controller.pri)


INCLUDEPATH += $$PWD/Model\
               $$PWD/View\
               $$PWD/Controller\
               $$PWD/Lib

DEPENDPATH += $$PWD/Model\
              $$PWD/View\
              $$PWD/Controller\
              $$PWD/Lib

LIBS += $$PWD/Lib/opencv_core248.lib\
        $$PWD/Lib/opencv_highgui248.lib\
        $$PWD/Lib/opencv_imgproc248.lib\
        $$PWD/Lib/libtesseract302.lib


FORMS +=

HEADERS +=
