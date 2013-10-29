#include "PuzzleSerializer.h"

using namespace Model;

PuzzleSerializer::PuzzleSerializer(){
    //TODO
}

PuzzleSerializer::~PuzzleSerializer(){
    //TODO
}

void PuzzleSerializer::serialize(Puzzle* puzzle, QString filePath){
    //TODO
    qDebug("Serializing puzzle...");
    ofstream file;
    file.open(filePath);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << QString::number(puzzle->defaultBoard[i][j]) + ", ";
        }
        file << "\n";
    }
    file << "\n";
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << QString::number(puzzle->solvedBoard[i][j]) + ", ";
        }
        file << "\n";
    }
    file.close();
}

Puzzle* PuzzleSerializer::deserialize(QString filePath){
    //TODO
    qDebug("Deserializing puzzle...");
    Puzzle puzzle = new Puzzle();

    return NULL;
}

