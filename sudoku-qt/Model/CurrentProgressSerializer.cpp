#include "CurrentProgressSerializer.h"
#include "../test.h"

using namespace Model;

//////////////////////////////////////////////////////////////////////////////////
// Constructor for CurrentProgressSerializer
//////////////////////////////////////////////////////////////////////////////////
CurrentProgressSerializer::CurrentProgressSerializer(){

}

//////////////////////////////////////////////////////////////////////////////////
// Destructor for CurrentProgressSerializer
//////////////////////////////////////////////////////////////////////////////////
CurrentProgressSerializer::~CurrentProgressSerializer(){

}

//////////////////////////////////////////////////////////////////////////////////
// This function creates a text file representing the current progress of a
// puzzle.
//////////////////////////////////////////////////////////////////////////////////
void CurrentProgressSerializer::serialize(Puzzle* puzzle, QString filePath){


    std::ofstream file;
    file.open(filePath.toLatin1());
    if (!puzzle->filePathRef.isEmpty()) {
        //shouldn't be empty, but will mess up either way if it is.
        file << puzzle->filePathRef.toUtf8().constData();
        file << "\n\n";
    }
    file << puzzle->currentTime<<"\n\n";

    // Write current board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->currentBoard[j][i] << ",";
        }
        file << "\n";
    }
    file << "\n";

    // Write undo stack
    file << puzzle->undo.count() << "\n";
    for(int i = 0; i < puzzle->undo.count(); i++){
        Move mv = puzzle->undo[i];
        file << mv.x << "," << mv.y << "," << mv.value << ",";
        file << "\n";
    }
    file << "\n";

    // Write redo stack
    file << puzzle->redo.count() << "\n";
    for(int i = 0; i < puzzle->redo.count(); i++){
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
    /*std::string time;
    std::getline(file, time);
    puzzle->currentTime = std::atoi(time.c_str());*/
    file >> puzzle->currentTime;
    file.ignore();
    // Read current board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->currentBoard[j][i] >> comma;
        }
        file.ignore();
    }

    file.ignore();

    // Read undo stack
    int count, x, y, val;
    file >> count;
    for(int i = 0; i < count; i++){
        file >> x >> comma >> y >> comma >> val >> comma;
        Move* mv = new Move(x,y,val);
        puzzle->undo.push(*mv);
    }

    file.ignore();

    // Read redo stack
    file >> count;
    for(int i = 0; i < count; i++){
        file >> x >> comma >> y >> comma >> val >> comma;
        Move* mv = new Move(x,y,val);
        puzzle->redo.push(*mv);
    }

    file.close();
    puzzle->filePathRef = qPath;
    return puzzle;
}
