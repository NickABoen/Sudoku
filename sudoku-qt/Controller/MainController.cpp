#include "MainController.h"
#include "difficultyselector.h"
#include "BoardGenerator.h"
#include "Move.h"
#include "Puzzle.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractButton>
#include <QAction>
#include <QPushButton>

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
    connect(&view, SIGNAL(onGenerateBoardPressed()), this, SLOT(onGenerateBoard()));

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
        currentProgressSerializer.deserialize(undo, puzzle, filePath);
    }
    displayBoard();


}

void MainController::onSaveProgress(){

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog();
    filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.*");

    if(filePath != "")
    {
        qDebug(filePath.toLatin1());
        currentProgressSerializer.serialize(undo, puzzle, filePath);
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

        undo.clear();
        redo.clear();

        view.undoAction->setEnabled(false);
        view.redoAction->setEnabled(false);
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
}

void MainController::onMakeMove(int* moveArray){

    int value = moveArray[0];
    int x = moveArray[1];
    int y = moveArray[2];

    undo.push(Model::Move( x, y, puzzle->currentBoard[x][y]));
    view.undoAction->setEnabled(true);

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

void MainController::onGenerateBoard(){
    DifficultySelector *DS = new DifficultySelector();
    BoardGenerator *BG = new BoardGenerator();
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
    puzzle = new Puzzle();
    DS->setFixedSize(192,145);
    DS->exec();
    int res = DS->DS;
    puzzle->defaultBoard = BG->ConvertBoard(BG->Generate(res));
    puzzle->solvedBoard = BG->ConvertBoard(BG->FinalBoard);
    displayBoard();
    view.centralWidget()->setEnabled(true);
    /*QString tmp = QString::number(res);
    QMessageBox msgbox;
    msgbox.setInformativeText(tmp);
    msgbox.exec();*/
}

void MainController::onUndoMove(){

    Model::Move undoMove = undo.pop();

    int x = undoMove.x;
    int y = undoMove.y;

    redo.push(Model::Move(x, y, puzzle->currentBoard[x][y]));
    view.redoAction->setEnabled(true);

    int moveArray[3] = {undoMove.value, x, y};
    view.makeMove(moveArray);
    undo.pop();
    if(undo.size() == 0) view.undoAction->setEnabled(false);
}

void MainController::onRedoMove(){

    Model::Move redoMove = redo.pop();

    int moveArray[3] = {redoMove.value, redoMove.x, redoMove.y};
    view.makeMove(moveArray);
    if(redo.size() == 0) view.redoAction->setEnabled(false);

    //undo.push(redoMove);
    view.undoAction->setEnabled(true);
}

Model::Move MainController::GetMoveOfCurrentPuzzle(int x, int y){

    Model::Move returnMove(x, y, puzzle->currentBoard[x][y]);

    qDebug("New Move: ["+QString::number(returnMove.x).toLatin1()+", "+QString::number(returnMove.y).toLatin1()+"] = "+QString::number(returnMove.value).toLatin1());

    return returnMove;
}
