#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i = 1; i < 10; i++) {
        for (int j = 1; j < 10; j++) {

            //Creating and formatting each QLineEdit field.
            fields[i][j] = new QLineEdit(" ",this);
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
    delete ui;
}

void MainWindow::fieldChanged(QString text)
{
    //Will eventually parse the object name '1_1' (row 1, col 1) into a position array,
    //and pass the position and values to the Controller, for now just update label to display Value/Field.
    QLineEdit *field = (QLineEdit *)sender();
    ui->label->setText("Value: " +text + " Field: " + field->objectName());
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
