#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>


using namespace View;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {

            //Creating and formatting each QLineEdit field.
            fields[i][j] = new QLineEdit(ui->centralWidget);
            QLineEdit *field = fields[i][j];
            QValidator *val = new QIntValidator(1,9,field);
            field->setValidator(val);
            field->setObjectName(QString::number(i) + "_" + QString::number(j));
            field->setGeometry(i*60 + i/3*4, j*60 + j/3*4, 60, 60);
            field->setAlignment(Qt::AlignCenter);
            field->setStyleSheet("font: 28pt;");

            connect(field, SIGNAL(textChanged(QString)),this,SLOT(fieldChanged(QString)));

        }
    }

    QFrame *line0 = new QFrame(ui->centralWidget);
    line0->setFrameShape(QFrame::HLine);
    line0->setGeometry(0, 3*60, 60*9+8, 4);
    line0->setLineWidth(4);

    QFrame *line1 = new QFrame(ui->centralWidget);
    line1->setFrameShape(QFrame::HLine);
    line1->setGeometry(0, 6*60+4, 60*9+8, 4);
    line1->setLineWidth(4);

    QFrame *line2 = new QFrame(ui->centralWidget);
    line2->setFrameShape(QFrame::VLine);
    line2->setGeometry(3*60, 0, 4, 60*9+8);
    line2->setLineWidth(4);

    QFrame *line3 = new QFrame(ui->centralWidget);
    line3->setFrameShape(QFrame::VLine);
    line3->setGeometry(6*60+4, 0, 4, 60*9+8);
    line3->setLineWidth(4);

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

void MainWindow::setDefaultMove(int* moveArray){
    makeMove(moveArray);
    fields[moveArray[1]][moveArray[2]]->setEnabled(false);
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

void MainWindow::clearBoard(){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            fields[i][j]->setEnabled(true);
            fields[i][j]->setText("");
        }
    }
}

int * MainWindow::createMoveArray(QString text, QString fieldname) {
    static int loc[3];
    QStringList pieces = fieldname.split("_");

    qDebug(text.toLatin1());
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

        QAction *generateBoardAction = fileMenu->addAction("Generate Board");
        connect(generateBoardAction, SIGNAL(triggered()), this, SIGNAL(onGenerateBoardPressed()));

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
    }

    QMenu *settingsMenu = ui->menuBar->addMenu("Settings");
    {
        QAction *settingsAction = settingsMenu->addAction("Settings");
        //TODO
    }
}
