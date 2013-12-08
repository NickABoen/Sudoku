#include <QApplication>

#include "MainController.h"
#include "test.h"

//////////////////////////////////////////////////////////////////////////////////
// Main for sudoku project
//////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    if(test) testfile << "Ma1 ####################### Main #######################\n";

    // Create QApplication and execute MainController
    QApplication a(argc, argv);
    Controller::MainController controller;

    int retValue = a.exec();

    if(test) testfile.close();

    return retValue;
}
