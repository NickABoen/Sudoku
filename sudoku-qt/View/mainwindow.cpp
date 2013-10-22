#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

using namespace View;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    undoButton = new QPushButton(this);
    undoButton->setText("Undo");
    undoButton->move(50, 600);
    connect(undoButton, SIGNAL(clicked()), this, SIGNAL(onUndo()));

    redoButton = new QPushButton(this);
    redoButton->setText("Redo");
    redoButton->move(150, 600);
    connect(redoButton, SIGNAL(clicked()), this, SIGNAL(onRedo()));

    loadPuzzleButton = new QPushButton(this);
    loadPuzzleButton->setText("Load Puzzle");
    loadPuzzleButton->move(250, 600);
    connect(loadPuzzleButton, SIGNAL(clicked()), this, SIGNAL(onLoadPuzzlePressed()));

    savePuzzleButton = new QPushButton(this);
    savePuzzleButton->setText("Save Puzzle");
    savePuzzleButton->move(350, 600);
    connect(savePuzzleButton, SIGNAL(clicked()), this, SIGNAL(onSavePuzzlePressed()));

    loadProgressButton = new QPushButton(this);
    loadProgressButton->setText("Load Progress");
    loadProgressButton->move(450, 600);
    connect(loadProgressButton, SIGNAL(clicked()), this, SIGNAL(onLoadProgressPressed()));

    saveProgressButton = new QPushButton(this);
    saveProgressButton->setText("Save Progress");
    saveProgressButton->move(550, 600);
    connect(saveProgressButton, SIGNAL(clicked()), this, SIGNAL(onSaveProgressPressed()));

    for (int i = 1; i < 10; i++) {
        for (int j = 1; j < 10; j++) {

            //Creating and formatting each QLineEdit field.
            fields[i][j] = new QLineEdit(" ",this);
            fields[i][j]->setInputMask("D");
            fields[i][j]->setObjectName(QString::number(i) + "_" + QString::number(j));
            fields[i][j]->setFixedHeight(31);
            fields[i][j]->setFixedWidth(41);
            fields[i][j]->setAlignment(Qt::AlignCenter);
            fields[i][j]->setStyleSheet("font: 18pt;");

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
    emit onMakeMove(QString("Value: " + text + " Field: " + field->objectName()));
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
