#include "MainController.h"
#include "Move.h"
#include "Puzzle.h"

#include <QDebug>

using namespace Controller;

MainController::MainController(): QObject(NULL),
    view(),
    puzzle(),
    currentProgressSerializer(),
    puzzleSerializer(),
    undo(),
    redo()
{
    //Connect all signals and slots
    connect(&view, SIGNAL(onUndo()), this, SLOT(onUndoMove()));
    connect(&view, SIGNAL(onRedo()), this, SLOT(onRedoMove()));
    connect(&view, SIGNAL(onSavePuzzlePressed()), this, SLOT(onSavePuzzle()));
    connect(&view, SIGNAL(onLoadPuzzlePressed()), this, SLOT(onLoadPuzzle()));
    connect(&view, SIGNAL(onSaveProgressPressed()), this, SLOT(onSaveProgress()));
    connect(&view, SIGNAL(onLoadProgressPressed()), this, SLOT(onLoadProgress()));
    connect(&view, SIGNAL(onMakeMove(QString)), this, SLOT(onMakeMove(QString)));

    //Show MainWindow
    view.show();
}

MainController::~MainController(){
    //TODO
}

void MainController::onLoadProgress(){
    //TODO
    qDebug("LOAD PROGRESS PRESSED");
}

void MainController::onSaveProgress(){
    //TODO
    qDebug("SAVE PROGRESS PRESSED");
}

void MainController::onLoadPuzzle(){
    //TODO
    qDebug("LOAD PUZZLE PRESSED");
}

void MainController::onSavePuzzle(){
    //TODO
    qDebug("SAVE PUZZLE PRESSED");
}

void MainController::onMakeMove(QString text){
    //TODO
    qDebug(text.toLatin1());
}


void MainController::onUndoMove(){
    //TODO
    qDebug("UNDO PRESSED");
}

void MainController::onRedoMove(){
    //TODO
    qDebug("REDO PRESSED");
}
