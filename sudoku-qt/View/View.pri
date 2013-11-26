QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sudoku-qt
TEMPLATE = app

SOURCES += View/mainwindow.cpp \
    View/difficultyselector.cpp

HEADERS += View/mainwindow.h \
    View/difficultyselector.h

FORMS += View/mainwindow.ui \
    View/difficultyselector.ui
