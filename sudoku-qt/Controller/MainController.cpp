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

void MainController::displayDefaultBoard(){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(puzzle->defaultBoard[i][j] != 0){
                int moveArray[3] = {i, j, puzzle->defaultBoard[i][j]};
                view.setDefaultMove(moveArray);
            }
        }
    }
}

void MainController::onLoadProgress(){
    //TODO
    qDebug("LOAD PROGRESS PRESSED");
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Progress", "/", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first(); //If user specifies more than one file only take first??
    if(filePath != "")
    {
        qDebug(filePath.toLatin1());
        puzzle->currentBoard = currentProgressSerializer.deserialize(filePath);
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
        qDebug(filePath.toLatin1());
        currentProgressSerializer.serialize(puzzle, filePath);
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
        qDebug(filePath.toLatin1());
        puzzle = puzzleSerializer.deserialize(filePath);

        displayDefaultBoard();
        undo.clear();
        redo.clear();
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
        qDebug(filePath.toLatin1());
        puzzleSerializer.serialize(puzzle, filePath);
    }
}

void MainController::onMakeMove(int* moveArray){
    //TODO
    Model::Move* oldMove = GetMoveOfCurrentPuzzle(moveArray[1], moveArray[2]);
    undo.push(*oldMove);
}


void MainController::onUndoMove(){
    //TODO
    qDebug("UNDO PRESSED");

    if(undo.size() > 0)
    {
        Model::Move undoMove = undo.pop();
        Model::Move* redoMove = GetMoveOfCurrentPuzzle(undoMove.x, undoMove.y);
        puzzle->makeMove(undoMove);
        redo.push(*redoMove);
    }
}

void MainController::onRedoMove(){
    //TODO
    qDebug("REDO PRESSED");

    if(redo.size() > 0)
    {
        Model::Move redoMove = redo.pop();
        Model::Move* undoMove = GetMoveOfCurrentPuzzle(redoMove.x, redoMove.y);
        puzzle->makeMove(redoMove);
        undo.push(*undoMove);
    }
}

Model::Move* MainController::GetMoveOfCurrentPuzzle(int x, int y){
    Model::Move* returnMove = new Model::Move();

    returnMove->x = x;
    returnMove->y = y;

    returnMove->value = puzzle->currentBoard[x][y];

    qDebug("New Move: ["+QString::number(returnMove->x).toLatin1()+", "+QString::number(returnMove->y).toLatin1()+"] = "+QString::number(returnMove->value).toLatin1());

    return returnMove;
}
