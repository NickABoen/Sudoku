#include "MainController.h"
#include "Move.h"
#include "Puzzle.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractButton>

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

    view.centralWidget()->setEnabled(false);

    //Show MainWindow
    view.show();
}

MainController::~MainController(){

    if(puzzle != NULL) delete puzzle;

}

void MainController::displayBoard(){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(puzzle->defaultBoard[i][j] != 0){
                int moveArray[3] = {puzzle->defaultBoard[i][j], i, j};
                view.setDefaultMove(moveArray);
            }
        }
    }
}

void MainController::onLoadProgress(){

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Progress", "", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first();
    if(filePath != "")
    {
        qDebug(filePath.toLatin1());
        currentProgressSerializer.deserialize(puzzle, filePath);
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

        displayBoard();
        view.centralWidget()->setEnabled(true);
    }
}

void MainController::onSavePuzzle(){

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog();
    filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.*");

    if(filePath != "")
    {
        qDebug(filePath.toLatin1());
        puzzleSerializer.serialize(puzzle, filePath);
    }
}

void MainController::onMakeMove(int* moveArray){

    int value = moveArray[0];
    int x = moveArray[1];
    int y = moveArray[2];

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
    //TODO
    qDebug("UNDO PRESSED");
}

void MainController::onRedoMove(){
    //TODO
    qDebug("REDO PRESSED");
}
