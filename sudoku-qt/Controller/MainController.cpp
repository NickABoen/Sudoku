#include "MainController.h"
#include "Move.h"
#include "Puzzle.h"

#include <QDebug>
#include <QFileDialog>

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
    connect(&view, SIGNAL(onUndoPressed()), this, SLOT(onUndoMove()));
    connect(&view, SIGNAL(onRedoPressed()), this, SLOT(onRedoMove()));
    connect(&view, SIGNAL(onSavePuzzlePressed()), this, SLOT(onSavePuzzle()));
    connect(&view, SIGNAL(onLoadPuzzlePressed()), this, SLOT(onLoadPuzzle()));
    connect(&view, SIGNAL(onSaveProgressPressed()), this, SLOT(onSaveProgress()));
    connect(&view, SIGNAL(onLoadProgressPressed()), this, SLOT(onLoadProgress()));
    connect(&view, SIGNAL(onMakeMove(int*)), this, SLOT(onMakeMove(int*)));

    //Show MainWindow
    view.show();
}

MainController::~MainController(){
    //TODO
}

void MainController::onLoadProgress(){
    //TODO
    qDebug("LOAD PROGRESS PRESSED");
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Progress", "/", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first(); //If user specifies more than one file only take first??
    if(filePath != "")
    {
        //currentProgressSerializer.deserialize(&Puzzle, filePath);
        qDebug(filePath.toLatin1());
    }
}

void MainController::onSaveProgress(){
    //TODO
    qDebug("SAVE PROGRESS PRESSED");
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Save Progress", "/", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first(); //If user specifies more than one file only take first??
    if(filePath != "")
    {
        //currentProgressSerializer.serialize(&Puzzle, filePath);
        qDebug(filePath.toLatin1());
    }
}

void MainController::onLoadPuzzle(){
    //TODO
    qDebug("LOAD PUZZLE PRESSED");
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Puzzle", "/", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first(); //If user specifies more than one file only take first??
    if(filePath != "")
    {
        //puzzleSerializer.deserialize(&Puzzle, filePath);
        qDebug(filePath.toLatin1());
    }
}

void MainController::onSavePuzzle(){
    //TODO
    qDebug("SAVE PUZZLE PRESSED");
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Save Puzzle", "/", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first(); //If user specifies more than one file only take first??
    if(filePath != "")
    {
        //puzzleSerializer.serialize(&Puzzle, filePath);
        qDebug(filePath.toLatin1());
    }
}

void MainController::onMakeMove(int* moveArray){
    //TODO
    qDebug("Value:");
    qDebug(QString::number(moveArray[0]).toLatin1());
    qDebug("Row:");
    qDebug(QString::number(moveArray[1]).toLatin1());
    qDebug("Column:");
    qDebug(QString::number(moveArray[2]).toLatin1());
}


void MainController::onUndoMove(){
    //TODO
    qDebug("UNDO PRESSED");
}

void MainController::onRedoMove(){
    //TODO
    qDebug("REDO PRESSED");
}
