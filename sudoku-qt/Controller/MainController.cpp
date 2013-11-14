#include "MainController.h"
#include "Move.h"
#include "Puzzle.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractButton>
#include <QAction>

using namespace Controller;

bool clearRedoStack = true;
bool enableUndoRedo = true;

MainController::MainController(): QObject(NULL),
    view(),
    puzzle(),
    currentProgressSerializer(),
    puzzleSerializer()
{
    //Connect all signals and slots
    connect(&view, SIGNAL(onUndoPressed()), this, SLOT(onUndoMove()));
    connect(&view, SIGNAL(onRedoPressed()), this, SLOT(onRedoMove()));
    connect(&view, SIGNAL(onSavePuzzlePressed()), this, SLOT(onSavePuzzle()));
    connect(&view, SIGNAL(onLoadPuzzlePressed()), this, SLOT(onLoadPuzzle()));
    connect(&view, SIGNAL(onSaveProgressPressed()), this, SLOT(onSaveProgress()));
    connect(&view, SIGNAL(onLoadProgressPressed()), this, SLOT(onLoadProgress()));
    connect(&view, SIGNAL(onMakeMove(int*)), this, SLOT(onMakeMove(int*)));

    view.centralWidget()->setEnabled(false);

    //Show MainWindow
    view.show();
}

MainController::~MainController(){

    if(puzzle != NULL) delete puzzle;

}

void MainController::displayDefaultBoard(){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(puzzle->defaultBoard[i][j] != 0){
                int moveArray[3] = {puzzle->defaultBoard[i][j], i, j};
                view.setMove(moveArray, false);
            }
        }
    }
}

void MainController::displayCurrentBoard(){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(puzzle->currentBoard[i][j] != puzzle->defaultBoard[i][j]){
                int moveArray[3] = {puzzle->currentBoard[i][j], i, j};
                view.setMove(moveArray, true);
            }
        }
    }
}

void MainController::onLoadProgress() {
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Progress", "", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first();
    if(filePath != "")
    {
        qDebug(filePath.toLatin1());
        puzzle = currentProgressSerializer.deserialize(filePath, puzzleSerializer);

        enableUndoRedo = false;
        displayDefaultBoard();
        displayCurrentBoard();
        enableUndoRedo = true;

        if(puzzle->undo.count() > 0) view.undoAction->setEnabled(true);
        if(puzzle->redo.count() > 0) view.redoAction->setEnabled(true);

        view.centralWidget()->setEnabled(true);
    } 
    else{
        //Popup error message...
        //TODO
        qDebug("Error filepath empty");
    }
}

void MainController::onSaveProgress(){

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog();
    filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.*");

    if(filePath != "")
    {
        qDebug(filePath.toLatin1());
        currentProgressSerializer.serialize(puzzle, filePath);
    }
    else{
        //Popup error message...
        //TODO
        qDebug("Error filepath empty");
    }
}

void MainController::onLoadPuzzle(){

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Puzzle", "", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first(); //If user specifies more than one file only take first??
    if(filePath != "")
    {
        qDebug(filePath.toLatin1());
        if(puzzle != NULL){
            //User started game, do they want to save progress?
            QMessageBox msgBox;
            msgBox.setText("You have unsaved progress.");
            msgBox.setInformativeText("Do you want to save your progress?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();

            if(ret == msgBox.Save){
                //Save user progress
                onSaveProgress();
            }
            else if(ret == msgBox.Discard){
                //Discard user progress
                //Do nothing
            }
            else if(ret == msgBox.Cancel){
                //Don't save or discard progress
                return;
            }

            //User done saving/discarding game, clear board and delete puzzle
            view.clearBoard();
            delete puzzle;
        }
        puzzle = puzzleSerializer.deserialize(filePath);

        enableUndoRedo = false;
        displayDefaultBoard();
        enableUndoRedo = true;

        view.centralWidget()->setEnabled(true);

        view.undoAction->setEnabled(false);
        view.redoAction->setEnabled(false);
    }
    else{
        //Popup error message...
        //TODO
        qDebug("Error filepath empty");
    }
}

void MainController::onSavePuzzle(){

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog();
    filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.*");

    if(filePath != "")
    {
        puzzleSerializer.serialize(puzzle, filePath);
    }
    else{
        //Popup error message...
        //TODO
        qDebug("Error filepath empty");
    }
}

void MainController::onMakeMove(int* moveArray){

    int value = moveArray[0];
    int x = moveArray[1];
    int y = moveArray[2];

    if(enableUndoRedo){
        if(clearRedoStack){
            puzzle->undo.push(Model::Move( x, y, puzzle->currentBoard[x][y]));
            view.undoAction->setEnabled(true);

            puzzle->redo.clear();
            view.redoAction->setEnabled(false);
        }
        else{
           clearRedoStack = true;
        }
    }

    puzzle->currentBoard[x][y] = value;
    if(puzzle->checkCompleted()){
        QMessageBox msgBox;
        msgBox.setText("You have sucessfully completed the puzzle!");
        msgBox.setInformativeText("Do you want to start a new game?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Ok){
            onLoadPuzzle();
        }
    }
}


void MainController::onUndoMove(){

    Model::Move undoMove = puzzle->undo.pop();

    int x = undoMove.x;
    int y = undoMove.y;

    puzzle->redo.push(Model::Move(x, y, puzzle->currentBoard[x][y]));
    view.redoAction->setEnabled(true);

    int moveArray[3] = {undoMove.value, x, y};
    clearRedoStack = false;
    view.makeMove(moveArray);
    if(puzzle->undo.size() == 0) view.undoAction->setEnabled(false);
}

void MainController::onRedoMove(){

    Model::Move redoMove = puzzle->redo.pop();

    int x = redoMove.x;
    int y = redoMove.y;

    puzzle->undo.push(Model::Move(x, y, puzzle->currentBoard[x][y]));
    view.undoAction->setEnabled(true);

    int moveArray[3] = {redoMove.value, x, y};
    clearRedoStack = false;
    view.makeMove(moveArray);
    if(puzzle->redo.size() == 0) view.redoAction->setEnabled(false);
}
