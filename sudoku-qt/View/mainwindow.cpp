#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

using namespace View;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createMenu();

    for (int i = 1; i < 10; i++) {
        for (int j = 1; j < 10; j++) {

            //Creating and formatting each QLineEdit field.
            fields[i][j] = new QLineEdit();
            QValidator *val = new QIntValidator(1,9,fields[i][j]);
            fields[i][j]->setValidator(val);
            fields[i][j]->setObjectName(QString::number(i) + "_" + QString::number(j));
            fields[i][j]->setFixedHeight(60);
            fields[i][j]->setFixedWidth(75);
            fields[i][j]->setAlignment(Qt::AlignCenter);
            fields[i][j]->setStyleSheet("font: 28pt;");

            connect(fields[i][j], SIGNAL(textChanged(QString)),this,SLOT(fieldChanged(QString)));
        }
    }
    createLayout();
}

MainWindow::~MainWindow()
{
    //buttons are deleted when ui is deleted since it is parent, see Qt documentation
    delete ui;
}

void MainWindow::fieldChanged(QString text)
{
    //Will eventually parse the object name '1_1' (row 1, col 1) into a position array,
    //and pass the position and values to the Controller, for now just update label to display Value/Field.
    QLineEdit *field = (QLineEdit *)sender();
    int *moveArray;
    moveArray = createMoveArray(text, field->objectName());
    emit onMakeMove(moveArray);
}

void MainWindow::setDefaultMove(int* moveArray){
    qDebug("Updating board...");
    int x = moveArray[0];
    int y = moveArray[1];
    int value = moveArray[2];
    fields[x][y]->setEnabled(false);
    fields[x][y]->setText(QString::number(value));
}

int * MainWindow::createMoveArray(QString text, QString fieldname) {
    static int loc[3];
    QStringList pieces = fieldname.split("_");
    loc[0] = text.toInt();
    loc[1] = pieces.value(0).toInt();
    loc[2] = pieces.value(1).toInt();
    return loc;
}

void MainWindow::createLayout()
{
    ui->gridLayout->setMargin(6);
    ui->gridLayout->setSpacing(6);

    for(int i = 1; i < 10; i++) {
        for(int j = 1; j < 10; j++) {
            ui->gridLayout->addWidget(fields[i][j], i, j);

        }
    }
}

void MainWindow::createMenu()
{
    QMenu *fileMenu = ui->menuBar->addMenu("File");
    {
        QAction *loadPuzzleAction = fileMenu->addAction("Load Puzzle");
        connect(loadPuzzleAction, SIGNAL(triggered()), this, SIGNAL(onLoadPuzzlePressed()));

        QAction *savePuzzleAction = fileMenu->addAction("Save Puzzle");
        connect(savePuzzleAction, SIGNAL(triggered()), this, SIGNAL(onSavePuzzlePressed()));

        QAction *loadProgressAction = fileMenu->addAction("Load Progress");
        connect(loadProgressAction, SIGNAL(triggered()), this, SIGNAL(onLoadProgressPressed()));

        QAction *saveProgressAction = fileMenu->addAction("Save Progress");
        connect(saveProgressAction, SIGNAL(triggered()), this, SIGNAL(onSaveProgressPressed()));

        QAction *exitAction = fileMenu->addAction("Exit");
        connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    }

    QMenu *editMenu = ui->menuBar->addMenu("Edit");
    {
        QAction *undoAction = editMenu->addAction("Undo");
        connect(undoAction, SIGNAL(triggered()), this, SIGNAL(onUndoPressed()));

        QAction *redoAction = editMenu->addAction("Redo");
        connect(redoAction, SIGNAL(triggered()), this, SIGNAL(onRedoPressed()));
    }

    QMenu *settingsMenu = ui->menuBar->addMenu("Settings");
    {
        QAction *settingsAction = settingsMenu->addAction("Settings");
        //TODO
    }
}
