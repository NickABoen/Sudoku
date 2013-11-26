#include "CurrentProgressSerializer.h"

using namespace Model;

CurrentProgressSerializer::CurrentProgressSerializer(){
    //TODO
}

CurrentProgressSerializer::~CurrentProgressSerializer(){
    //TODO
}

void CurrentProgressSerializer::serialize(Puzzle* puzzle, QString filePath){

    qDebug("Serializing current progress...");
    std::ofstream file;
    file.open(filePath.toLatin1());

    if (!puzzle->filePathRef.isEmpty()) {
        //shouldn't be empty, but will mess up either way if it is.
        file << puzzle->filePathRef.toUtf8().constData();
        file << "\n\n";
    }

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->currentBoard[i][j] << ",";
        }
        file << "\n";
    }
    file << "\n";

    file << puzzle->undo.count() << "\n";
    for(int i = 0; i < puzzle->undo.count(); i++){
        Move mv = puzzle->undo[i];
        file << mv.x << "," << mv.y << "," << mv.value << ",";
        file << "\n";
    }
    file << "\n";

    file << puzzle->redo.count() << "\n";
    for(int i = 0; i < puzzle->redo.count(); i++){
        Move mv = puzzle->redo[i];
        file << mv.x << "," << mv.y << "," << mv.value << ",";
        file << "\n";
    }

    qDebug("Finished serializing progress...");
    file.close();
}

Puzzle* CurrentProgressSerializer::deserialize(QString filePath, PuzzleSerializer puzzleSerializer){
    qDebug("Deserializing current progress...");
    Puzzle* puzzle;

    std::string refFilePath;
    std::ifstream getRefFile(filePath.toLatin1());
    getRefFile >> refFilePath;
    getRefFile.close();
    qDebug("Retrieved reference file path:");
    QString qPath = QString::fromStdString(refFilePath);
    qDebug(qPath.toLatin1());

    char comma;
    qDebug("Setting Default and Solved");

    puzzle = puzzleSerializer.deserialize(qPath);

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

    int count, x, y, val;
    file >> count;
    for(int i = 0; i < count; i++){
        file >> x >> comma >> y >> comma >> val >> comma;
        Move* mv = new Move(x,y,val);
        puzzle->undo.push(*mv);
    }

    file.ignore();

    file >> count;
    for(int i = 0; i < count; i++){
        file >> x >> comma >> y >> comma >> val >> comma;
        Move* mv = new Move(x,y,val);
        puzzle->redo.push(*mv);
    }

    qDebug("Done.");
    file.close();
    puzzle->filePathRef = qPath;
    return puzzle;
}
