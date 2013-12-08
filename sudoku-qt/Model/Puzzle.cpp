#include "Puzzle.h"
#include "../test.h"

#include <QString>

using namespace Model;

//////////////////////////////////////////////////////////////////////////////////
// Constructor for Puzzle
//////////////////////////////////////////////////////////////////////////////////
Puzzle::Puzzle():
    currentBoard(new int*[9]),
    defaultBoard(new int*[9]),
    solvedBoard(new int*[9]),
    undo(),
    redo()
{

    if(test) testfile << "P1  ####################### Puzzle constructor #######################\n";

    for(int i = 0; i < 9; ++i){
        currentBoard[i] = new int[9];
        defaultBoard[i] = new int[9];
        solvedBoard[i] = new int[9];

        for(int j = 0; j < 9; j++)
        {
            if(test) testfile << "P2 Puzzle at " << i << "," << j << " initialized\n";
            currentBoard[i][j] = 0;
            defaultBoard[i][j] = 0;
            solvedBoard[i][j] = 0;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// Destructor for Puzzle
//////////////////////////////////////////////////////////////////////////////////
Puzzle::~Puzzle(){

    if(test) testfile << "P3  ####################### Puzzle constructor #######################\n";

}

//////////////////////////////////////////////////////////////////////////////////
// This function checks if the current board matches the solved board, in other
// words, if the puzzle has been completed.
//////////////////////////////////////////////////////////////////////////////////
bool Puzzle::checkCompleted(){

    if(test) testfile << "P4  ####################### Puzzle checkCompleted #######################\n";

    // Compare current board to solved board
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(test) testfile << "P5 Puzzle at " << i << "," << j << " checked if correct\n";
            if(currentBoard[i][j] != solvedBoard[i][j]){
                if(test) testfile << "P6 Puzzle not finished/correct\n";
                return false;
            }
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////
// This function is called whenever a move is made to update the current board.
//////////////////////////////////////////////////////////////////////////////////
void Puzzle::makeMove(Move move){

    if(test) testfile << "P7  ####################### Puzzle makeMove #######################\n";

    currentBoard[move.x][move.y] = move.value;
}

