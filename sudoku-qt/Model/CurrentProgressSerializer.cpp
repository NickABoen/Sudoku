#include "CurrentProgressSerializer.h"

using namespace Model;

CurrentProgressSerializer::CurrentProgressSerializer(){
    //TODO
}

CurrentProgressSerializer::~CurrentProgressSerializer(){
    //TODO
}

void CurrentProgressSerializer::serialize(Puzzle* puzzle, QString filePath){
    //TODO
    qDebug("Serializing current progress...");
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
    qDebug("Finished serializing progress...");
    file.close();
}

void CurrentProgressSerializer::deserialize(Puzzle* puzzle, QString filePath){
    //TODO
    qDebug("Deserializing current progress...");

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

}
