#include <iostream>
#include <fstream>

#include "PuzzleSerializer.h"

using namespace Model;

PuzzleSerializer::PuzzleSerializer(){
    //TODO
}

PuzzleSerializer::~PuzzleSerializer(){
    //TODO
}

void PuzzleSerializer::serialize(Puzzle* puzzle, QString filePath)
{
    std::ofstream file;
    file.open(filePath.toLatin1());
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->defaultBoard[i][j] << ",";
        }
        file << "\n";
    }
    file << "\n";
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->solvedBoard[i][j] << ",";
        }
        file << "\n";
    }
    file.close();
}

Puzzle* PuzzleSerializer::deserialize(QString filePath){

    Puzzle *puzzle = new Puzzle();

    std::ifstream file(filePath.toLatin1());

    char comma;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->defaultBoard[i][j] >> comma;
        }
        file.ignore();
    }
    file.ignore();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->solvedBoard[i][j] >> comma;
        }
        file.ignore();
    }

    file.close();

    puzzle->currentBoard = puzzle->defaultBoard;
    return puzzle;
}

