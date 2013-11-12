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

    if (!puzzle->filePathRef.isEmpty()) {
        //shouldn't be empty, but will fuck up either way if it is.
        file << puzzle->filePathRef.toUtf8().constData();
        file << "\n";
    }

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->currentBoard[i][j] << ",";
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

Puzzle* CurrentProgressSerializer::deserialize(QStack<Model::Move> undo, QString filePath){
    qDebug("Deserializing current progress...");
    Puzzle* puzzle = new Puzzle();

    std::string refFilePath;
    std::ifstream getRefFile(filePath.toLatin1());
    getRefFile >> refFilePath;
    getRefFile.close();
    qDebug("Retrieved reference file path:");
    QString qPath = QString::fromStdString(refFilePath);
    qDebug(qPath.toLatin1());

    char comma;
    qDebug("Setting Default and Solved");

    std::ifstream oFile(qPath.toLatin1());

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            oFile >> puzzle->defaultBoard[i][j] >> comma;
        }
        oFile.ignore();
    }
    oFile.ignore();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            oFile >> puzzle->solvedBoard[i][j] >> comma;
        }
        oFile.ignore();
    }

    oFile.close();

    //Reopen original filepath, set current
    qDebug("Reopening original to set current");

    std::ifstream file(filePath.toLatin1());
    file >> refFilePath;
    file.ignore();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->currentBoard[i][j] >> comma;
        }
        file.ignore();
    }

    file.ignore();

    int x,y,val;
    while(file >> x >> comma >> y >> comma >> val >> comma) {
        Move* mv = new Move(x,y,val);
        undo.push_back(*mv);
    }

    qDebug("Done.");
    file.close();
    puzzle->filePathRef = qPath;
    return puzzle;
}
