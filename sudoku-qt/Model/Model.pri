QT += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sudoku-qt
TEMPLATE = app

SOURCES += Model/CurrentProgressSerializer.cpp\
           Model/Move.cpp\
           Model/Puzzle.cpp\
           Model/PuzzleSerializer.cpp\
           Model/Boardsolver.cpp \
           Model/PriorityQueue.cpp \
           Model/BoardGenerator.cpp

HEADERS += Model/CurrentProgressSerializer.h\
           Model/Move.h\
           Model/Puzzle.h\
           Model/PuzzleSerializer.h \
           Model/BoardSolver.h \
           Model/PriorityQueue.h \
           Model/BoardGenerator.h
