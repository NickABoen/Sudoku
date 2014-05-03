#include "MainController.h"
#include "difficultyselector.h"
#include "BoardGenerator.h"
#include "BoardSolver.h"
#include "Move.h"
#include "Puzzle.h"
#include "ImagePuzzleGenerator.h"
#include "../test.h"
#include "ScoreboardTableModel.h"

#include <string>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractButton>
#include <QAction>
#include <QPushButton>
#include <QLibrary>
#include <QTimer>
#include <QEventLoop>
#include <QThread>
#include <QInputDialog>

extern bool test;
extern std::ofstream testfile;
//typedef void(*functionPointer) ();
using namespace Controller;

bool clearRedoStack = true;
bool enableUndoRedo = true;
int numHints;

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
    scoreboardEnabled = false;
    ctimer = new QTimer(this);

    qDebug() << "Temporary Initialization of scoreboardModel";
    scoreboardModel = new ScoreboardTableModel(0,NULL);

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
    connect(&view, SIGNAL(onGenerateBoardFromImagePressed()), this, SLOT(onGenerateBoardFromImage()));
    connect(&view, SIGNAL(onHintPressed()), this, SLOT(onHint()));
    connect(&view, SIGNAL(onEnableNotesPressed()), this, SLOT(onEnableNotes()));
    connect(&view, SIGNAL(onEnableValidationPressed()), this, SLOT(onEnableValidation()));
    connect(&view, SIGNAL(onScoreBoardPressed()), this, SLOT(onToggleScoreBoard()));
    connect(&view, SIGNAL(onCluePressed()), this, SLOT(onClues()));
    connect(ctimer, SIGNAL(timeout()), this,SLOT(giveClues()));
    connect(&view, SIGNAL(closeThread()), this, SLOT(endThread()));

    numHints = 1;

    view.centralWidget()->setEnabled(false);
    timerThread = new GameTimer(&view);
    timerThread->setPuzzle(puzzle);
    //Show MainWindow
    view.show();
}
void MainController::endThread()
{
    timerThread->terminate();
    view.close();
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

    enableUndoRedo = false;

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
    enableUndoRedo = true;

    view.centralWidget()->setEnabled(true);
    view.undoAction->setEnabled(false);
    view.redoAction->setEnabled(false);
    view.clearAction->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////
// This function displays the current board on the gui
//////////////////////////////////////////////////////////////////////////////////
void MainController::displayCurrentBoard(){

    if(test) testfile << "MC5  ####################### MainController displayCurrentBoard #######################\n";

    enableUndoRedo = false;

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
    view.DisableScoreboardView();
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

        InitializeScoreBoard();

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
    //make move
    puzzle->currentBoard[x][y] = value;

    //Board Validation here
    bool valid = true;
    if(view.isValidationEnabled()) {
        if (value != 0) {
            valid = validateBoard(value,x,y);
            qDebug() << "Validation: " + QString::number(valid);
        }
    }
    if (!valid) {
        view.changeColor(x,y);
    } else {
        view.resetColor(x,y);
    }
    // check if completed
    if(puzzle->checkCompleted()){

        timerThread->paused = true;
        timerThread->resetTimer();

        if(test) testfile << "MC29 Puzzle is completed\n";
        QMessageBox msgBox;
        bool ok;
        QString playerName = QInputDialog::getText(msgBox.parentWidget(),tr("Input your name for your score"),
                                                   tr("Player Name:"),
                                                   QLineEdit::Normal,QDir::home().dirName(),&ok);


        if(ok)
        {
            qDebug() << "Adding Score to ScoreboardList";
            addScore(playerName, timerThread->currentTime);
            qDebug() << "Finished Adding Score";
        }

        msgBox.setText("You have sucessfully completed the puzzle!");
        msgBox.setInformativeText("Do you want to start a new puzzle or progress?");
        QAbstractButton *puzzleBtn = msgBox.addButton(trUtf8("Load Puzzle"), QMessageBox::YesRole);
        QAbstractButton *progressBtn = msgBox.addButton(trUtf8("Load Progress"), QMessageBox::NoRole);
        QAbstractButton *generateBtn = msgBox.addButton(trUtf8("Generate Puzzle"), QMessageBox::NoRole);
        QAbstractButton *generateImgBtn =  msgBox.addButton(trUtf8("Generate Puzzle from Image"), QMessageBox::NoRole);
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
            if(test) testfile << "MC40 MessageBox Generate Puzzle button clicked\n";
            onGenerateBoard();
        }

        else if (msgBox.clickedButton() == generateImgBtn){
            if(test) testfile << "MC40 MessageBox Generate Puzzle from Image button clicked\n";
            onGenerateBoardFromImage();
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
    view.DisableScoreboardView();
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
            //timerThread->resetTimer();
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
    qDebug() << "test1";
    view.notesEnabled = false;
    view.enableNotes->setChecked(false);
    view.clearLabels();
    puzzle = new Puzzle();
    qDebug() << "test2";
    DS->setFixedSize(192,145);
    qDebug() << "test3";
    DS->exec();
    qDebug() << "test4";
    int res = DS->DS;
    if(res > 0){
        puzzle->defaultBoard = BG->ConvertBoard(BG->Generate(res));
        puzzle->solvedBoard = BG->ConvertBoard(BG->FinalBoard);

        InitializeScoreBoard();

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

//////////////////////////////////////////////////////////////////////////////////
void MainController::onGenerateBoardFromImage()
{
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

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Board Image", "", "*.*");

    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first();
    if(filePath == "") return;

    ImagePuzzleGenerator gen;
    puzzle = gen.generate(filePath);

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

//////////////////////////////////////////////////////////////////////////////////
void MainController::onHint(){
    //this needs to be optimized.
    timerThread->runningTime += numHints *5000;
    numHints++;
    int x = rand() % 9;
    int y = rand() % 9;
    while(puzzle->currentBoard[x][y] != 0){
        x = rand() % 9;
        y = rand() % 9;
    }
    int moveArray[3] = {puzzle->solvedBoard[x][y], x, y};

    enableUndoRedo = false;
    view.setMove(moveArray, false);
    enableUndoRedo = true;

}

//////////////////////////////////////////////////////////////////////////////////
void MainController::onEnableNotes() {
    bool enabled = view.isNotesEnabled();
    view.setNotesEnabled(!enabled);
    view.enableNotes->setChecked(!enabled);
}

//////////////////////////////////////////////////////////////////////////////////
void MainController::onToggleScoreBoard()
{
    if(scoreboardEnabled)
    {
        view.DisableScoreboardView();
    }
    else
    {
        qDebug() << "Showing Board";
        scoreboardModel->debug();
        qDebug() << "\tTesting Board";
        view.EnableScoreboardView(scoreboardModel);
        qDebug() << "Board Shown";
    }
    scoreboardEnabled = !scoreboardEnabled;
}
void MainController::onEnableValidation() {
    qDebug() << "IN YO SHIZZ.";
    bool enabled = view.isValidationEnabled();
    view.setValidationEnabled(!enabled);
    view.enableValidation->setChecked(!enabled);
}

void MainController::InitializeScoreBoard()
{
    qDebug() << "Initializing Scoreboard";
    //qDebug() << scoreboardModel->debug().toUtf8().constData();
    delete scoreboardModel;
    scoreboardModel = new ScoreboardTableModel(0,&(puzzle->scoreboardList));
    //view.SetTableViewModel(scoreboardModel);
    view.EnableScoreboardView(scoreboardModel);
    qDebug()<<"Finished Initializing";
    //qDebug() << scoreboardModel->debug().toUtf8().constData();

}

void MainController::addScore(QString playerName, int score)
{
    if(scoreboardModel != NULL)
    {
        qDebug() << "Adding " << score << " to " << playerName.toUtf8().constData();
        scoreboardModel->addScore(playerName, score);
        qDebug() << "Setting Table View Model";
        view.SetTableViewModel(scoreboardModel);
        qDebug() << "Saving the Score";
        storeFilePath(puzzle->filePathRef);
    }

}

void MainController::onClues(){
    if (clueTimer) clueTimer = false;
    else clueTimer = true;
    view.clue->setChecked(clueTimer);
}

//////////////////////////////////////////////////////////////////////////////////
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

    if (puzzle->undo.isEmpty()) return;

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

    if(puzzle->redo.isEmpty()) return;

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
// This function is called when the current board and undo and redo stacks needs
// to be cleared.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onClear(){

    if(test) testfile << "MC35 ####################### MainController onClear #######################\n";

    // Clear everything
    view.clearBoard();
    displayDefaultBoard();
}

bool MainController::checkIfUnsavedProgressAndReset()
{
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
        }
        else if(ret == msgBox.Discard){
            //Discard user progress
            //Do nothing
        }
        else if(ret == msgBox.Cancel){
            //Don't save or discard progress
            return false;
        }

        //User done saving/discarding game, clear board and delete puzzle
        view.clearBoard();
        delete puzzle;

    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////
bool MainController::validateBoard(int value, int x, int y) {
    //rows and cols
    for (int i = 0; i < 9; i++) {
        if (i != x && puzzle->currentBoard[i][y] == value) {
            return false;
        }
        if (i != y && puzzle->currentBoard[x][i] == value) {
            return false;
        }
    }
    //section
    int sectionX = x/3;
    int sectionY = y/3;
    int secX = sectionX*3;
    int secY = sectionY*3;

    for(int i=secX; i <= secX + 2; i++) {
        for(int j = secY; j <= secY + 2; j++) {
            if (i != x && j != y) {
                if (puzzle->currentBoard[i][j] == value) {
                    return false;
                }
            }
        }
    }
    return true;
}

//included as parameter for "quiet" saving so that
//the user doesn't need to select the puzzle file in order
//to update the scoreboard list
void MainController::storeFilePath(QString filePath)
{
    qDebug() << "Entering File Serialization";
    if(filePath == "")
    {
        qDebug()<<"\tOpening File Dialog";
        QFileDialog* fileDialog = new QFileDialog();
        filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.txt");
    }
    else
    {
        qDebug() << "Soft saving at: " << filePath.toUtf8().constData();
        qDebug() << scoreboardModel->debug().toUtf8().constData();
    }

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
