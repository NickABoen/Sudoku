QT += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sudoku-qt
TEMPLATE = app

SOURCES += Controller/MainController.cpp

HEADERS += Controller/MainController.h
