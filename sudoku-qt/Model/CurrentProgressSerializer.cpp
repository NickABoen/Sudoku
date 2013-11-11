#include "CurrentProgressSerializer.h"

using namespace Model;

CurrentProgressSerializer::CurrentProgressSerializer(){
    //TODO
}

CurrentProgressSerializer::~CurrentProgressSerializer(){
    //TODO
}

void CurrentProgressSerializer::serialize(QStack<Model::Move> undo, Puzzle* puzzle, QString filePath){
    //TODO
    Move* mv = new Move(1,8,4);
    undo.push(*mv);
    mv = new Move(1,3,8);
    undo.push(*mv);
    mv = new Move(1,7,7);
    undo.push(*mv);

    qDebug("Serializing current progress...");
    std::ofstream file;
    file.open(filePath.toLatin1());
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->currentBoard[i][j] << ",";
        }
        file << "\n";
    }
    file << "\n";
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            //this needs to be not here....
            //AND SOMEWHERE NEED TO SAVE THE REFERENCE (filepath) TO THE SOLVEDSTATE ORIGINAL PUZZLE
            //preferably above
            file << puzzle->solvedBoard[i][j] << ",";
        }
        file << "\n";
    }
    file << "\n";

    while(!undo.isEmpty()) {
        Move mv = undo.pop();
        file << mv.x << "," << mv.y << "," << mv.value << ",";
        file << "\n";
    }

    qDebug("Finished serializing progress...");
    file.close();
}

void CurrentProgressSerializer::deserialize(QStack<Model::Move> undo, Puzzle* puzzle, QString filePath){
    //TODO
    qDebug("Deserializing current progress...");

    std::ifstream file(filePath.toLatin1());

    char comma;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->currentBoard[i][j] >> comma;
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

    file.ignore();

    int x,y,val;
    while(file >> x >> comma >> y >> comma >> val >> comma) {
        Move* mv = new Move(x,y,val);
        undo.push_back(*mv);
    }

    file.close();
}
