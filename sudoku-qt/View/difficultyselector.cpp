#include "difficultyselector.h"
#include "ui_difficultyselector.h"
#include <QMessageBox>

DifficultySelector::DifficultySelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DifficultySelector)
{
    ui->setupUi(this);
    connect(ui->EasyButton,SIGNAL(clicked()),this, SLOT(EasyClicked()));
    connect(ui->MediumButton,SIGNAL(clicked()),this, SLOT(MediumClicked()));
    connect(ui->HardButton,SIGNAL(clicked()),this, SLOT(HardClicked()));
}

DifficultySelector::~DifficultySelector()
{
    delete ui;
}

