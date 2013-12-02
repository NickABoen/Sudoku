#include "PriorityQueue.h"
#include "BoardGenerator.h"
#include "BoardSolver.h"
#include <iostream>
#include <map>
#include <limits.h>
#include <list>

using namespace Model;
//Board Generator constructor
BoardGenerator::BoardGenerator(){
    Board = new int*[9];
    FinalBoard = new int*[9];
    for(int i = 0; i < 9; i++){
        Board[i] = new int[9];
        FinalBoard[i] = new int[9];
    }
    for(int i = 0; i<9; i++){
        for(int j = 0; j < 9; j++){
            Board[i][j] = 0;
            FinalBoard[i][j] = 0;
        }
    }
}
//Board Generator Destructor
BoardGenerator::~BoardGenerator(){
    for(int i = 0; i<9; i++){
        delete(Board[i]);
        delete(FinalBoard[i]);
    }
    delete(Board);
    delete(FinalBoard);
}
//Function Generates a Board based on specified
//Difficulty.
void BoardGenerator::Generate(int Diff){
    for(int i = 0;  i < 9; i++){
        int row;
        int col;
        do{
            int random = rand() % 9;
            row = GetRow(i,random);
            col = GetColumn(i,random);
        }while(Board[row][col] != 0);
        Board[row][col] = i+1;
    }
    BoardSolver *BS = new BoardSolver(Board);
    Board = BS->Solve();
    delete BS;
    for(int i = 0; i < 9; i+=3){
        int col1 = rand() % 3;
        int col2 = rand() % 3;
        while(col2 == col1){ col2 = rand() % 3;}
        SwapColumns(col1+i,col2+i);
        int row1 = rand() % 3;
        int row2 = rand() % 3;
        while(row2 == row1){ row2 = rand() % 3;}
        SwapRows(row1+i,row2+i);
    }
    for(int i =0; i< 9; i++){
        for(int j=0; j<9; j++){
            FinalBoard[i][j] = Board[i][j];
        }
    }
    int RemoveNUM;
    if(Diff == 1){
        RemoveNUM = 40;
    }else if(Diff == 2){
        RemoveNUM = 50;
    }
    else if(Diff == 3){
        RemoveNUM = 60;
    }
    RemoveNUM = RemoveNUM/2;
    for(int i = 0; i<RemoveNUM; i++){
            int randSection = rand() % 9;
            int randLocation = rand() % 9;
        while(!RemoveValue(randSection, randLocation)){
            randSection = rand() % 9;
            randLocation = rand() % 9;
        }
    }
}
//Function Swaps two Columns, based on the
//two column indicies given.
void BoardGenerator::SwapColumns(int c1, int c2){
    int temp;
    for(int j = 0; j < 9; j++){
        temp = Board[c1][j];
        Board[c1][j] = Board[c2][j];
        Board[c2][j] = temp;
    }
}
//Function Swaps two rows, based on the
//two row indicies given.
void BoardGenerator::SwapRows(int r1, int r2){
    int temp;
    for(int i = 0; i < 9; i++){
        temp = Board[i][r1];
        Board[i][r1] = Board[i][r2];
        Board[i][r2] = temp;
    }
}
//Function that removes a value at the given
//Section and location and returns true if
//a value was removed.
bool BoardGenerator::RemoveValue(int section, int loc){
    int X1 = GetRow(section, loc);
    int Y1 = GetColumn(section, loc);
    int X2 = 8-X1;
    int Y2 = 8-Y1;
    if(Board[X1][Y1] != 0){
        int val1 = Board[X1][Y1];
        if(Board[X2][Y2] != 0){
            int val2 = Board[X2][Y2];
            Board[X1][Y1] = 0;
            Board[X2][Y2] = 0;
            BoardSolver *BS = new BoardSolver(Board);
            int**SolvedBoard = BS->Solve();
            if(!CompareBoards(SolvedBoard)){
                Board[X1][Y1] = val1;
                Board[X2][Y2] = val2;
                return false;
            }
            delete(BS);
            return true;
        }
        return false;
    }
    return false;
}
//Calculates the row when given a section
//and location.
int BoardGenerator::GetRow(int section, int loc){
    return 3*(section/3)+loc/3;
}
//Calculates the Column when given a section
//and location.
int BoardGenerator::GetColumn(int section, int loc){
    return 3*(section%3)+loc%3;
}
/*bool BoardGenerator::CheckOneSolution(){
    BoardSolver *BS = new BoardSolver(Board);
    int **temp = BS->Solve;
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(!temp[i][j] == FinalBoard[i][j]) return false;
        }
    }
    delete(BS);
    return true;
}*/
//Compares the Final Board with a provied
//Board and returns true if they are
//the same.
bool BoardGenerator::CompareBoards(int** ComparedBoard){

    for(int i = 0; i< 9; i++){
        for(int j = 0; j < 9; j++){
            if(ComparedBoard[i][j] != FinalBoard[i][j]) return false;
        }
    }
    return true;
}
