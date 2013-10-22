QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sudoku-qt
TEMPLATE = app

SOURCES += View/mainwindow.cpp

HEADERS += View/mainwindow.h

FORMS += View/mainwindow.ui
