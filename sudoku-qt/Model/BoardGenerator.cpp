#include "PriorityQueue.h"
#include "BoardGenerator.h"
#include "BoardSolver.h"

#include <iostream>
#include <map>
#include <limits.h>
#include <list>
#include <QtDebug>
#include <ctime>

using namespace Model;

//////////////////////////////////////////////////////////////////////////////////
// Constructor for BoardGenerator
//////////////////////////////////////////////////////////////////////////////////
BoardGenerator::BoardGenerator(){
    GBoard = new int*[9];
    FinalBoard = new int*[9];
    for(int i = 0; i < 9; i++){
        GBoard[i] = new int[9];
        FinalBoard[i] = new int[9];
    }
    for(int i = 0; i<9; i++){
        for(int j = 0; j < 9; j++){
            GBoard[i][j] = 0;
            FinalBoard[i][j] = 0;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// Destructor for BoardGenerator
//////////////////////////////////////////////////////////////////////////////////
BoardGenerator::~BoardGenerator(){
//    for(int i = 0; i<9; i++){
//        delete(GBoard[i]);
//        delete(FinalBoard[i]);
//    }
//    delete(GBoard);
//    delete(FinalBoard);
}

//////////////////////////////////////////////////////////////////////////////////
// This function generates a board based on specified difficulty
//////////////////////////////////////////////////////////////////////////////////
int** BoardGenerator::Generate(int Diff){
    srand ( time(NULL) );
    BoardSolver *BS = new BoardSolver(GBoard);
    GBoard = BS->Solve();
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
    for(int i = 0; i<9; i++){
        for(int j = 0; j < 9; j++){
            int row = GetRow(i,j);
            int col = GetColumn(i,j);

            FinalBoard[row][col] = GBoard[row][col];
        }
    }
    int RemoveNUM;
    if(Diff == 1){

        RemoveNUM = 20;
    }else if(Diff == 2){
        RemoveNUM = 22;
    }
    else if(Diff == 3){
        RemoveNUM = 24;
    }
    for(int i = 0; i<RemoveNUM; i++){
        int randSec = rand() % 9;
        int randLoc = rand() % 9;
        while(!RemoveValue(randSec, randLoc)){
            randSec = rand() % 9;
            randLoc = rand() % 9;
        }
    }
    return GBoard;
}

//////////////////////////////////////////////////////////////////////////////////
// This function swaps two columns based on indicies
//////////////////////////////////////////////////////////////////////////////////
void BoardGenerator::SwapColumns(int c1, int c2){
    int temp;
    for(int j = 0; j < 9; j++){
        int section1 = GetSection(j,c1);
        int location1 = GetLocation(j,c1);
        int location2 = GetLocation(j,c2);
        temp = GBoard[section1][location1];
        GBoard[section1][location1] = GBoard[section1][location2];
        GBoard[section1][location2] = temp;
    }
}
//////////////////////////////////////////////////////////////////////////////////
// This function swaps two rows based on indicies
//////////////////////////////////////////////////////////////////////////////////
void BoardGenerator::SwapRows(int r1, int r2){
    int temp;
    for(int i = 0; i < 9; i++){
        int section1 = GetSection(r1,i);
        int location1 = GetLocation(r1,i);
        int location2 = GetLocation(r2,i);
        temp = GBoard[section1][location1];
        GBoard[section1][location1] = GBoard[section1][location2];

        GBoard[section1][location2] = temp;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function removes a falue from the given location and returns true if the
// was removed
//////////////////////////////////////////////////////////////////////////////////
bool BoardGenerator::RemoveValue(int sec, int loc){
    int S1 = sec;
    int L1 = loc;
    int x2 = 8-GetRow(sec, loc);
    int y2 = 8-GetColumn(sec,loc);
    int S2 = GetSection(x2,y2);
    int L2 = GetLocation(x2,y2);
    if(GBoard[S1][L1] != 0){
        int val1 = GBoard[S1][L1];
        if(GBoard[S2][L2] != 0){
            int val2 = GBoard[S2][L2];

            GBoard[S1][L1] = 0;
            GBoard[S2][L2] = 0;
            BoardSolver *BS = new BoardSolver(GBoard);
            int **SolvedBoard = new int*[9];
            for(int i = 0; i <9; i++){
                SolvedBoard[i] = new int[9];
            }
            SolvedBoard = BS->Solve();
            if(!CompareBoards(SolvedBoard)){
                GBoard[S1][L1] = val1;
                GBoard[S2][L2] = val2;
                return false;
            }
            delete(BS);
            return true;
        }
        return false;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////
// This function calculates the row of a selection and location
//////////////////////////////////////////////////////////////////////////////////
int BoardGenerator::GetRow(int section, int loc){
    return 3*(section/3)+loc/3;
}

//////////////////////////////////////////////////////////////////////////////////
// This function calculates the column of a selection and location
//////////////////////////////////////////////////////////////////////////////////
int BoardGenerator::GetColumn(int section, int loc){
    return 3*(section%3)+loc%3;
}

//////////////////////////////////////////////////////////////////////////////////
// This function calculates the selection of the cartesian coordinates
//////////////////////////////////////////////////////////////////////////////////
int BoardGenerator::GetSection(int x, int y){
    return 3*(x/3)+y/3;
}
//////////////////////////////////////////////////////////////////////////////////
// This function calculates the location of the cartesian coordinates
//////////////////////////////////////////////////////////////////////////////////
int BoardGenerator::GetLocation(int x, int y){
    return 3*(x%3)+y%3;
}

//////////////////////////////////////////////////////////////////////////////////
// This function compares the the given board with the final board to see if they
// the same
//////////////////////////////////////////////////////////////////////////////////
bool BoardGenerator::CompareBoards(int** ComparedBoard){
    for(int i = 0; i< 9; i++){
        for(int j = 0; j < 9; j++){
            int x = GetRow(i,j);
            int y = GetColumn(i,j);

            if(ComparedBoard[x][y] != FinalBoard[x][y])return false;
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////
// This function converts a board from selection location coordinates to cartesian
// coordinates
//////////////////////////////////////////////////////////////////////////////////
int** BoardGenerator::ConvertBoard(int** BoardToConvert){
    int** ConvertedBoard = new int*[9];
    for(int i = 0; i < 9; i++){
        ConvertedBoard[i] = new int[9];
    }
    for(int i = 0; i<9;i++){
        for(int j = 0; j<9; j++){
            int row = GetRow(i,j);
            int column = GetColumn(i,j);
            ConvertedBoard[row][column] = BoardToConvert[i][j];
        }
    }
    return ConvertedBoard;
}

