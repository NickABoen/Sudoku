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
#include <QTimer>
#include <QEventLoop>
#include <QThread>

extern bool test;
extern std::ofstream testfile;
//typedef void(*functionPointer) ();
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
    clueTimer = false;
    ctimer = new QTimer(this);
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
    connect(&view, SIGNAL(onHintPressed()), this, SLOT(onHint()));
    connect(&view, SIGNAL(onEnableNotesPressed()), this, SLOT(onEnableNotes()));
    connect(&view, SIGNAL(onCluePressed()), this, SLOT(onClues()));
    connect(ctimer, SIGNAL(timeout()), this,SLOT(giveClues()));
    connect(&view, SIGNAL(view::close()), this, SLOT(endThread()));


    view.centralWidget()->setEnabled(false);
    timerThread = new GameTimer(&view);
    timerThread->setPuzzle(puzzle);
    //Show MainWindow
    view.show();
}
void MainController::endThread()
{
    timerThread->terminate();
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
    timerThread->paused = true;
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Progress", "", "*.txt");
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

        view.notesEnabled = false;
        view.enableNotes->setChecked(false);
        view.clearLabels();
        puzzle = currentProgressSerializer.deserialize(filePath, puzzleSerializer);

        //Display the default and current board
        timerThread->setPuzzle(puzzle);
        timerThread->resetTimer();
        timerThread->paused = false;
        timerThread->start();
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

    timerThread->paused = true;
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog();
    filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.txt");

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
    timerThread->resetTimer();
    timerThread->setPuzzle(puzzle);
    timerThread->paused = false;
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when a puzzle needs to be loaded from a text file.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onLoadPuzzle(){

    if(test) testfile << "MC15 ####################### MainController onLoadPuzzle #######################\n";
    timerThread->paused = true;
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Puzzle", "", "*.txt");
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
        view.notesEnabled = false;
        view.enableNotes->setChecked(false);
        view.clearLabels();
        puzzle = puzzleSerializer.deserialize(filePath);

        timerThread->setPuzzle(puzzle);
        timerThread->resetTimer();
        timerThread->paused = false;
        timerThread->start();
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

    storeFilePath();
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
        QAbstractButton *generateBtn = msgBox.addButton(trUtf8("Generate Puzzle"), QMessageBox::NoRole);
        msgBox.exec();

        if(msgBox.clickedButton() == puzzleBtn){
            if(test) testfile << "MC30 MessageBox Load Puzzle button clicked\n";
            onLoadPuzzle();
        }
        else if (msgBox.clickedButton() == progressBtn){
            if(test) testfile << "MC40 MessageBox Load Progress button clicked\n";
            onLoadProgress();
        }
        else if (msgBox.clickedButton() == generateBtn){
            if(test) testfile << "MC40 MessageBox Load Progress button clicked\n";
            onGenerateBoard();
        }
    }
    else if(clueTimer==true){
        ctimer->start(5000);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to generate a new puzzle.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onGenerateBoard(){
    DifficultySelector *DS = new DifficultySelector();
    BoardGenerator *BG = new BoardGenerator();
    timerThread->paused = true;
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
            onSavePuzzle();
            clueTimer = false;
            qDebug() << "Setting Puzzle Time";
            timerThread->setPuzzle(puzzle);
            timerThread->paused = false;
            timerThread->start();
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
    view.notesEnabled = false;
    view.enableNotes->setChecked(false);
    view.clearLabels();
    puzzle = new Puzzle();
    DS->setFixedSize(192,145);
    DS->exec();
    int res = DS->DS;
    if(res > 0){
        puzzle->defaultBoard = BG->ConvertBoard(BG->Generate(res));
        puzzle->solvedBoard = BG->ConvertBoard(BG->FinalBoard);

        enableUndoRedo = false;
        displayDefaultBoard();
        enableUndoRedo = true;

        view.centralWidget()->setEnabled(true);
        view.undoAction->setEnabled(false);
        view.redoAction->setEnabled(false);
        view.clearAction->setEnabled(false);
        storeFilePath();
        clueTimer = false;
        qDebug() << "Setting Puzzle Time";
        timerThread->resetTimer();
        timerThread->setPuzzle(puzzle);
        timerThread->paused = false;
        timerThread->start();
    }
}

void MainController::onHint(){
    //this needs to be optimized.
    int x = rand() % 9;
    int y = rand() % 9;
    while(puzzle->currentBoard[x][y] != 0){
        x = rand() % 9;
        y = rand() % 9;
    }
    int moveArray[3] = {puzzle->solvedBoard[x][y], x, y};
    view.setMove(moveArray, false);

}

void MainController::onEnableNotes() {
    //need to set up a disable function as well. Will get there
    bool enabled = view.isNotesEnabled();
    view.setNotesEnabled(!enabled);
    view.enableNotes->setChecked(!enabled);
}
void MainController::onClues(){
    if (clueTimer) clueTimer = false;
    else clueTimer = true;
    view.clue->setChecked(clueTimer);
}

void MainController::giveClues(){

    int x = rand() % 9;
    int y = rand() % 9;
    while(puzzle->currentBoard[x][y] != 0){
        x = rand() % 9;
        y = rand() % 9;
    }
    int moveArray[3] = {puzzle->solvedBoard[x][y], x, y};
    view.setMove(moveArray, false);
    view.changeColor(x, y);
    QEventLoop loop;
    QTimer::singleShot(2500, &loop, SLOT(quit()));
    loop.exec();
    view.clearMove(x,y);
    view.resetColor(x, y);
    ctimer->stop();
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when the last move needs to be undone.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onUndoMove(){

    if(test) testfile << "MC31 ####################### MainController onUndoMove #######################\n";

    Model::Move checkMove = puzzle->undo.top();
    if (!view.isFieldEnabled(checkMove.x, checkMove.y)) {
        //If the top value is locked (hints, etc), then remove it and don't use it.
        puzzle->undo.pop();
        if (puzzle->undo.isEmpty()) return;
    }

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

void MainController::storeFilePath() {
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog();
    filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.txt");

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
