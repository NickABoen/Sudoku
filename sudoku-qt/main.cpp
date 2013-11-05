#include <QApplication>

#include "MainController.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Controller::MainController controller;

    return a.exec();
}
