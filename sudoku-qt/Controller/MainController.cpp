#include "MainController.h"
#include "difficultyselector.h"
#include "BoardGenerator.h"
#include "BoardSolver.h"
#include "Move.h"
#include "Puzzle.h"
#include "../test.h"
//Tesseract includes
#include "baseapi.h"
//OpenCV includes
#include "core.hpp"
#include "highgui.hpp"
#include "imgproc/imgproc.hpp"

//TODO Temp Includes Remove
#include <iostream>
#include <fstream>


#include <string>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractButton>
#include <QAction>
#include <QPushButton>
#include <QLibrary>

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
    connect(&view, SIGNAL(onGenerateBoardFromImagePressed()), this, SLOT(onGenerateBoardFromImage()));

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
    if(filePath == "") return;

    if(!checkIfUnsavedProgressAndReset()) return;

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
    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first();
    if(filePath == "") return;

    if(!checkIfUnsavedProgressAndReset()) return;

    puzzle = puzzleSerializer.deserialize(filePath);

    enableUndoRedo = false;
    displayDefaultBoard();
    enableUndoRedo = true;

    view.centralWidget()->setEnabled(true);

    view.undoAction->setEnabled(false);
    view.redoAction->setEnabled(false);
    view.clearAction->setEnabled(false);
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
void MainController::onGenerateBoard()
{

    DifficultySelector *DS = new DifficultySelector();
    BoardGenerator *BG = new BoardGenerator();

    if(!checkIfUnsavedProgressAndReset()) return;

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

void MainController::onGenerateBoardFromImage()
{
    QString filePath;
    QFileDialog* fileDialog = new QFileDialog(&view, "Board Image", "", "*.*");

    if(fileDialog->exec()) filePath = fileDialog->selectedFiles().first();
    if(filePath == "") return;

    if(!checkIfUnsavedProgressAndReset()) return;

    puzzle = new Puzzle();

    std::ofstream file;
    file.open("ocr-output.txt");

    //file << filePath.toStdString().c_str() << "\n";

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Users\\Logan\\Desktop\\tesseract-ocr", "eng") == -1)
    {
        //TODO error handling

    }

    // Set to only recognize numbers
    api->SetVariable("tessedit_char_whitelist", "0123456789");

    // Open input image with OpenCV
    cv::Mat src = cv::imread(filePath.toStdString());
    if(src.empty())
    {
        //TODO error handling
        return;
    }

//    cv::vector<cv::Mat> channels;
//    cv::split(src, channels);
//    cv::Scalar m = cv::mean(channels[0]);

//    src = channels[0];

    //Set src image grey and threshold to binary image
    cv::cvtColor(src, src, CV_BGR2GRAY);
    cv::imwrite("grey.png", src);

//    cv::adaptiveThreshold(src, src, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C , cv::THRESH_BINARY, 25, 50);
//    cv::imwrite("afterThreshold.png", src);

    // Remove lines that are close to height/width of image,
    // basically remove grid so that only numbers are left for
    // tesseract to detect.

    cv::Mat cranny;

    cv::Canny(src, cranny, 50, 200, 3);
    //cv::imwrite("Canny.png", cranny);
    cv::vector<cv::Vec4i> lines;
    cv::HoughLinesP(cranny, lines, 1, CV_PI/180, 50, 50, 10 );

    for( size_t i = 0; i < lines.size(); i++ )
    {
        cv::Vec4i l = lines[i];
        cv::line( src, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,255,255), 3, CV_AA);
    }

   // cv::imwrite("AfterLinesRemoved.png", src);

    api->SetImage((uchar*)src.data, src.size().width,
                  src.size().height, src.channels(), src.step1());



    char *boxtext = api->GetBoxText(0);

    // Destroy used object and release memory
    api->End();

    int cellWidth = src.size().width/9;
    int cellHeight = src.size().height/9;

    std::istringstream boxTextStream(boxtext);

    puzzle = new Puzzle();

    //file << "Parsed box text...\n";
    while (!boxTextStream.eof())
    {
       int left, right, top, bottom, value, trash;

       boxTextStream >> value >> left >> bottom >> right >> top >> trash;

       //TODO for some reason it reads in the last line twice...

       float x1 = left / cellWidth;
       float x2 = right / cellWidth;
       float y1 = bottom / cellHeight;
       float y2 = top / cellHeight;

       if((1 - (x1 - (int)x1)) < (x2 - (int)x2)) x1 = x2;
       if((1 - (y1 - (int)y1)) < (y2 - (int)y2)) y1 = y2;
       puzzle->defaultBoard[(int)x1][8 - (int)y1] = value;
    }


    //TODO fix this
    int** ConvertedBoard = new int*[9];
    for(int i = 0; i < 9; i++){
        ConvertedBoard[i] = new int[9];
    }
    for(int i = 0; i<9;i++){
        for(int j = 0; j<9; j++){
            int row = 3*(i/3)+j/3;
            int column = 3*(i%3)+j%3;
            ConvertedBoard[i][j] = puzzle->defaultBoard[row][column];
        }
    }

    BoardGenerator*  boardGenerator = new BoardGenerator();

    BoardSolver boardSolver(ConvertedBoard);
    puzzle->solvedBoard = boardGenerator->ConvertBoard(boardSolver.Solve());

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->solvedBoard[j][i] << ",";
        }
        file << "\n";
    }

    enableUndoRedo = false;
    displayDefaultBoard();
    enableUndoRedo = true;

    view.centralWidget()->setEnabled(true);

    view.undoAction->setEnabled(false);
    view.redoAction->setEnabled(false);
    view.clearAction->setEnabled(false);



//    file << "OCR output:\n";
//    if(outText != NULL){
//        file << outText;
//    }

    file.close();
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
