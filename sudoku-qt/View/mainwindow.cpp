#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QKeyEvent>


using namespace View;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int pad = 20;
    int seperatorWidth = 2;
    int squareSize = 60;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {

            //Creating and formatting each QLineEdit field.
            fields[i][j] = new QLineEdit(ui->centralWidget);
            QLineEdit *field = fields[i][j];
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

        }
    }

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

    createMenu();
}

MainWindow::~MainWindow()
{
    //buttons are deleted when ui is deleted since it is parent, see Qt documentation
    delete ui;
}

void MainWindow::fieldChanged(QString text)
{
    QLineEdit *field = (QLineEdit *)sender();
    int *moveArray = createMoveArray(text, field->objectName());
    emit onMakeMove(moveArray);
}

void MainWindow::fieldFocused() {
    if (isNotesEnabled()) {
        QLineEdit *field = (QLineEdit *)sender();
        int *moveArray = createMoveArray("1", field->objectName());
        QLabel *label = labels[moveArray[1]][moveArray[2]];

        QString result = QInputDialog::getText(0, "Add Note", "Value:", QLineEdit::Normal, label->text());

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

void MainWindow::setNotesEnabled(bool enabled) {
    notesEnabled = enabled;
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

        QAction *generateBoardAction = fileMenu->addAction("Generate Board");
        connect(generateBoardAction, SIGNAL(triggered()), this, SIGNAL(onGenerateBoardPressed()));

        QAction *generateBoardFromImageAction = fileMenu->addAction("Generate Board From Image");
        connect(generateBoardFromImageAction, SIGNAL(triggered()), this, SIGNAL(onGenerateBoardFromImagePressed()));

        QAction *exitAction = fileMenu->addAction("Exit");
        connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    }

    QMenu *editMenu = ui->menuBar->addMenu("Edit");
    {
        undoAction = editMenu->addAction("Undo");
        undoAction->setEnabled(false);
        connect(undoAction, SIGNAL(triggered()), this, SIGNAL(onUndoPressed()));

        redoAction = editMenu->addAction("Redo");
        redoAction->setEnabled(false);
        connect(redoAction, SIGNAL(triggered()), this, SIGNAL(onRedoPressed()));

        clearAction = editMenu->addAction("Clear Board");
        clearAction->setEnabled(false);
        connect(clearAction, SIGNAL(triggered()), this, SIGNAL(onClearPressed()));
    }

    QMenu *settingsMenu = ui->menuBar->addMenu("Settings");
    {
        QAction *settingsAction = settingsMenu->addAction("Hints");
        settingsAction->setEnabled(true);
        connect(settingsAction, SIGNAL(triggered()), this, SIGNAL(onHintPressed()));
        //TODO
        enableNotes = settingsMenu->addAction("Enable Notes");
        enableNotes->setEnabled(true);
        connect(enableNotes, SIGNAL(triggered()), this, SIGNAL(onEnableNotesPressed()));

        disableNotes = settingsMenu->addAction("Disable Notes");
        disableNotes->setEnabled(false);
        connect(disableNotes, SIGNAL(triggered()), this, SIGNAL(onEnableNotesPressed()));

        clue = settingsMenu->addAction("Clues");
        clue->setEnabled(true);
        connect(clue, SIGNAL(triggered()), this, SIGNAL(onCluePressed()));
    }
}

