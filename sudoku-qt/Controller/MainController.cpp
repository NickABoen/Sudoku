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
#include <qDebug>
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
    clueTimer = false;
    scoreboardEnabled = false;
    ctimer = new QTimer(this);

    //qDebug() << "Temporary Initialization of scoreboardModel";
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

    if(puzzle != NULL) delete puzzle;
}

//////////////////////////////////////////////////////////////////////////////////
// This function displays the default board on the gui
//////////////////////////////////////////////////////////////////////////////////
void MainController::displayDefaultBoard(){
    enableUndoRedo = false;

    //For all values in default board display on gui if non-zero
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(puzzle->defaultBoard[i][j] != 0){
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



    enableUndoRedo = false;

    //For all values in current board display on gui if not a default value
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(puzzle->currentBoard[i][j] != puzzle->defaultBoard[i][j]){

                int moveArray[3] = {puzzle->currentBoard[i][j], i, j};
                view.setMove(moveArray, true);
            }
        }
    }

    enableUndoRedo = true;

    if(puzzle->undo.count() > 0){

        view.clearAction->setEnabled(true);
        view.undoAction->setEnabled(true);
    }
    if(puzzle->redo.count() > 0){

        view.redoAction->setEnabled(true);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when progress needs to be loaded from a text file.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onLoadProgress() {


    timerThread->paused = true;
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Progress", "", "*.txt");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first();
    if(filePath != "")
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

        view.notesEnabled = false;
        view.enableNotes->setChecked(false);
        view.clearLabels();
        puzzle = currentProgressSerializer.deserialize(filePath, puzzleSerializer);

        //Display the default and current board
        timerThread->setPuzzle(puzzle);
        timerThread->resetTimer();
        timerThread->paused = false;

        displayDefaultBoard();
        displayCurrentBoard();

        if(puzzle->undo.count() > 0){

            view.clearAction->setEnabled(true);
            view.undoAction->setEnabled(true);
        }
        if(puzzle->redo.count() > 0){

            view.redoAction->setEnabled(true);
        }

    }
    else{

        //Popup error message...
        //TODO
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when progress needs to be saved to a text file.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onSaveProgress(){



    timerThread->paused = true;
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog();
    filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.txt");

    if(filePath != "")
    {

        currentProgressSerializer.serialize(puzzle, filePath);
    }
    else{

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


    timerThread->paused = true;
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Load Puzzle", "", "*.txt");
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first(); //If user specifies more than one file only take first??
    view.DisableScoreboardView();
    if(filePath != "")
    {


        checkIfUnsavedProgressAndReset();

        view.notesEnabled = false;
        view.enableNotes->setChecked(false);
        view.clearLabels();
        puzzle = puzzleSerializer.deserialize(filePath);

        InitializeScoreBoard();

        timerThread->setPuzzle(puzzle);
        timerThread->resetTimer();
        timerThread->paused = false;
        timerThread->start();

        displayDefaultBoard();
    }
    else{

        //Popup error message...
        //TODO
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This funciton is called when a puzzle needs to be saved to a text file.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onSavePuzzle(){



    storeFilePath();
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when a move is made and updates the current board.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onMakeMove(int* moveArray){



    int value = moveArray[0];
    int x = moveArray[1];
    int y = moveArray[2];

    // Deal with undo/redo
    if(enableUndoRedo){

        if(clearRedoStack){

            puzzle->undo.push(Model::Move( x, y, puzzle->currentBoard[x][y]));
            view.clearAction->setEnabled(true);
            view.undoAction->setEnabled(true);

            puzzle->redo.clear();
            view.redoAction->setEnabled(false);
        }
        else{

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
            //qDebug() << "Validation: " + QString::number(valid);
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


        QMessageBox msgBox;
        bool ok;
        QString playerName = QInputDialog::getText(msgBox.parentWidget(),tr("Input your name for your score"),
                                                   tr("Player Name:"),
                                                   QLineEdit::Normal,QDir::home().dirName(),&ok);


        if(ok)
        {
            //qDebug() << "Adding Score to ScoreboardList";
            addScore(playerName, timerThread->currentTime);
            //qDebug() << "Finished Adding Score";
        }

        msgBox.setText("You have sucessfully completed the puzzle!");
        msgBox.setInformativeText("Do you want to start a new puzzle or progress?");
        QAbstractButton *puzzleBtn = msgBox.addButton(trUtf8("Load Puzzle"), QMessageBox::YesRole);
        QAbstractButton *progressBtn = msgBox.addButton(trUtf8("Load Progress"), QMessageBox::NoRole);
        QAbstractButton *generateBtn = msgBox.addButton(trUtf8("Generate Puzzle"), QMessageBox::NoRole);
        QAbstractButton *generateImgBtn =  msgBox.addButton(trUtf8("Generate Puzzle from Image"), QMessageBox::NoRole);
        msgBox.exec();
        if(msgBox.clickedButton() == puzzleBtn){

            onLoadPuzzle();
        }
        else if (msgBox.clickedButton() == progressBtn){

            onLoadProgress();
        }
        else if (msgBox.clickedButton() == generateBtn){

            onGenerateBoard();
        }

        else if (msgBox.clickedButton() == generateImgBtn){

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

    checkIfUnsavedProgressAndReset();

    //qDebug() << "test1";
    view.notesEnabled = false;
    view.enableNotes->setChecked(false);
    view.clearLabels();
    puzzle = new Puzzle();
    //qDebug() << "test2";
    DS->setFixedSize(192,145);
    //qDebug() << "test3";
    DS->exec();
    //qDebug() << "test4";
    int res = DS->DS;
    if(res > 0){
        puzzle->defaultBoard = BG->ConvertBoard(BG->Generate(res));
        puzzle->solvedBoard = BG->ConvertBoard(BG->FinalBoard);

        InitializeScoreBoard();
        displayDefaultBoard();

        storeFilePath();
        clueTimer = false;
        //qDebug() << "Setting Puzzle Time";
        timerThread->resetTimer();
        timerThread->setPuzzle(puzzle);
        timerThread->paused = false;
        timerThread->start();
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to generate a new puzzle from an existing image.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onGenerateBoardFromImage()
{
    timerThread->paused = true;

    checkIfUnsavedProgressAndReset();

    view.notesEnabled = false;
    view.enableNotes->setChecked(false);
    view.clearLabels();

    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Board Image", "", "*.*");

    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first();
    if(filePath == "") return;

    ImagePuzzleGenerator gen;
    puzzle = gen.generate(filePath);

    if(puzzle == NULL)
    {
        QMessageBox::about(&view, "Board Generation from Image", "Error: there was a problem while trying to generate board from image. Please try with a different image");
        return;
    }

    displayDefaultBoard();

    storeFilePath();
    clueTimer = false;
    //qDebug() << "Setting Puzzle Time";
    timerThread->resetTimer();
    timerThread->setPuzzle(puzzle);
    timerThread->paused = false;
    timerThread->start();

}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when a hint needs to be shown.
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
// This function is called to enable notes.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onEnableNotes() {
    bool enabled = view.isNotesEnabled();
    view.setNotesEnabled(!enabled);
    view.enableNotes->setChecked(!enabled);
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to show and hide the score board.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onToggleScoreBoard()
{
    if(scoreboardEnabled)
    {
        view.DisableScoreboardView();
    }
    else
    {
        //qDebug() << "Showing Board";
        scoreboardModel->debug();
        //qDebug() << "\tTesting Board";
        view.EnableScoreboardView(scoreboardModel);
        //qDebug() << "Board Shown";
    }
    scoreboardEnabled = !scoreboardEnabled;
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to enable move validation.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onEnableValidation() {
    //qDebug() << "IN YO SHIZZ.";
    bool enabled = view.isValidationEnabled();
    view.setValidationEnabled(!enabled);
    view.enableValidation->setChecked(!enabled);
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to initialize the score board.
//////////////////////////////////////////////////////////////////////////////////
void MainController::InitializeScoreBoard()
{
    //qDebug() << "Initializing Scoreboard";
    ////qDebug() << scoreboardModel->debug().toUtf8().constData();
    delete scoreboardModel;
    scoreboardModel = new ScoreboardTableModel(0,&(puzzle->scoreboardList));
    //view.SetTableViewModel(scoreboardModel);
    view.EnableScoreboardView(scoreboardModel);
    //qDebug()<<"Finished Initializing";
    ////qDebug() << scoreboardModel->debug().toUtf8().constData();

}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to add a score to the score board.
//////////////////////////////////////////////////////////////////////////////////
void MainController::addScore(QString playerName, int score)
{
    if(scoreboardModel != NULL)
    {
        //qDebug() << "Adding " << score << " to " << playerName.toUtf8().constData();
        scoreboardModel->addScore(playerName, score);
        //qDebug() << "Setting Table View Model";
        view.SetTableViewModel(scoreboardModel);
        //qDebug() << "Saving the Score";
        storeFilePath(puzzle->filePathRef);
    }

}
//////////////////////////////////////////////////////////////////////////////////
// This function is called to enable and disable clues.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onClues(){
    if (clueTimer) clueTimer = false;
    else clueTimer = true;
    view.clue->setChecked(clueTimer);
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to pick a random clue to be shown to user.
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

        view.clearAction->setEnabled(false);
        view.undoAction->setEnabled(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when the previous move needs to be redone.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onRedoMove(){

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

        view.redoAction->setEnabled(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called when the current board and undo and redo stacks needs
// to be cleared.
//////////////////////////////////////////////////////////////////////////////////
void MainController::onClear(){



    // Clear everything
    view.clearBoard();
    displayDefaultBoard();
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to check if there is unsaved progress and user
// wants to start/load a new game.
//////////////////////////////////////////////////////////////////////////////////
void MainController::checkIfUnsavedProgressAndReset()
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
            clueTimer = false;
            //qDebug() << "Setting Puzzle Time";
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
    return;
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called to validate that all moves are legal.
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

//////////////////////////////////////////////////////////////////////////////////
// This function is called to save the file path of the puzzle.
//////////////////////////////////////////////////////////////////////////////////
void MainController::storeFilePath(QString filePath)
{
    //qDebug() << "Entering File Serialization";
    if(filePath == "")
    {
        //qDebug()<<"\tOpening File Dialog";
        QFileDialog* fileDialog = new QFileDialog();
        filePath = fileDialog->getSaveFileName(&view, "Save file", "", "*.txt");
    }

    //included as parameter for "quiet" saving so that
    //the user doesn't need to select the puzzle file in order
    //to update the scoreboard list
    else
    {
        //qDebug() << "Soft saving at: " << filePath.toUtf8().constData();
        //qDebug() << scoreboardModel->debug().toUtf8().constData();
    }

    if(filePath != "")
    {
        puzzleSerializer.serialize(puzzle, filePath);
    }
    else{
        //Popup error message...
        //TODO
    }
}
