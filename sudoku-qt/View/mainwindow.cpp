#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "focusLineEdit.h"

#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include <QShortcut>

using namespace View;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(584, 660);
    int pad = 20;
    int seperatorWidth = 2;
    int squareSize = 60;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {

            //Creating and formatting each QLineEdit field.
            fields[i][j] = new FocusLineEdit(ui->centralWidget);
            FocusLineEdit *field = fields[i][j];
            QValidator *val = new QIntValidator(1,9,field);
            field->setValidator(val);
            field->setObjectName(QString::number(i) + "_" + QString::number(j));
            field->setGeometry(i*squareSize + i/3*seperatorWidth + pad,
                               j*squareSize + j/3*seperatorWidth + pad,
                               squareSize,
                               squareSize);
            field->setAlignment(Qt::AlignCenter);
            field->setStyleSheet("font: 28pt;");

            labels[i][j] = new QLabel(ui->centralWidget);
            QLabel *label = labels[i][j];
            label->setEnabled(true);
            label->setGeometry(i*squareSize + i/3*seperatorWidth + pad,
                               j*squareSize + j/3*seperatorWidth + pad,
                               squareSize,
                               squareSize/4);
            label->setText("");


            connect(field, SIGNAL(selectionChanged()), this, SLOT(fieldFocused()));
            connect(field, SIGNAL(textChanged(QString)),this,SLOT(fieldChanged(QString)));
            connect(field, SIGNAL(focused(bool)),this,SLOT(focusChanged(bool)));
            connect(field, SIGNAL(moveLeft()), this, SLOT(moveLeft()));
            connect(field, SIGNAL(moveRight()), this, SLOT(moveRight()));
            connect(field, SIGNAL(shortcutAddNote()), this, SLOT(shortcutAddNote()));
        }
    }
    focusedField[0] = 0;
    focusedField[1] = 0;
    notesEnabled = false;
    validationEnabled = false;


    QFrame *line0 = new QFrame(ui->centralWidget);
    line0->setFrameShape(QFrame::HLine);
    line0->setGeometry(0 + pad, 3*squareSize + pad, squareSize*9+seperatorWidth*2, seperatorWidth);
    line0->setLineWidth(seperatorWidth);

    QFrame *line1 = new QFrame(ui->centralWidget);
    line1->setFrameShape(QFrame::HLine);
    line1->setGeometry(0 + pad, 6*squareSize+4 + pad, squareSize*9+seperatorWidth*2, seperatorWidth);
    line1->setLineWidth(seperatorWidth);

    QFrame *line2 = new QFrame(ui->centralWidget);
    line2->setFrameShape(QFrame::VLine);
    line2->setGeometry(3*squareSize + pad, 0 + pad, seperatorWidth, squareSize*9+seperatorWidth*2);
    line2->setLineWidth(seperatorWidth);

    QFrame *line3 = new QFrame(ui->centralWidget);
    line3->setFrameShape(QFrame::VLine);
    line3->setGeometry(6*squareSize+seperatorWidth + pad, 0 + pad, seperatorWidth, squareSize*9+seperatorWidth*2);
    line3->setLineWidth(seperatorWidth);
    TIMER = new QLabel(ui->centralWidget);
    TIMER->setText("00:00:00");
    TIMER->setStyleSheet("font: bold 50px");
    TIMER->setGeometry(175, 560, 240,50);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_U), this, SIGNAL(onUndoPressed()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this, SIGNAL(onRedoPressed()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_E), this, SIGNAL(onEnableNotesPressed()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this, SIGNAL(onCluePressed()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), this, SIGNAL(onHintPressed()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_G), this, SIGNAL(onGenerateBoardPressed()));
    new QShortcut(QKeySequence(Qt::Key_Up), this, SLOT(moveUp()));
    new QShortcut(QKeySequence(Qt::Key_Down), this, SLOT(moveDown()));

    createMenu();
}

MainWindow::~MainWindow()
{
    //buttons are deleted when ui is deleted since it is parent, see Qt documentation
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    emit(closeThread());
    QMainWindow::closeEvent(event);
}

void MainWindow::fieldChanged(QString text)
{
    FocusLineEdit *field = (FocusLineEdit *)sender();
    int *moveArray = createMoveArray(text, field->objectName());
    emit onMakeMove(moveArray);
}

void MainWindow::focusChanged(bool b)
{
    if (b) {
        FocusLineEdit *field = (FocusLineEdit *)sender();
        int *moveArray = createMoveArray("1", field->objectName());
        focusedField[0] = moveArray[1];
        focusedField[1] = moveArray[2];
    }
}

void MainWindow::moveUp() {
    bool enabled = false;
    int y = focusedField[1];
    while (!enabled && y > 0) {
        y -= 1;
        if (fields[focusedField[0]][y]->isEnabled()) {
            enabled = true;
            focusedField[1] = y;
        }
    }
    fields[focusedField[0]][focusedField[1]]->setFocus();
}
void MainWindow::moveDown() {
    bool enabled = false;
    int y = focusedField[1];
    while (!enabled && y < 8) {
        y += 1;
        if (fields[focusedField[0]][y]->isEnabled()) {
            enabled = true;
            focusedField[1] = y;
        }
    }
    fields[focusedField[0]][focusedField[1]]->setFocus();
}
void MainWindow::moveRight() {
    bool enabled = false;
    int x = focusedField[0];
    while (!enabled && x < 8) {
        x += 1;
        if (fields[x][focusedField[1]]->isEnabled()) {
            enabled = true;
            focusedField[0] = x;
        }
    }
    fields[focusedField[0]][focusedField[1]]->setFocus();
}
void MainWindow::moveLeft() {
    bool enabled = false;
    int x = focusedField[0];;
    while (!enabled && x > 0) {
        x -= 1;
        if (fields[x][focusedField[1]]->isEnabled()) {
            enabled = true;
            focusedField[0] = x;
        }
    }
    fields[focusedField[0]][focusedField[1]]->setFocus();
}

//For notes (should be double clicked.
void MainWindow::fieldFocused() {
    if (isNotesEnabled()) {
        FocusLineEdit *field = (FocusLineEdit *)sender();
        int *moveArray = createMoveArray("1", field->objectName());
        QLabel *label = labels[moveArray[1]][moveArray[2]];

        QString result = QInputDialog::getText(0, "Add Note", "Value:", FocusLineEdit::Normal, label->text());

        label->setText(result);
    }
}

void MainWindow::shortcutAddNote() {
    if (isNotesEnabled()) {
        QLabel *label = labels[focusedField[0]][focusedField[1]];

        QString result = QInputDialog::getText(0, "Add Note", "Value:", FocusLineEdit::Normal, label->text());

        label->setText(result);
    }
}

void MainWindow::setMove(int* moveArray, bool isCurrent){
    int value = moveArray[0];
    int x = moveArray[1];
    int y = moveArray[2];
    fields[x][y]->setEnabled(isCurrent);
    fields[x][y]->setText(QString::number(value));
}

void MainWindow::makeMove(int* moveArray){
    int value = moveArray[0];
    int x = moveArray[1];
    int y = moveArray[2];
    if(value > 0){
        fields[x][y]->setText(QString::number(value));
    }
    else{
        fields[x][y]->setText("");
    }
}
void MainWindow::clearMove(int x, int y){

    fields[x][y]->setEnabled(true);
    fields[x][y]->setText("");
}

void MainWindow::clearBoard(){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            clearMove(i, j);
        }
    }
}

void MainWindow::clearLabels() {
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            labels[i][j]->setText("");
        }
    }
}

int * MainWindow::createMoveArray(QString text, QString fieldname) {
    static int loc[3];
    QStringList pieces = fieldname.split("_");

    if(text != ""){
        loc[0] = text.toInt();
    }
    else{
        loc[0] = 0;
    }
    loc[1] = pieces.value(0).toInt();
    loc[2] = pieces.value(1).toInt();
    return loc;
}

bool MainWindow::isFieldEnabled(int i, int j) {
    return fields[i][j]->isEnabled();
}

bool MainWindow::isNotesEnabled() {
    return notesEnabled;
}
bool MainWindow::isValidationEnabled() {
    return validationEnabled;
}

void MainWindow::setNotesEnabled(bool enabled) {
    notesEnabled = enabled;
}
void MainWindow::setValidationEnabled(bool enabled) {
    validationEnabled = enabled;
}

void MainWindow::changeColor(int x, int y)
{
    fields[x][y]->setStyleSheet("QLineEdit{color: white; background: red;font: 28pt;}");
}
void MainWindow::resetColor(int x, int y)
{
    //fields[x][y]->setStyleSheet("QLineEdit{color: black; background: white;font: 28pt;}");
    fields[x][y]->setStyleSheet("");
    fields[x][y]->setStyleSheet("QLineEdit{font: 28pt;}");
}

void MainWindow::createMenu()
{
    QMenu *fileMenu = ui->menuBar->addMenu("File");
    {
        QAction *loadPuzzleAction = fileMenu->addAction("Load Puzzle");
        connect(loadPuzzleAction, SIGNAL(triggered()), this, SIGNAL(onLoadPuzzlePressed()));

        //Not needed for this phase, not even sure if we will ever want a menu option for this
        //Will probably just want to use functionality for generating puzzles...
        //QAction *savePuzzleAction = fileMenu->addAction("Save Puzzle");
        //connect(savePuzzleAction, SIGNAL(triggered()), this, SIGNAL(onSavePuzzlePressed()));

        QAction *loadProgressAction = fileMenu->addAction("Load Progress");
        connect(loadProgressAction, SIGNAL(triggered()), this, SIGNAL(onLoadProgressPressed()));

        QAction *saveProgressAction = fileMenu->addAction("Save Progress");
        connect(saveProgressAction, SIGNAL(triggered()), this, SIGNAL(onSaveProgressPressed()));

        QAction *generateBoardAction = fileMenu->addAction("Generate Board\tCtrl+G");
        connect(generateBoardAction, SIGNAL(triggered()), this, SIGNAL(onGenerateBoardPressed()));

        QAction *exitAction = fileMenu->addAction("Exit");
        connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    }

    QMenu *editMenu = ui->menuBar->addMenu("Edit");
    {
        undoAction = editMenu->addAction("Undo\tCtrl+U");
        undoAction->setEnabled(false);
        connect(undoAction, SIGNAL(triggered()), this, SIGNAL(onUndoPressed()));

        redoAction = editMenu->addAction("Redo\tCtrl+R");
        redoAction->setEnabled(false);
        connect(redoAction, SIGNAL(triggered()), this, SIGNAL(onRedoPressed()));

        clearAction = editMenu->addAction("Clear Board");
        clearAction->setEnabled(false);
        connect(clearAction, SIGNAL(triggered()), this, SIGNAL(onClearPressed()));
    }

    QMenu *settingsMenu = ui->menuBar->addMenu("Options");
    {
        QAction *settingsAction = settingsMenu->addAction("Hint\t\tCtrl+T");
        settingsAction->setEnabled(true);
        connect(settingsAction, SIGNAL(triggered()), this, SIGNAL(onHintPressed()));
        //TODO
        enableNotes = settingsMenu->addAction("Enable Notes\tCtrl+E");
        enableNotes->setEnabled(true);
        enableNotes->setCheckable(true);
        connect(enableNotes, SIGNAL(triggered()), this, SIGNAL(onEnableNotesPressed()));

        clue = settingsMenu->addAction("Enable Clues\tCtrl+L");
        clue->setEnabled(true);
        clue->setCheckable(true);
        connect(clue, SIGNAL(triggered()), this, SIGNAL(onCluePressed()));

        enableValidation = settingsMenu->addAction("Enable Validation\tCtrl+V");
        enableValidation->setEnabled(true);
        enableValidation->setCheckable(true);
        connect(enableValidation, SIGNAL(triggered()), this, SIGNAL(onEnableValidationPressed()));
    }

}

