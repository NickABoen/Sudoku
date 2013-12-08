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
           test.cpp

include(Model/Model.pri)
include(View/View.pri)
include(Controller/Controller.pri)

INCLUDEPATH += $$PWD/Model\
               $$PWD/View\
               $$PWD/Controller

DEPENDPATH += $$PWD/Model\
              $$PWD/View\
              $$PWD/Controller

FORMS +=
