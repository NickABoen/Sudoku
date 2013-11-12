#include "Puzzle.h"

#include <QString>

using namespace Model;

Puzzle::Puzzle():
    currentBoard(new int*[9]),
    defaultBoard(new int*[9]),
    solvedBoard(new int*[9])
{
    for(int i = 0; i < 9; ++i){
        currentBoard[i] = new int[9];
        defaultBoard[i] = new int[9];
        solvedBoard[i] = new int[9];

        for(int j = 0; j < 9; j++)
        {
            currentBoard[i][j] = 0;
            defaultBoard[i][j] = 0;
            solvedBoard[i][j] = 0;
        }
    }

    qDebug("Current Board [0][0] = " + QString::number(currentBoard[0][0]).toLatin1());
}

Puzzle::~Puzzle(){
    //TODO
}

bool Puzzle::checkCompleted(){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(currentBoard[i][j] != solvedBoard[i][j]) return false;
        }
    }
    return true;
}

void Puzzle::makeMove(Move move){
    currentBoard[move.x][move.y] = move.value;
}

