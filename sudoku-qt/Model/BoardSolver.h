#ifndef BOARDSOLVER_H
#define BOARDSOLVER_H
#include "PriorityQueue.h"
namespace Model{
    class BoardSolver {
    private:
        //a boolean array that represents
        //each section of a sudoku board
        //and each array contains an array
        //of boolean values that show if
        //the values 1-9 are in that
        //section or not.
        bool **sectionContains;
        //a boolean array that represents
        //each row of a sudoku board
        //and each array contains an array
        //of boolean values that show if
        //the values 1-9 are in that
        //row or not.
        bool **rowContains;
        //a boolean array that represents
        //each column of a sudoku board
        //and each array contains an array
        //of boolean values that show if
        //the values 1-9 are in that
        //column or not.
        bool **columnContains;
        //represents the Board to be solved
        int **defaultBoard;
        //represents the Final Solved Board
        int **board;
    public:
        //Constructor that takes in a Board
        //and sets the Board to the
        //defaultBoard variable.
        BoardSolver(int**);
        //Destructor for the Board Solver.
        ~BoardSolver(void);
        //Function that builds and fills a priority queue
        //containing the empty cells, with cells having a
        //priority equal to the sum of the values in the
        //given arrays corresponding to its section,
        //row, and columns.
        PriorityQueue FillPriorityQueue(int*, int*, int*);
        //tries to add a value to given secion and location
        bool AddValue(int,int,int);
        //Increases the priorityies of each available cell
        //in the same section, row, and column as the cell
        //in the cell in the given section and location
        void IncreasePriorities(PriorityQueue, int, int);
        //Decreases the priorityies of each available cell
        //in the same section, row, and column as the cell
        //in the cell in the given section and location
        void DecreasePriorities(PriorityQueue, int, int);
        //looks for a solution to the puzzle, assuming there
        //are no conflicts.  If a solution is found, leaves
        //that solution in the Final Solved Board and
        //returns true if was successful.
        bool FillBoard(PriorityQueue);
        //Tries to solve the Board
        int** Solve(void);
        //Calculates the row when given a section
        //and location.
        int GetRow(int,int);
        //Calculates the Column when given a section
        //and location.
        int GetColumn(int,int);
    };
}
#endif
