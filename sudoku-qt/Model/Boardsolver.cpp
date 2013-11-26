#include "BoardSolver.h"
#include "PriorityQueue.h"
#include <iostream>
#include <string>
namespace Model{
    //Constructor that allocates memory for
    //all the initial variables and arrays.
    //inputBoard is the board to be solved.
    BoardSolver::BoardSolver(int** inputBoard){
        sectionContains = new bool*[9];
        rowContains = new bool*[9];
        columnContains = new bool*[9];
        defaultBoard = inputBoard;
        board = new int*[9];
        for(int i = 0; i < 9; i++){
            sectionContains[i] = new bool[10];
            rowContains[i] = new bool[10];
            columnContains[i] = new bool[10];
            //defaultBoard[i] = new int[9];
            board[i] = new int[9];
        }
        for(int i = 0; i<9; i++){
            for(int j = 1; j < 10; j++){
                sectionContains[i][j] = false;
                rowContains[i][j] = false;
                columnContains[i][j] = false;
            }
        }
    }
    //Destructor for the BoardSolver
    BoardSolver::~BoardSolver(){
        for(int i = 0; i < 9; i++){
            delete(sectionContains[i]);
            delete(rowContains[i]);
            delete(columnContains[i]);
            delete(defaultBoard[i]);
            delete(board[i]);
        }
        delete(sectionContains);
        delete(rowContains);
        delete(columnContains);
        delete(defaultBoard);
        delete(board);
    }
    //Function that builds and fills a priority queue
    //containing the empty cells, with cells having a
    //priority equal to the sum of the values in the
    //given arrays corresponding to its section,
    //row, and columns.
    PriorityQueue BoardSolver::FillPriorityQueue(int* sectionOccupied, int* rowOccupied, int* columnOccupied){
        PriorityQueue availableCells(81);
        for(int i = 0; i < 9; i++){
            for(int j = 0; j < 9; j++){
                if(board[i][j] == 0){
                    int pri = sectionOccupied[i]+rowOccupied[GetRow(i,j)]+columnOccupied[GetColumn(i,j)];
                    availableCells.Add(new QPoint(i,j),pri);
                }
            }
        }
        return availableCells;
    }
    //tries to add a value to given secion and location
    bool BoardSolver::AddValue(int section,int loc, int val){
        if(sectionContains[section][val])return false;
        int row = GetRow(section, loc);
        if(rowContains[row][val])return false;
        int col = GetColumn(section, loc);
        if(columnContains[col,val])return false;
        board[section][loc] = val;
        sectionContains[section][val] = true;
        rowContains[row][val] = true;
        columnContains[col][val] = true;
        return true;
    }
    //Increases the priorityies of each available cell
    //in the same section, row, and column as the cell
    //in the cell in the given section and location
    void BoardSolver::IncreasePriorities(PriorityQueue availableCells, int section, int loc){
        int row = GetRow(section, loc);
        int col = GetColumn(section, loc);
        for(int i = 0; i < 9; i++){
            for(int j = 0; j < 9; j++){
                if(board[i][j] == 0){
                    QPoint* p = new QPoint(i,j);
                    if(i == section) availableCells.IncreasePriority(p);
                    int thisRow = GetRow(i,j);
                    int thisCol = GetColumn(i,j);
                    if(thisRow = row) availableCells.IncreasePriority(p);
                    if(thisCol = col) availableCells.IncreasePriority(p);
                }
            }
        }
    }
    //Decreases the priorityies of each available cell
    //in the same section, row, and column as the cell
    //in the cell in the given section and location
    void BoardSolver::DecreasePriorities(PriorityQueue availableCells, int section, int loc){
        int row = GetRow(section, loc);
        int col = GetColumn(section, loc);
        for(int i = 0; i < 9; i++){
            for(int j = 0; j < 9; j++){
                if(board[i][j] == 0){
                    QPoint* p = new QPoint(i,j);
                    if(i == section) availableCells.DecreasePriority(p);
                    int thisRow = GetRow(i,j);
                    int thisCol = GetColumn(i,j);
                    if(thisRow = row) availableCells.DecreasePriority(p);
                    if(thisCol = col) availableCells.DecreasePriority(p);
                }
            }
        }
    }
    //looks for a solution to the puzzle, assuming there
    //are no conflicts.  If a solution is found, leaves
    //that solution in the Final Solved Board and
    //returns true if was successful.
    bool BoardSolver::FillBoard(PriorityQueue availableCells){
        if(availableCells.count == 0) return true;
        else{
            int priority = availableCells.MaxPriority();
            QPoint* cell = availableCells.RemoveMax();
            int section = cell->x();
            int loc = cell->y();
            board[section][loc] = 1;
            IncreasePriorities(availableCells, section, loc);
            for(int i = 1; i <=9; i++){
                if(AddValue(section, loc, i)){
                    if(FillBoard(availableCells)) return true;
                    else{
                        sectionContains[section][i] = false;
                        rowContains[GetRow(section,loc)][i] = false;
                        columnContains[GetColumn(section,loc)][i] = false;

                    }
                }
            }
            DecreasePriorities(availableCells, section, loc);
            availableCells.Add(cell,priority);
            board[section][loc] = 0;
            return false;
        }
    }
    //Tries to solve the Board
    int** BoardSolver::Solve(){
        int sectionOccupied[9];
        int rowOccupied[9];
        int collumnOccupied[9];
        for(int i = 0; i < 9; i++){
            for(int j = 0; j < 9; j++){
                if(defaultBoard[i][j] > 0){
                    if(!AddValue(i,j,defaultBoard[i][j])) break;
                    sectionOccupied[i]++;
                    rowOccupied[GetRow(i,j)]++;
                    collumnOccupied[GetColumn(i,j)]++;
                }
            }
        }
        PriorityQueue availableCells  = FillPriorityQueue(sectionOccupied, rowOccupied, collumnOccupied);
        if(FillBoard(availableCells)) return board;
        else return defaultBoard;
    }
    //Calculates the row when given a section
    //and location.
    int BoardSolver::GetRow(int section, int loc){
        return 3*(section%3)+loc%3;
    }
    //Calculates the Column when given a section
    //and location.
    int BoardSolver::GetColumn(int section, int loc){
        return 3*(section/3)+loc/3;
    }
}
