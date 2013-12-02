#ifndef BOARDGENERATOR_H
#define BOARDGENERATOR_H
#include <list>
#include "BoardSolver.h"
using namespace Model;
class BoardGenerator {
private:
    //internal representation of current Board
    int** Board;
    //internal representation of Final Board Solution
    int** FinalBoard;
    //Enum class specifying the difficulty of the
    //Generated Board.
    //enum class Difficulty{Easy, Medium, Hard};
public:
    //Board Generator Constructor;
    BoardGenerator();
    //Board Generator Descructor;
    ~BoardGenerator(void);
    //Function Generates a Board based on specified
    //Difficulty.
    void Generate(int);
    //Function Swaps two rows, based on the
    //two row indicies given.
    void SwapRows(int,int);
    //Function Swaps two Columns, based on the
    //two column indicies given.
    void SwapColumns(int,int);
    //Function that removes a value at the given
    //Section and location and returns true if
    //a value was removed.
    bool RemoveValue(int,int);
    //Calculates the row when given a section
    //and location.
    int GetRow(int,int);
    //Calculates the Column when given a section
    //and location.
    int GetColumn(int,int);
    //bool CheckSolution(void);
    //Compares the Final Board with a provied
    //Board and returns true if they are
    //the same.
    bool CompareBoards(int**);
};
#endif
