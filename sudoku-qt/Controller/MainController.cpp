#include "MainController.h"
#include "difficultyselector.h"
#include "BoardGenerator.h"
#include "Move.h"
#include "Puzzle.h"
#include "../test.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractButton>
#include <QAction>
#include <QPushButton>

extern bool test;
extern std::ofstream testfile;

using namespace Controller;

bool clearRedoStack = true;
bool enableUndoRedo = true;

//////////////////////////////////////////////////////////////////////////////////
// Constructor for MainController
//////////////////////////////////////////////////////////////////////////////////
MainController::MainController(): QObject(NULL),
    view(),
    puzzle(),
    currentProgressSerializer(),
    puzzleSerializer()
{
    if(test) testfile << "MC1  ####################### MainController constructor #######################\n";

    //Connect all signals and slots
    connect(&view, SIGNAL(onUndoPressed()), this, SLOT(onUndoMove()));
    connect(&view, SIGNAL(onRedoPressed()), this, SLOT(onRedoMove()));
    connect(&view, SIGNAL(onClearPressed()), this, SLOT(onClear()));
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

//////////////////////////////////////////////////////////////////////////////////
// Destructor for MainController
//////////////////////////////////////////////////////////////////////////////////
MainController::~MainController(){

    if(test) testfile << "MC2  ####################### MainController destructor #######################\n";

    if(puzzle != NULL) delete puzzle;
}

//////////////////////////////////////////////////////////////////////////////////
// This function displays the default board on the gui
//////////////////////////////////////////////////////////////////////////////////
void MainController::displayDefaultBoard(){

    if(test) testfile << "MC3  ####################### MainController displayDefalutBoard #######################\n";

    //For all values in default board display on gui if non-zero
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(puzzle->defaultBoard[i][j] != 0){
                if(test) testfile << "MC4  Setting board values at " << i << "," << j << "=" << puzzle->currentBoard[i][j] << "\n";
                int moveArray[3] = {puzzle->defaultBoard[i][j], i, j};
                view.setMove(moveArray, false);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function displays the current board on the gui
//////////////////////////////////////////////////////////////////////////////////
void MainController::displayCurrentBoard(){

    if(test) testfile << "MC5  ####################### MainController displayCurrentBoard #######################\n";

    //For all values in current board display on gui if not a default value
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(puzzle->currentBoard[i][j] != puzzle->defaultBoard[i][j]){
                if(test) testfile << "MC6  Setting board values at " << i << "," << j << "=" << puzzle->currentBoard[i][j] << "\n";
                int moveArray[3] = {puzzle->currentBoard[i][j], i, j};
                view.setMove(moveArray, true);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when progress needs to be loaded from a text file.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onLoadProgress() {

    if(test) testfile << "MC7  ####################### MainController onLoadProgress #######################\n";

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Progress", "", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first();
    if(filePath != "")
    {
        if(test) testfile << "MC8  FilePath is not empty\n";
        if(puzzle != NULL){
            if(test) testfile << "MC36 Puzzle is not NULL\n";
            //User started game, do they want to save progress?
            QMessageBox msgBox;
            msgBox.setText("You have unsaved progress.");
            msgBox.setInformativeText("Do you want to save your progress?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();

            if(ret == msgBox.Save){
                if(test) testfile << "MC37 MessageBox save button clicked\n";
                //Save user progress
                onSaveProgress();
            }
            else if(ret == msgBox.Discard){
                if(test) testfile << "MC38 MessageBox discard button clicked\n";
                //Discard user progress
                //Do nothing
            }
            else if(ret == msgBox.Cancel){
                if(test) testfile << "MC39 MessageBox cancel button clicked\n";
                //Don't save or discard progress
                return;
            }

            //User done saving/discarding game, clear board and delete puzzle
            view.clearBoard();
            delete puzzle;
        }

        puzzle = currentProgressSerializer.deserialize(filePath, puzzleSerializer);

        //Display the default and current board
        enableUndoRedo = false;
        displayDefaultBoard();
        displayCurrentBoard();
        enableUndoRedo = true;

        if(puzzle->undo.count() > 0){
            if(test) testfile << "MC9  Puzzle undo count > 0\n";
            view.clearAction->setEnabled(true);
            view.undoAction->setEnabled(true);
        }
        if(puzzle->redo.count() > 0){
            if(test) testfile << "MC10 Puzzle redo count > 0\n";
            view.redoAction->setEnabled(true);
        }

        view.centralWidget()->setEnabled(true);
    } 
    else{
        if(test) testfile << "MC11 Filepath was empty\n";
        //Popup error message...
        //TODO
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when progress needs to be saved to a text file.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onSaveProgress(){

    if(test) testfile << "MC12 ####################### MainController onSaveProgress #######################\n";

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog();
    filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.*");

    if(filePath != "")
    {
        if(test) testfile << "MC13 FilePath is not empty\n";
        currentProgressSerializer.serialize(puzzle, filePath);
    }
    else{
        if(test) testfile << "MC14 Filepath was empty\n";
        //Popup error message...
        //TODO
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when a puzzle needs to be loaded from a text file.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onLoadPuzzle(){

    if(test) testfile << "MC15 ####################### MainController onLoadPuzzle #######################\n";

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Puzzle", "", "*.*");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first(); //If user specifies more than one file only take first??
    if(filePath != "")
    {
        if(test) testfile << "MC16 FilePath is not empty\n";
        if(puzzle != NULL){
            if(test) testfile << "MC17 Puzzle is not NULL\n";
            //User started game, do they want to save progress?
            QMessageBox msgBox;
            msgBox.setText("You have unsaved progress.");
            msgBox.setInformativeText("Do you want to save your progress?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();

            if(ret == msgBox.Save){
                if(test) testfile << "MC18 MessageBox save button clicked\n";
                //Save user progress
                onSaveProgress();
            }
            else if(ret == msgBox.Discard){
                if(test) testfile << "MC19 MessageBox discard button clicked\n";
                //Discard user progress
                //Do nothing
            }
            else if(ret == msgBox.Cancel){
                if(test) testfile << "MC20 MessageBox cancel button clicked\n";
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
        view.clearAction->setEnabled(false);
    }
    else{
        if(test) testfile << "MC21 Filepath was empty\n";
        //Popup error message...
        //TODO
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This funciton is called when a puzzle needs to be saved to a text file.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onSavePuzzle(){

    if(test) testfile << "MC22 ####################### MainController onSavePuzzle #######################\n";

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog();
    filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.*");

    if(filePath != "")
    {
        if(test) testfile << "MC23 FilePath is not empty\n";
        puzzleSerializer.serialize(puzzle, filePath);
    }
    else{
        if(test) testfile << "MC24 Filepath was empty\n";
        //Popup error message...
        //TODO
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when a move is made and updates the current board.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onMakeMove(int* moveArray){

    if(test) testfile << "MC25 ####################### MainController onMakeMove #######################\n";

    int value = moveArray[0];
    int x = moveArray[1];
    int y = moveArray[2];

    // Deal with undo/redo
    if(enableUndoRedo){
        if(test) testfile << "MC26 UndoRedo is enabled\n";
        if(clearRedoStack){
            if(test) testfile << "MC27 ClearRedoStack is true\n";
            puzzle->undo.push(Model::Move( x, y, puzzle->currentBoard[x][y]));
            view.clearAction->setEnabled(true);
            view.undoAction->setEnabled(true);

            puzzle->redo.clear();
            view.redoAction->setEnabled(false);
        }
        else{
            if(test) testfile << "MC28 ClearRedoStack is false\n";
            clearRedoStack = true;
        }
    }

    // Make move and check if completed
    puzzle->currentBoard[x][y] = value;
    if(puzzle->checkCompleted()){
        if(test) testfile << "MC29 Puzzle is completed\n";
        QMessageBox msgBox;
        msgBox.setText("You have sucessfully completed the puzzle!");
        msgBox.setInformativeText("Do you want to start a new puzzle or progress?");
        QAbstractButton *puzzleBtn = msgBox.addButton(trUtf8("Load Puzzle"), QMessageBox::YesRole);
        QAbstractButton *progressBtn = msgBox.addButton(trUtf8("Load Progress"), QMessageBox::NoRole);
        msgBox.exec();

        if(msgBox.clickedButton() == puzzleBtn){
            if(test) testfile << "MC30 MessageBox Load Puzzle button clicked\n";
            onLoadPuzzle();
        }
        else if (msgBox.clickedButton() == progressBtn){
            if(test) testfile << "MC40 MessageBox Load Progress button clicked\n";
            onLoadProgress();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to generate a new puzzle.
//////////////////////////////////////////////////////////////////////////////////
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
    displayDefaultBoard();
    view.centralWidget()->setEnabled(true);
    /*QString tmp = QString::number(res);
    QMessageBox msgbox;
    msgbox.setInformativeText(tmp);
    msgbox.exec();*/
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when the last move needs to be undone.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onUndoMove(){

    if(test) testfile << "MC31 ####################### MainController onUndoMove #######################\n";

    Model::Move undoMove = puzzle->undo.pop();

    int x = undoMove.x;
    int y = undoMove.y;

    //Push inverse move onto redo stack and pop from undo stack
    puzzle->redo.push(Model::Move(x, y, puzzle->currentBoard[x][y]));
    view.redoAction->setEnabled(true);

    int moveArray[3] = {undoMove.value, x, y};
    clearRedoStack = false;
    view.makeMove(moveArray);
    if(puzzle->undo.size() == 0){
        if(test) testfile << "MC32 Puzzle undo stack size is 0\n";
        view.clearAction->setEnabled(false);
        view.undoAction->setEnabled(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when the previous move needs to be redone.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onRedoMove(){

    if(test) testfile << "MC33 ####################### MainController onRedoMove #######################\n";

    Model::Move redoMove = puzzle->redo.pop();

    int x = redoMove.x;
    int y = redoMove.y;

    //Push inverse move on undo stack and pop from redo stack
    puzzle->undo.push(Model::Move(x, y, puzzle->currentBoard[x][y]));
    view.undoAction->setEnabled(true);
    view.clearAction->setEnabled(true);

    int moveArray[3] = {redoMove.value, x, y};
    clearRedoStack = false;
    view.makeMove(moveArray);
    if(puzzle->redo.size() == 0){
        if(test) testfile << "MC34 Puzzle redo stack size is 0\n";
        view.redoAction->setEnabled(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when the current board, and undo and redo stacks needs
// to be cleared.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onClear(){

    if(test) testfile << "MC35 ####################### MainController onClear #######################\n";

    // Clear everything
    view.clearBoard();
    displayDefaultBoard();

    puzzle->undo.clear();
    puzzle->redo.clear();

    view.undoAction->setEnabled(false);
    view.redoAction->setEnabled(false);
    view.clearAction->setEnabled(false);
}
