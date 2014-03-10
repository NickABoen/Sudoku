#include <iostream>
#include <fstream>

#include "PuzzleSerializer.h"
#include "../test.h"

using namespace Model;

//////////////////////////////////////////////////////////////////////////////////
// Constructor for PuzzleSerializer
//////////////////////////////////////////////////////////////////////////////////
PuzzleSerializer::PuzzleSerializer(){

    if(test) testfile << "PS1  ####################### PuzzleSerializer constructor #######################\n";

}

//////////////////////////////////////////////////////////////////////////////////
// Destructor for PuzzleSerializer
//////////////////////////////////////////////////////////////////////////////////
PuzzleSerializer::~PuzzleSerializer(){

    if(test) testfile << "PS2  ####################### PuzzleSerializer destructor #######################\n";

}

//////////////////////////////////////////////////////////////////////////////////
// This function creates a text file representing a unplayed puzzle.
//////////////////////////////////////////////////////////////////////////////////
void PuzzleSerializer::serialize(Puzzle* puzzle, QString filePath)
{

    if(test) testfile << "PS3  ####################### PuzzleSerializer serialize #######################\n";

    std::ofstream file;
    file.open(filePath.toLatin1());
    // Write default board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->defaultBoard[j][i] << ",";
            if(test) testfile << "PS4 Puzzle defaultBoard at " << i << "," << j << " saved as " << puzzle->defaultBoard[i][j] << "\n";
        }
        file << "\n";
    }
    file << "\n";
    // Write solved board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->solvedBoard[j][i] << ",";
            if(test) testfile << "PS5  Puzzle solvedBoard at " << i << "," << j << " saved as " << puzzle->solvedBoard[i][j] << "\n";
        }
        file << "\n";
    }
    puzzle->filePathRef = filePath;
    file.close();
}

//////////////////////////////////////////////////////////////////////////////////
// This function creates a puzzle from a text file that represents a previously
// unplayed puzzle.
//////////////////////////////////////////////////////////////////////////////////
Puzzle* PuzzleSerializer::deserialize(QString filePath){

    if(test) testfile << "PS6  ####################### PuzzleSerializer deserialize #######################\n";

    Puzzle *puzzle = new Puzzle();

    std::ifstream file(filePath.toLatin1());

    char comma;

    // Read in defalut board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->defaultBoard[j][i] >> comma;
            if(test) testfile << "PS7 Puzzle defaultBoard at " << i << "," << j << " set to " << puzzle->defaultBoard[i][j] << "\n";
        }
        file.ignore();
    }
    file.ignore();
    // Read in solved board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->solvedBoard[j][i] >> comma;
            if(test) testfile << "PS8  Puzzle solvedBoard at " << i << "," << j << " set to " << puzzle->solvedBoard[i][j] << "\n";
        }
        file.ignore();
    }

    file.close();

    puzzle->filePathRef = filePath;
    return puzzle;
}

