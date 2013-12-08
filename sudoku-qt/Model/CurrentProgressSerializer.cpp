#include "CurrentProgressSerializer.h"
#include "../test.h"

using namespace Model;

//////////////////////////////////////////////////////////////////////////////////
// Constructor for CurrentProgressSerializer
//////////////////////////////////////////////////////////////////////////////////
CurrentProgressSerializer::CurrentProgressSerializer(){

    if(test) testfile << "CPS1  ####################### CurrentProgressSerializer constructor #######################\n";
}

//////////////////////////////////////////////////////////////////////////////////
// Destructor for CurrentProgressSerializer
//////////////////////////////////////////////////////////////////////////////////
CurrentProgressSerializer::~CurrentProgressSerializer(){

    if(test) testfile << "CPS2  ####################### CurrentProgressSerializer destructor #######################\n";
}

//////////////////////////////////////////////////////////////////////////////////
// This function creates a text file representing the current progress of a
// puzzle.
//////////////////////////////////////////////////////////////////////////////////
void CurrentProgressSerializer::serialize(Puzzle* puzzle, QString filePath){

    if(test) testfile << "CPS3  ####################### CurrentProgressSerializer serialize #######################\n";

    std::ofstream file;
    file.open(filePath.toLatin1());

    if (!puzzle->filePathRef.isEmpty()) {
        if(test) testfile << "CPS4  Puzzle filepath reference is not empty\n";
        //shouldn't be empty, but will mess up either way if it is.
        file << puzzle->filePathRef.toUtf8().constData();
        file << "\n\n";
    }

    // Write current board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->currentBoard[j][i] << ",";
            if(test) testfile << "CPS5  Puzzle at " << i << "," << j << " saved as " << puzzle->currentBoard[i][j] << "\n";
        }
        file << "\n";
    }
    file << "\n";

    // Write undo stack
    file << puzzle->undo.count() << "\n";
    for(int i = 0; i < puzzle->undo.count(); i++){
        if(test) testfile << "CPS6  Undo move saved\n";
        Move mv = puzzle->undo[i];
        file << mv.x << "," << mv.y << "," << mv.value << ",";
        file << "\n";
    }
    file << "\n";

    // Write redo stack
    file << puzzle->redo.count() << "\n";
    for(int i = 0; i < puzzle->redo.count(); i++){
        if(test) testfile << "CPS7  Redo move saved\n";
        Move mv = puzzle->redo[i];
        file << mv.x << "," << mv.y << "," << mv.value << ",";
        file << "\n";
    }

    file.close();
}

//////////////////////////////////////////////////////////////////////////////////
// This function creates a puzzle from a text file that represents a previously
// played puzzle.
//////////////////////////////////////////////////////////////////////////////////
Puzzle* CurrentProgressSerializer::deserialize(QString filePath, PuzzleSerializer puzzleSerializer){

    if(test) testfile << "CPS8  ####################### CurrentProgressSerializer deserialize #######################\n";

    Puzzle* puzzle;

    std::string refFilePath;
    std::ifstream getRefFile(filePath.toLatin1());
    getRefFile >> refFilePath;
    getRefFile.close();
    QString qPath = QString::fromStdString(refFilePath);

    char comma;

    puzzle = puzzleSerializer.deserialize(qPath);

    //Reopen original filepath, set current

    std::ifstream file(filePath.toLatin1());
    file >> refFilePath;
    file.ignore();

    // Read current board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->currentBoard[j][i] >> comma;
            if(test) testfile << "CPS9  Puzzle at " << i << "," << j << " set to " << puzzle->currentBoard[i][j] << "\n";
        }
        file.ignore();
    }

    file.ignore();

    // Read undo stack
    int count, x, y, val;
    file >> count;
    for(int i = 0; i < count; i++){
        if(test) testfile << "CPS10 Undo move set\n";
        file >> x >> comma >> y >> comma >> val >> comma;
        Move* mv = new Move(x,y,val);
        puzzle->undo.push(*mv);
    }

    file.ignore();

    // Read redo stack
    file >> count;
    for(int i = 0; i < count; i++){
        if(test) testfile << "CPS11 Redo move set\n";
        file >> x >> comma >> y >> comma >> val >> comma;
        Move* mv = new Move(x,y,val);
        puzzle->redo.push(*mv);
    }

    file.close();
    puzzle->filePathRef = qPath;
    return puzzle;
}
