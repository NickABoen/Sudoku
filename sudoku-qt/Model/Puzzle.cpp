#include "Puzzle.h"

using namespace Model;

Puzzle::Puzzle():
    currentBoard(NULL),
    defaultBoard(NULL),
    solvedBoard(NULL)
{
    //TODO
}

Puzzle::~Puzzle(){
    //TODO
}

bool Puzzle::checkCompleted(){
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            if(currentBoard[i][j] != solvedBoard[i][j]) return false;
        }
    }
    return true;
}

void Puzzle::makeMove(Move move){
    currentBoard[move.x][move.y] = move.value;
}

