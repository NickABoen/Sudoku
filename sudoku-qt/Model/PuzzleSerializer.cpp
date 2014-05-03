#include <iostream>
#include <fstream>

#include "PuzzleSerializer.h"
#include "../test.h"
#include "qdebug.h"
#include <string.h>
#include <QDebug>

using namespace Model;

//////////////////////////////////////////////////////////////////////////////////
// Constructor for PuzzleSerializer
//////////////////////////////////////////////////////////////////////////////////
PuzzleSerializer::PuzzleSerializer(){


}

//////////////////////////////////////////////////////////////////////////////////
// Destructor for PuzzleSerializer
//////////////////////////////////////////////////////////////////////////////////
PuzzleSerializer::~PuzzleSerializer(){


}

//////////////////////////////////////////////////////////////////////////////////
// This function creates a text file representing a unplayed puzzle.
//////////////////////////////////////////////////////////////////////////////////
void PuzzleSerializer::serialize(Puzzle* puzzle, QString filePath)
{

    std::ofstream file;
    file.open(filePath.toLatin1());
    // Write default board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->defaultBoard[j][i] << ",";
        }
        file << "\n";
    }
    file << "\n";
    // Write solved board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file << puzzle->solvedBoard[j][i] << ",";
        }
        file << "\n";
    }

    // Write Scoreboard List
    //TODO: Fix Serializer
    qDebug() << "Saving Scoreboard...";
    file << puzzle->scoreboardList.count() << "\n";
    std::string player;
    for(int i = 0; i < puzzle->scoreboardList.count(); i++){
        //if(test) testfile << "CPS7  Redo move saved\n";
        QPair<QString,int> scorePair = puzzle->scoreboardList.at(i);
        file << scorePair.first.toUtf8().constData() << "\n" << scorePair.second << "\n";
        scorePair.first.fromStdString(player);
        qDebug()<<"\t" << scorePair.first.toUtf8().constData() << "\t" << scorePair.second;
    }

    puzzle->filePathRef = filePath;
    file.close();
}

//////////////////////////////////////////////////////////////////////////////////
// This function creates a puzzle from a text file that represents a previously
// unplayed puzzle.
//////////////////////////////////////////////////////////////////////////////////
Puzzle* PuzzleSerializer::deserialize(QString filePath){


    Puzzle *puzzle = new Puzzle();

    std::ifstream file(filePath.toLatin1());
    char comma;

    // Read in default board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->defaultBoard[j][i] >> comma;
        }
        file.ignore();
    }
    file.ignore();
    // Read in solved board
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            file >> puzzle->solvedBoard[j][i] >> comma;
        }
        file.ignore();
    }

    // Read Scoreboard List
    qDebug()<<"Reading Scoreboard";
    int count, score;
    std::string player;
    file >> count;
    for(int i = 0; i < count; i++){
        //if(test) testfile << "CPS11 Redo move set\n";
        file >> player >> score;
        puzzle->scoreboardList.append(*(new QPair<QString,int>(QString::fromStdString(player), score)));
        qDebug() << "\t" << puzzle->scoreboardList.at(i).first.toUtf8().constData() << "\t" << puzzle->scoreboardList.at(i).second;
    }
    qDebug()<<"Finished Reading Scoreboard";
    file.close();

    puzzle->filePathRef = filePath;
    return puzzle;
}

